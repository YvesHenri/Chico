#pragma once

#include "Geometry.hpp"

class Shape
{
public:
	explicit Shape(float x = 0.f, float y = 0.f);

	virtual bool overlaps(const Shape& shape) const;
	virtual bool overlaps(float x, float y, float radius) const = 0;
	virtual bool overlaps(float x, float y, float width, float height) const = 0;

	virtual bool intersects(const Shape& shape) const;
	virtual bool intersects(float x, float y, float radius) const = 0;
	virtual bool intersects(float x, float y, float width, float height) const = 0;

	float x;
	float y;
};

class Circle : public Shape
{
public:
	explicit Circle(float x = 0.f, float y = 0.f, float radius = 0.f);

	bool overlaps(float x, float y, float radius) const override;
	bool overlaps(float x, float y, float width, float height) const override;
	bool intersects(float x, float y, float radius) const override;
	bool intersects(float x, float y, float width, float height) const override;

	float radius;
};

class Rect : public Shape
{
public:
	explicit Rect(float x = 0.f, float y = 0.f, float width = 0.f, float height = 0.f);

	bool overlaps(float x, float y, float radius) const override;
	bool overlaps(float x, float y, float width, float height) const override;
	bool intersects(float x, float y, float radius) const override;
	bool intersects(float x, float y, float width, float height) const override;

	float width;
	float height;
};