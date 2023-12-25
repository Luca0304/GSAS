#include "unit_of_work.h"
#include "drawing_mapper.h"
#include "painter_mapper.h"
#include "domain_object.h"
#include <list>
#include <iostream>



UnitOfWork * UnitOfWork::_instance = nullptr;

UnitOfWork::UnitOfWork() {
    
}


void UnitOfWork::commit(){
    for (const auto& n : this->_clean) {
        if(dynamic_cast<Drawing *>(n.second)){
        }
        else{
        }
    }
        this->_clean.clear();

    for (const auto& n : this->_dirty) {
        if(dynamic_cast<Drawing *>(n.second)){
            DrawingMapper::instance()->update(n.second->id());
            registerClean(n.second);
        }
        else{
            PainterMapper::instance()->update(n.second->id());
            registerClean(n.second);
        }
    }
        this->_dirty.clear();   
    for (const auto& n : this->_new) {
        if(dynamic_cast<Drawing *>(n.second)){
            DrawingMapper::instance()->add(n.second);
            registerClean(n.second);
        }
        else{
            PainterMapper::instance()->add(n.second);
            registerClean(n.second);
        }
    }
        this->_new.clear(); 
    for (const auto& n : this->_deleted) {
        if(dynamic_cast<Drawing *>(n.second)){
            DrawingMapper::instance()->del(n.first);
        }
        else{
            PainterMapper::instance()->del(n.first);
        }
    }
        this->_deleted.clear();

}