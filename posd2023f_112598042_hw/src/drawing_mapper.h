#pragma once

#include <string>
#include <sqlite3.h>

#include "abstract_mapper.h"
#include "painter_mapper.h"

class DomainObject;
class Drawing;
class Scanner;
class Parser;
class Builder;

class DrawingMapper : public AbstractMapper
{
public:
    ~DrawingMapper(){
        // if(_instance){
        //     delete _instance;
        // }
    }
    Drawing *find(std::string id);

    // add
    void add(DomainObject *Drawing);

    // update
    void update(std::string id);

    // delete
    void del(std::string id);

    std::string updateStmt(DomainObject *domainObject) const;

    std::string findByIdStmt(std::string id) const;

    std::string addStmt(DomainObject *domainObject) const;

    std::string deleteByIdStmt(std::string id) const;

    static DrawingMapper *instance();

protected:
    DrawingMapper();

    static int callback(void *notUsed, int argc, char **argv, char **colNames){
        PainterMapper * painterMapper = PainterMapper::instance();
        Painter * painter = painterMapper->find(argv[1]);
        
        Drawing * drawing = new Drawing(argv[0], painter,DrawingMapper::instance()->convertShapes(argc, argv));

        DrawingMapper::instance()->load(drawing);
        return 0;
    }

    std::list<Shape *> convertShapes(int argc, char **argv);

private:
    char *_errorMessage;
    static DrawingMapper *_instance;
    Builder *_builder;
    Scanner *_scanner;
    Parser *_parser;
};