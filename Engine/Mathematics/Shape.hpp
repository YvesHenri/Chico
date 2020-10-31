#pragma once

#include "Shape.h"

Shape::Shape(float x, float y) : x(x), y(y)
{}

Circle::Circle(float x, float y, float radius) : Shape(x, y), radius(radius)
{}

Rect::Rect(float x, float y, float width, float height) : Shape(x, y), width(width), height(height)
{}

bool Shape::overlaps(const Shape& shape) const {
	if (auto circle = dynamic_cast<const Circle*>(&shape)) {
		return overlaps(circle->x, circle->y, circle->radius);
	}
	if (auto rectangle = dynamic_cast<const Rect*>(&shape)) {
		return overlaps(rectangle->x, rectangle->y, rectangle->width, rectangle->height);
	}

	throw "Unknown shape";
}

bool Shape::intersects(const Shape& shape) const {
	if (auto circle = dynamic_cast<const Circle*>(&shape)) {
		return intersects(circle->x, circle->y, circle->radius);
	}
	if (auto rectangle = dynamic_cast<const Rect*>(&shape)) {
		return intersects(rectangle->x, rectangle->y, rectangle->width, rectangle->height);
	}

	throw "Unknown shape";
}

bool Circle::overlaps(float x, float y, float radius) const {
	return Geometry::overlaps(this->x, this->y, this->radius, x, y, radius);
}

bool Circle::overlaps(float x, float y, float width, float height) const {
	return Geometry::overlaps(this->x, this->y, this->radius, x, y, width, height);
}

bool Circle::intersects(float x, float y, float radius) const {
	return Geometry::intersects(this->x, this->y, this->radius, x, y, radius);
}

bool Circle::intersects(float x, float y, float width, float height) const {
	return Geometry::intersects(this->x, this->y, this->radius, x, y, width, height);
}

bool Rect::overlaps(float x, float y, float radius) const {
	return Geometry::overlaps(this->x, this->y, this->width, this->height, x, y, radius);
}

bool Rect::overlaps(float x, float y, float width, float height) const {
	return Geometry::overlaps(this->x, this->y, this->width, this->height, x, y, width, height);
}

bool Rect::intersects(float x, float y, float radius) const {
	return Geometry::intersects(x, y, radius, this->x, this->y, this->width, this->height);
}

bool Rect::intersects(float x, float y, float width, float height) const {
	return Geometry::intersects(this->x, this->y, this->width, this->height, x, y, width, height);
}