#pragma once

#include <Engine/Mathematics/Geometry.hpp>

/*
 * Representation of anything to be within a quad.
 */
class QuadNode
{
public:
	explicit QuadNode(unsigned id, float x, float y) : id(id), x(x), y(y) {}

    virtual bool fits(float x, float y, float width, float height) const = 0;
    virtual bool intersects(float x, float y, float radius) const = 0;
    virtual bool intersects(float x, float y, float width, float height) const = 0;

    float x;
    float y;
	unsigned id;
};

/*
 * Circle representation for circle-based collisions.
 */
class CircleNode : public QuadNode
{
public:
	explicit CircleNode(unsigned id, float cx, float cy, float radius) : QuadNode(id, cx, cy), radius(radius) {}

    bool fits(float x, float y, float width, float height) const override {
        return Geometry::overlaps(x, y, width, height, this->x, this->y, this->radius);
    }

    bool intersects(float x, float y, float radius) const override {
        return Geometry::intersects(this->x, this->y, this->radius, x, y, radius);
    }

    bool intersects(float x, float y, float width, float height) const override {
        return Geometry::intersects(this->x, this->y, this->radius, x, y, width, height);
    }

    float radius;
};

/*
 * Rectangle representation for rectangle-based collisions.
 */
class RectangleNode : public QuadNode
{
public:
	explicit RectangleNode(unsigned id, float x, float y, float width, float height) : QuadNode(id, x, y), width(width), height(height) {}

    bool fits(float x, float y, float width, float height) const override {
        return Geometry::overlaps(x, y, width, height, this->x, this->y, this->width, this->height);
    }

    bool intersects(float x, float y, float radius) const override {
        return Geometry::intersects(x, y, radius, this->x, this->y, this->width, this->height);
    }

    bool intersects(float x, float y, float width, float height) const override {
        return Geometry::intersects(x, y, width, height, this->x, this->y, this->width, this->height);
    }

    float width;
    float height;
};