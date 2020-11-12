#ifndef DATA_QUADTREE_QUADTREE_HPP
#define DATA_QUADTREE_QUADTREE_HPP

#include "Quad.hpp"

namespace qdt
{
	class QuadTree
	{
	public:
		explicit QuadTree(float x, float y, float right, float bottom) : QuadTree(x, y, right, bottom, 64.f, 64.f) {}
		explicit QuadTree(float x, float y, float right, float bottom, float minQuadWidth, float minQuadHeight) : root(x, y, right, bottom, minQuadWidth, minQuadHeight, 4U) {}

		void clear() {
			root.clear();
		}

		void addCircle(unsigned id, float cx, float cy, float radius) {
			root.addCircle(id, cx, cy, radius);
		}

		void addRectangle(unsigned id, float x, float y, float right, float bottom) {
			root.addRectangle(id, x, y, right, bottom);
		}

		void query(const Rectangle& bounds, std::vector<std::shared_ptr<Object>>& result) {
			query(bounds.x, bounds.y, bounds.width, bounds.height, result);
		}

		void query(float x, float y, float right, float bottom, std::vector<std::shared_ptr<Object>>& result) {
			root.query(x, y, right, bottom, result);
		}

		void query(const Circle& bounds, std::vector<std::shared_ptr<Object>>& result) {
			query(bounds.x, bounds.y, bounds.radius, result);
		}

		void query(float cx, float cy, float radius, std::vector<std::shared_ptr<Object>>& result) {
			root.query(cx, cy, radius, result);
		}

		void queryCircles(const Rectangle& bounds, std::vector<std::shared_ptr<CircleObject>>& result) {
			queryCircles(bounds.x, bounds.y, bounds.width, bounds.height, result);
		}

		void queryCircles(float x, float y, float right, float bottom, std::vector<std::shared_ptr<CircleObject>>& result) {
			root.query(x, y, right, bottom, result);
		}

		void queryCircles(const Circle& bounds, std::vector<std::shared_ptr<CircleObject>>& result) {
			queryCircles(bounds.x, bounds.y, bounds.radius, result);
		}

		void queryCircles(float x, float y, float radius, std::vector<std::shared_ptr<CircleObject>>& result) {
			root.query(x, y, radius, result);
		}

		void queryRectangles(const Rectangle& bounds, std::vector<std::shared_ptr<RectangleObject>>& result) {
			queryRectangles(bounds.x, bounds.y, bounds.width, bounds.height, result);
		}

		void queryRectangles(float x, float y, float right, float bottom, std::vector<std::shared_ptr<RectangleObject>>& result) {
			root.query(x, y, right, bottom, result);
		}

		void queryRectangles(const Circle& bounds, std::vector<std::shared_ptr<RectangleObject>>& result) {
			queryRectangles(bounds.x, bounds.y, bounds.radius, result);
		}

		void queryRectangles(float x, float y, float radius, std::vector<std::shared_ptr<RectangleObject>>& result) {
			root.query(x, y, radius, result);
		}

	protected:
		void traverse(const std::function<void(const Rectangle&)>& callback) {
			root.traverse(callback);
		}

		Quad root;
	};
}

#endif