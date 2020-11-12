#ifndef DATA_QUADTREE_OBJECT_HPP
#define DATA_QUADTREE_OBJECT_HPP

#include "Shape.hpp"

namespace qdt
{
    /*
     * Representation of anything to be within a quad.
     * Superclasses must explicitly call the Shape constructor, due to this virtual inheritance.
     */
    struct Object : public virtual Shape
    {
        explicit Object(unsigned id)
        {
            this->id = id;
        }

        unsigned id;
    };

    /*
     * Circle representation for circle-based collisions.
     */
    struct CircleObject final : public Circle, public Object
    {
        explicit CircleObject(unsigned id, float cx, float cy, float radius) : Circle(radius), Shape(cx, cy), Object(id) {}
    };

    /*
     * Rectangle representation for rectangle-based collisions.
     */
    struct RectangleObject final : public Rectangle, public Object
    {
        explicit RectangleObject(unsigned id, float x, float y, float width, float height) : Rectangle(width, height), Shape(x, y), Object(id) {}
    };
}

#endif