#ifndef DATA_QUADTREE_SHAPE_HPP
#define DATA_QUADTREE_SHAPE_HPP

#include "../../Geometry.hpp"

namespace qdt
{
    /*
    * Representation of a shape. Things like a Quad and Node have shapes.
    */
    struct Shape
    {
        explicit Shape(float x = 0.f, float y = 0.f)
        {
            this->x = x;
            this->y = y;
        }

        virtual bool fits(const Shape& shape) const;
        virtual bool fits(float x, float y, float radius) const = 0;
        virtual bool fits(float x, float y, float width, float height) const = 0;
        
        virtual bool overlaps(const Shape& shape) const;
        virtual bool overlaps(float x, float y, float radius) const = 0;
        virtual bool overlaps(float x, float y, float width, float height) const = 0;
        
        virtual bool intersects(const Shape& shape) const;
        virtual bool intersects(float x, float y, float radius) const = 0;
        virtual bool intersects(float x, float y, float width, float height) const = 0;        

        float x;
        float y;
    };

    /*
    * Representation of a circle shape.
    * Superclasses must explicitly call the Shape constructor, due to this virtual inheritance.
    */
    struct Circle : public virtual Shape
    {
        explicit Circle(float radius)
        {
            this->radius = radius;
        }

        bool fits(float x, float y, float radius) const override {
            return Geometry::circleFitsCircle(this->x, this->y, this->radius, x, y, radius);
        }

        bool fits(float x, float y, float width, float height) const override {
            return Geometry::circleFitsRectangle(this->x, this->y, this->radius, x, y, width, height);
        }

        bool overlaps(float x, float y, float radius) const override {
            return Geometry::circleOverlapsCircle(this->x, this->y, this->radius, x, y, radius);
        }

        bool overlaps(float x, float y, float width, float height) const override {
            return Geometry::circleOverlapsRectangle(this->x, this->y, this->radius, x, y, width, height);
        }

        bool intersects(float x, float y, float radius) const override {
            return Geometry::circleIntersectsCircle(this->x, this->y, this->radius, x, y, radius);
        }

        bool intersects(float x, float y, float width, float height) const override {
            return Geometry::circleIntersectsRectangle(this->x, this->y, this->radius, x, y, width, height);
        }

        float radius;
    };

    /*
    * Representation of a rectangle shape. 
    * Superclasses must explicitly call the Shape constructor, due to this virtual inheritance.
    */
    struct Rectangle : public virtual Shape
    {
        explicit Rectangle(float width, float height)
        {
            this->width = width;
            this->height = height;
        }

        bool fits(float x, float y, float radius) const override {
            return Geometry::rectangleFitsCircle(this->x, this->y, this->width, this->height, x, y, radius);
        }

        bool fits(float x, float y, float width, float height) const override {
            return Geometry::rectangleFitsRectangle(this->x, this->y, this->width, this->height, x, y, width, height);
        }

        bool overlaps(float x, float y, float radius) const override {
            return Geometry::rectangleOverlapsCircle(this->x, this->y, this->width, this->height, x, y, radius);
        }

        bool overlaps(float x, float y, float width, float height) const override {
            return Geometry::rectangleOverlapsRectangle(this->x, this->y, this->width, this->height, x, y, width, height);
        }

        bool intersects(float x, float y, float radius) const override {
            return Geometry::rectangleIntersectsCircle(this->x, this->y, this->width, this->height, x, y, radius);
        }

        bool intersects(float x, float y, float width, float height) const override {
            return Geometry::rectangleIntersectsRectangle(this->x, this->y, this->width, this->height, x, y, width, height);
        }

        float width;
        float height;
    };

    bool Shape::fits(const Shape& shape) const {
        // Circle
        if (auto circle = dynamic_cast<const Circle*>(&shape)) {
            return fits(circle->x, circle->y, circle->radius);
        }

        // Rectangle
        if (auto rectangle = dynamic_cast<const Rectangle*>(&shape)) {
            return fits(rectangle->x, rectangle->y, rectangle->width, rectangle->height);
        }

        throw "Unknown shape";
    }

    bool Shape::overlaps(const Shape& shape) const {
        // Circle
        if (auto circle = dynamic_cast<const Circle*>(&shape)) {
            return overlaps(circle->x, circle->y, circle->radius);
        }

        // Rectangle
        if (auto rectangle = dynamic_cast<const Rectangle*>(&shape)) {
            return overlaps(rectangle->x, rectangle->y, rectangle->width, rectangle->height);
        }

        throw "Unknown shape";
    }

    bool Shape::intersects(const Shape& shape) const {
        // Circle
        if (auto circle = dynamic_cast<const Circle*>(&shape)) {
            return intersects(circle->x, circle->y, circle->radius);
        }

        // Rectangle
        if (auto rectangle = dynamic_cast<const Rectangle*>(&shape)) {
            return intersects(rectangle->x, rectangle->y, rectangle->width, rectangle->height);
        }

        throw "Unknown shape";
    }
}

#endif