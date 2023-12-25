#pragma once

#include <string>
#include <sqlite3.h>

#include "abstract_mapper.h"

#include "painter.h"
class Painter;

class PainterMapper: public AbstractMapper {
public:
    ~PainterMapper(){
        // if(_instance){
        //     delete _instance;
        // }
    }

    Painter* find(std::string id){
        return static_cast<Painter *>(abstractFind(id, PainterMapper::callback));
    }
    
    void add(DomainObject * Painter){
        abstractAdd(Painter);    
    }

    void update(std::string id){
        abstractUpdate(find(id));
    }

    void del(std::string id){
        abstractDelete(id);
    }

    std::string updateStmt(DomainObject * domainObject) const{
        Painter * obj = static_cast<Painter *>(domainObject);        
        std::string stmt = "UPDATE painter SET name = '"+ obj->name() +"' WHERE ID='" + domainObject->id() + "'";
        return stmt;
    }

    std::string findByIdStmt(std::string id) const{
        std::string stmt = "SELECT * FROM painter WHERE ID='" + id + "'";
        return stmt;
    }

    std::string addStmt(DomainObject * domainObject) const{
        Painter * obj = static_cast<Painter *>(domainObject);
        std::string stmt = "INSERT INTO painter(ID, name) values ('" + domainObject->id() + "','" + obj->name() + "')";
        return stmt;
    }

    std::string deleteByIdStmt(std::string id) const{
        std::string stmt = "DELETE FROM painter WHERE ID='" + id + "'"; 
        return stmt;
    }

    static PainterMapper* instance(){
        if (_instance == nullptr)
        {
            _instance = new PainterMapper();
        }
        return _instance;
    }

protected:
    PainterMapper();

    static int callback(void* notUsed, int argc, char** argv, char** colNames){
        Painter * painter = new Painter(argv[0], argv[1]);  
        PainterMapper::instance()->load(painter);
        return 0;   
    }
    
private:
    char* _errorMessage;
    static PainterMapper* _instance;
};