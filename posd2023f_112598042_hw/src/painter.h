#pragma once

#include "domain_object.h"
#include "unit_of_work.h"

// #include "painter_mapper.h"
class PainterMapper;

class Painter : public DomainObject {
public:
    Painter(std::string id, std::string name) : DomainObject(id), _name(name) {
        UnitOfWork::instance()->registerNew(this);
    }

    ~Painter(){
        UnitOfWork::instance()->eraseAll(this->id());
        UnitOfWork::instance()->registerDeleted(this);
    }

    std::string name() const {
        return _name;
    }

    void setName(std::string name) {
        if(name==""){
            UnitOfWork::instance()->registerDeleted(this);       
        }else{
            _name = name;
            UnitOfWork::instance()->registerDirty(this);
        }
    }

private:
    std::string _name;
};