#include <gtest/gtest.h>
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <list>
#include <memory>
#include "../src/unit_of_work.h"
#include "../src/drawing_mapper.h"
#include "../src/painter_mapper.h"
#include "../src/drawing.h"

#include "../src/scanner.h"
#include "../src/parser.h"
#include "../src/builder.h"

class DBSuite : public ::testing::Test
{
protected:
    void SetUp() override
    {
        create_drawing_table();
        create_painter_table();
        populate_drawings();
        populate_painters();
        dm = DrawingMapper::instance();
        dm->setDB("resource/drawing.db");
        pm = PainterMapper::instance();
        pm->setDB("resource/painter.db");
    }

    void TearDown() override
    {
        drop_drawing_table();
        drop_painter_table();
        ASSERT_EQ(SQLITE_OK,sqlite3_close(db));
        sqlite3_close(db_p);
    }

    void create_drawing_table()
    {
        ASSERT_EQ(0, sqlite3_open("resource/drawing.db", &db));
        const char *stmt = "CREATE TABLE drawing"\
                           "(ID         char(6) PRIMARY KEY     not null,"\
                           "painter    varchar(50)             not null,"\
                           "shapes varchar(1028),"\
                           "FOREIGN KEY(painter) REFERENCES painter(ID))";
        int rc = sqlite3_exec(db, stmt, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
    }

    void drop_drawing_table()
    {
        ASSERT_EQ(SQLITE_OK,sqlite3_open("resource/drawing.db", &db));
        const char *dropStmt = "DROP TABLE drawing";
        int rc = sqlite3_exec(db, dropStmt, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
    }

    void create_painter_table()
    {
        ASSERT_EQ(0, sqlite3_open("resource/painter.db", &db_p));
        const char *stmt = "CREATE TABLE painter ("
                           "ID         char(6) PRIMARY KEY     not null,"
                           "name    varchar(50)             not null)";
        int rc = sqlite3_exec(db_p, stmt, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
    }

    void drop_painter_table()
    {
        const char *dropStmt = "DROP TABLE painter";
        int rc = sqlite3_exec(db_p, dropStmt, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
    }

    void populate_drawings()
    {
        const char *s1 = "INSERT INTO drawing"
                         "(ID, painter, shapes)"
                         "values"
                         "('d_0001', 'p_0001', 'triangle 1 1 1')";
        int rc = sqlite3_exec(db, s1, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
        ASSERT_EQ(SQLITE_OK, rc);

        const char *s2 = "INSERT INTO drawing"
                         "(ID,  painter,   shapes)"
                         "values"
                         "('d_0002', 'p_0001', 'triangle 2 2 2')";
        rc = sqlite3_exec(db, s2, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
        ASSERT_EQ(SQLITE_OK, rc);

        const char *s3 = "INSERT INTO drawing"
                         "(ID,  painter,   shapes)"
                         "values"
                         "('d_0003', 'p_0002', 'compound { triangle 1 1 1 triangle 2 2 2 } triangle 3 3 3')";
        rc = sqlite3_exec(db, s3, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
        ASSERT_EQ(SQLITE_OK, rc);
    }

    void populate_painters()
    {
        const char *s1 = "INSERT INTO painter"
                         "(ID, name)"
                         "values"
                         "('p_0001', 'Patrick')";
        int rc = sqlite3_exec(db_p, s1, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
        ASSERT_EQ(SQLITE_OK, rc);

        const char *s2 = "INSERT INTO painter"
                         "(ID, name)"
                         "values"
                         "('p_0002', 'Mary')";
        rc = sqlite3_exec(db_p, s2, NULL, NULL, &err_msg);
        display_err_msg_and_fail_if_any(rc);
        ASSERT_EQ(SQLITE_OK, rc);
    }

    void display_err_msg_and_fail_if_any(int rc)
    {
        if (rc)
        {
            std::cout << "db error: " << err_msg << std::endl;
            sqlite3_free(err_msg);
        }
        ASSERT_EQ(SQLITE_OK, rc);
    }

    sqlite3 *db;
    sqlite3 *db_p;
    char *err_msg = nullptr;
    DrawingMapper *dm;
    PainterMapper *pm;
};

TEST_F(DBSuite, Sanity)
{
}

TEST_F(DBSuite, findDrawing)
{
    Drawing *drawing = dm->find("d_0001");

    EXPECT_TRUE(UnitOfWork::instance()->inClean("d_0001"));
    EXPECT_FALSE(UnitOfWork::instance()->inDirty("d_0001"));
    EXPECT_TRUE(UnitOfWork::instance()->inClean("p_0001"));
    EXPECT_FALSE(UnitOfWork::instance()->inDirty("p_0001"));
    ASSERT_EQ(drawing->id(), "d_0001");
    ASSERT_EQ(drawing->getShape(0)->perimeter(), 3);
    ASSERT_EQ(drawing->painter()->id(), "p_0001");
    ASSERT_EQ(drawing->painter()->name(), "Patrick");

    delete drawing;
}


TEST_F(DBSuite, UnitOfWorkRegisterNew) {
  Painter * painter = new Painter("p_777", "Luca");
  UnitOfWork::instance()->registerNew(painter);
  ASSERT_TRUE(UnitOfWork::instance()->inNew("p_777"));
  UnitOfWork::instance()->commit();
  ASSERT_FALSE(UnitOfWork::instance()->inNew("p_777"));
  ASSERT_TRUE(UnitOfWork::instance()->inClean("p_777"));

  delete painter;
}

TEST_F(DBSuite, FindDrawingAndUPDATE)
{
    Drawing *drawing = dm->find("d_0002");
    Painter *painter = new Painter("p_0002","Luca");

    drawing->setPainter(painter);
    EXPECT_FALSE(UnitOfWork::instance()->inClean("d_0002"));
    EXPECT_TRUE(UnitOfWork::instance()->inDirty("d_0002"));

    UnitOfWork * uow = UnitOfWork::instance();

    uow->commit();

    Drawing *D = dm->find("d_0002");

    ASSERT_EQ(drawing->id(), "d_0002");
    ASSERT_EQ(drawing->getShape(0)->perimeter(), 6);
    ASSERT_EQ(drawing->painter()->id(), "p_0002");
    ASSERT_EQ(drawing->painter()->name(), "Luca");

    EXPECT_TRUE(UnitOfWork::instance()->inClean("d_0002"));

    delete drawing;
    delete painter;
    delete uow;
}

TEST_F(DBSuite, ADDUOWAllNewandFind){
    std::string input = "compound {\n  triangle 2.0 2.0 2.0\n  triangle 3.0 3.0 3.0\n}";
    Scanner *scanner = new Scanner();
    Builder * builder = new Builder();
    Parser *parser = new Parser(scanner, builder);
    parser->setInput(input);
    parser->parse();
    std::list<Shape *>shapes = parser->getShapes();

    Painter *painter = new Painter("p_999","Luca");
    // DomainObject *drawing = new Drawing("d_999",painter,shapes);

    // UnitOfWork * uow = UnitOfWork::instance();

    // uow->commit();

    // Drawing *D = dm->find("d_999");

    // ASSERT_EQ(D->id(), "d_999");
    // ASSERT_EQ(D->getShapesAsString(), "compound { triangle 2 2 2 triangle 3 3 3 }\n");
    // ASSERT_EQ(D->painter()->id(), "p_999");
    // ASSERT_EQ(D->painter()->name(), "Luca");

    // Painter *P = pm->find("p_999");

    // ASSERT_EQ(P->id(), "p_999");
    // ASSERT_EQ(P->name(), "Luca");

    // ASSERT_TRUE(UnitOfWork::instance()->inClean("d_999"));
    // ASSERT_TRUE(UnitOfWork::instance()->inClean("p_999"));

    // delete scanner;
    // delete builder;
    // delete parser;
    // delete drawing;
    // delete painter;
    // delete uow;
}


// TEST_F(DBSuite, ADDUOWNewOldandFind){
//     std::string input = "compound {\n  triangle 2.0 2.0 2.0\n  triangle 3.0 3.0 3.0\n}";
//     Scanner *scanner = new Scanner();
//     Builder * builder = new Builder();
//     Parser *parser = new Parser(scanner, builder);
//     parser->setInput(input);
//     parser->parse();
//     std::list<Shape *>shapes = parser->getShapes();

//     Painter *painter = pm->find("p_0001");

//     Painter *Patrick = new Painter("p_0001","Patrick");
//     DomainObject *drawing = new Drawing("d_888",Patrick,shapes);

//     UnitOfWork * uow = UnitOfWork::instance();

//     uow->commit();

//     Drawing *D = dm->find("d_888");

//     ASSERT_EQ(D->id(), "d_888");
//     ASSERT_EQ(D->getShapesAsString(), "compound { triangle 2 2 2 triangle 3 3 3 }\n");
//     ASSERT_EQ(D->painter()->id(), "p_0001");
//     ASSERT_EQ(D->painter()->name(), "Patrick");

//     ASSERT_TRUE(UnitOfWork::instance()->inClean("d_888"));
//     ASSERT_TRUE(UnitOfWork::instance()->inClean("p_0001"));

//     delete scanner;
//     delete builder;
//     delete parser;
//     delete painter;
//     delete Patrick;
//     delete drawing;
//     delete uow;
// }

// TEST_F(DBSuite, DELUOWNewPainter){
//     Painter *Luca = new Painter("p_666","Luca");
    
//     ASSERT_TRUE(UnitOfWork::instance()->inNew("p_666"));

//     UnitOfWork * uow = UnitOfWork::instance();

//     Luca->setName("");

//     ASSERT_FALSE(UnitOfWork::instance()->inNew(Luca->id()));
//     ASSERT_TRUE(UnitOfWork::instance()->inDeleted("p_666"));

//     delete Luca;
//     delete uow;
// }

// TEST_F(DBSuite, DELUOWCleanPainter){
//     Painter *P = pm->find("p_0002");

//     ASSERT_TRUE(UnitOfWork::instance()->inClean("p_0002"));

//     UnitOfWork * uow = UnitOfWork::instance();

//     P->setName("");

//     uow->commit();

//     ASSERT_TRUE(pm->find("p_0002") == nullptr);

//     delete P;
//     delete uow;
// }