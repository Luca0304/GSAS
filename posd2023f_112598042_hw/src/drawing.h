#pragma once

#include <string>
#include <list>
#include "domain_object.h"
#include "shape.h"
#include "painter.h"

// #include "drawing_mapper.h"
// class DrawingMapper;

class Drawing : public DomainObject
{
public:
  Drawing(std::string id, Painter *painter) : DomainObject(id), _painter(painter)
  {
    UnitOfWork::instance()->registerNew(this);
  }

  Drawing(std::string id, Painter *painter, std::list<Shape *> shapes) : DomainObject(id), _painter(painter), _shapes(shapes)
  {
    UnitOfWork::instance()->registerNew(this);
  }

  ~Drawing(){
    UnitOfWork::instance()->eraseAll(this->id());
    UnitOfWork::instance()->registerDeleted(this);
  }

  Painter *painter() const
  {
    return _painter;
  }

  void setPainter(Painter *painter)
  {
    _painter = painter;
    UnitOfWork::instance()->registerDirty(this);
  }

  Shape *getShape(unsigned int i) const
  {
    if (i < 0 || i > _shapes.size() - 1)
      return nullptr;

    auto it = _shapes.begin();
    for (unsigned int j = 0; j < i; j++)
      it++;

    return *it;
  }

  std::string getShapesAsString() const
  {
    std::string result = "";
    for (auto shape : _shapes)
    {
      result += shape->toString() + "\n";
    }
    return result;
  }

private:
  Painter *_painter;
  std::list<Shape *> _shapes;
};