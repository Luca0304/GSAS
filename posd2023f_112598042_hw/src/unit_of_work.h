#pragma once

#include <unordered_map>
#include <set>
#include <string>

#include "domain_object.h"
#include <iostream>

class DomainObject;

class UnitOfWork {
public:
    static UnitOfWork * instance(){
        if (_instance == nullptr)
        {
            _instance = new UnitOfWork();
        }
        return _instance;
    }
    ~UnitOfWork(){
        // if(_instance){
        //     delete _instance;
        // }
    }
    void registerNew(DomainObject * domainObject){
        std::string id = domainObject->id();
        _new[id] = domainObject;
    }

    void registerClean(DomainObject * domainObject){
        std::string id = domainObject->id();
        _clean[id] = domainObject;
    }

    void registerDirty(DomainObject * domainObject){
        std::string id = domainObject->id();
        _dirty[id] = domainObject;
        _clean.erase(domainObject->id());
    }

    void registerDeleted(DomainObject * domainObject){
        std::string id = domainObject->id();
        _deleted[id] = nullptr;       
        _new.erase(id);
        _clean.erase(id);
        _dirty.erase(id);
    }

    void eraseAll(std::string id){
        _new.erase(id);
        _clean.erase(id);
        _dirty.erase(id);
    }

    bool inNew(std::string id) const{
        return _new.count(id);
    }

    bool inClean(std::string id) const{
        return _clean.count(id);
    }

    bool inDirty(std::string id) const{
        return _dirty.count(id);
    }

    bool inDeleted(std::string id) const{
        return _deleted.count(id);
    }

    void commit();

protected:
    UnitOfWork();
private:
    std::unordered_map<std::string, DomainObject *> _new;
    std::unordered_map<std::string, DomainObject *> _dirty;
    std::unordered_map<std::string, DomainObject *> _clean;
    std::unordered_map<std::string, DomainObject *> _deleted;
    static UnitOfWork * _instance;
};