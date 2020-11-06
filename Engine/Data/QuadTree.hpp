#pragma once

#include "Quad.hpp"

class QuadTree final
{
public:
	explicit QuadTree(float x, float y, float width, float height) : root(x, y, width, height, 64.f, 64.f, 4U, 0U, 0U) {}
	explicit QuadTree(float x, float y, float width, float height, float minQuadWidth, float minQuadHeight) : root(x, y, width, height, minQuadWidth, minQuadHeight, 4U, 0U, 0U) {}

	void clear() {
		root.clear();
	}

	void addCircle(unsigned id, float cx, float cy, float radius) {
		root.addCircle(id, cx, cy, radius);
	}

	void addRectangle(unsigned id, float x, float y, float width, float height) {
		root.addRectangle(id, x, y, width, height);
	}

	void query(float cx, float cy, float radius, std::vector<std::shared_ptr<qdt::Node>>& result) {
		root.query(cx, cy, radius, result);
	}

	void query(float x, float y, float width, float height, std::vector<std::shared_ptr<qdt::Node>>& result) {
		root.query(x, y, width, height, result);
	}

private:
	qdt::Quad root;
};