#pragma once

#include "QuadShape.hpp"

namespace qdt
{
    /*
     * Representation of anything to be within a quad.
     * Superclasses must explicitly call the Shape constructor, due to this virtual inheritance.
     */
    struct Node : public virtual Shape
    {
        explicit Node(unsigned id)
        {
            this->id = id;
        }

        unsigned id;
    };

    /*
     * Circle representation for circle-based collisions.
     */
    struct CircleShapedNode final : public Circle, public Node
    {
        explicit CircleShapedNode(unsigned id, float cx, float cy, float radius) : Circle(radius), Shape(cx, cy), Node(id) {}
    };

    /*
     * Rectangle representation for rectangle-based collisions.
     */
    struct RectangleShapedNode final : public Rectangle, public Node
    {
        explicit RectangleShapedNode(unsigned id, float x, float y, float width, float height) : Rectangle(width, height), Shape(x, y), Node(id) {}
    };
}