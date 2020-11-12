#ifndef DATA_QUADTREE_QUAD_HPP
#define DATA_QUADTREE_QUAD_HPP

#include <memory>
#include <vector>
#include <functional>

#include "Object.hpp"

namespace qdt
{
	class Quad final : public Rectangle
	{
	public:
		explicit Quad(float x, float y, float width, float height, float minWidth, float minHeight, unsigned capacity) : Rectangle(width, height), Shape(x, y)
		{
			this->minWidth = minWidth;
			this->minHeight = minHeight;
			this->capacity = capacity;
		}

		void clear() {
			objects.clear();
			northwest.reset();
			northeast.reset();
			southwest.reset();
			southeast.reset();
		}

		void traverse(const std::function<void(const Rectangle&)>& callback) {
			callback(*this);
			if (northwest) northwest->traverse(callback);
			if (northeast) northeast->traverse(callback);
			if (southwest) southwest->traverse(callback);
			if (southeast) southeast->traverse(callback);
		}

		void addCircle(unsigned id, float cx, float cy, float radius) {
			if (northwest && northwest->overlaps(cx, cy, radius)) {
				northwest->addCircle(id, cx, cy, radius);
				return;
			}
			if (northeast && northeast->overlaps(cx, cy, radius)) {
				northeast->addCircle(id, cx, cy, radius);
				return;
			}
			if (southwest && southwest->overlaps(cx, cy, radius)) {
				southwest->addCircle(id, cx, cy, radius);
				return;
			}
			if (southeast && southeast->overlaps(cx, cy, radius)) {
				southeast->addCircle(id, cx, cy, radius);
				return;
			}

			objects.emplace_back(new CircleObject(id, cx, cy, radius));
			split();
		}

		void addRectangle(unsigned id, float x, float y, float width, float height) {
			if (northwest && northwest->overlaps(x, y, width, height)) {
				northwest->addRectangle(id, x, y, width, height);
				return;
			}
			if (northeast && northeast->overlaps(x, y, width, height)) {
				northeast->addRectangle(id, x, y, width, height);
				return;
			}
			if (southwest && southwest->overlaps(x, y, width, height)) {
				southwest->addRectangle(id, x, y, width, height);
				return;
			}
			if (southeast && southeast->overlaps(x, y, width, height)) {
				southeast->addRectangle(id, x, y, width, height);
				return;
			}

			objects.emplace_back(new RectangleObject(id, x, y, width, height));
			split();
		}

		template <typename O, typename = typename std::enable_if<std::is_base_of<Object, O>::value>::type>
		void query(float cx, float cy, float radius, std::vector<std::shared_ptr<O>>& result) {
			// Skip quad if it does not intersect with the search boundary
			if (intersects(cx, cy, radius)) {
				for (auto& object : objects) {
					auto dynamicObject = std::dynamic_pointer_cast<O>(object);

					if (dynamicObject) {
						if (dynamicObject->intersects(cx, cy, radius)) {
							result.push_back(dynamicObject);
						}
					}
				}

				if (northwest) northwest->query(cx, cy, radius, result);
				if (northeast) northeast->query(cx, cy, radius, result);
				if (southwest) southwest->query(cx, cy, radius, result);
				if (southeast) southeast->query(cx, cy, radius, result);
			}
		}

		template <typename O, typename = typename std::enable_if<std::is_base_of<Object, O>::value>::type>
		void query(float x, float y, float width, float height, std::vector<std::shared_ptr<O>>& result) {
			// Skip quad if it does not intersect with the search boundary
			if (intersects(x, y, width, height)) {
				for (auto& object : objects) {
					auto dynamicObject = std::dynamic_pointer_cast<O>(object);

					if (dynamicObject) {
						if (dynamicObject->intersects(x, y, width, height)) {
							result.push_back(dynamicObject);
						}
					}
				}

				if (northwest) northwest->query(x, y, width, height, result);
				if (northeast) northeast->query(x, y, width, height, result);
				if (southwest) southwest->query(x, y, width, height, result);
				if (southeast) southeast->query(x, y, width, height, result);
			}
		}

	private:
		bool leaf() {
			return !northwest && !northeast && !southwest && !southeast;
		}

		void split() {
			auto subWidth = width / 2.f;
			auto subHeight = height / 2.f;

			// Check whether it's time to split (if not already split) - TODO add acceptable margin percentage for min width/height
			if (objects.size() > capacity && subWidth > minWidth && subHeight > minHeight && leaf()) {
				northwest = std::make_unique<Quad>(x, y, subWidth, subHeight, minWidth, minHeight, capacity);
				northeast = std::make_unique<Quad>(x + subWidth, y, subWidth, subHeight, minWidth, minHeight, capacity);
				southeast = std::make_unique<Quad>(x + subWidth, y + subHeight, subWidth, subHeight, minWidth, minHeight, capacity);
				southwest = std::make_unique<Quad>(x, y + subHeight, subWidth, subHeight, minWidth, minHeight, capacity);

				// Move objects to their corresponding new quads
				for (auto iterator = objects.begin(); iterator != objects.end(); /* iterator++ */) {
					auto& object = *iterator;

					if (object->fits(northwest->x, northwest->y, northwest->width, northwest->height)) {
						northwest->objects.push_back(std::move(object));
						iterator = objects.erase(iterator);
						continue;
					}
					if (object->fits(northeast->x, northeast->y, northeast->width, northeast->height)) {
						northeast->objects.push_back(std::move(object));
						iterator = objects.erase(iterator);
						continue;
					}
					if (object->fits(southwest->x, southwest->y, southwest->width, southwest->height)) {
						southwest->objects.push_back(std::move(object));
						iterator = objects.erase(iterator);
						continue;
					}
					if (object->fits(southeast->x, southeast->y, southeast->width, southeast->height)) {
						southeast->objects.push_back(std::move(object));
						iterator = objects.erase(iterator);
						continue;
					}

					++iterator; // Does not fit. Move on.
				}
			}
		}

		float minWidth;
		float minHeight;
		unsigned capacity;

		std::unique_ptr<Quad> northwest;
		std::unique_ptr<Quad> northeast;
		std::unique_ptr<Quad> southwest;
		std::unique_ptr<Quad> southeast;
		std::vector<std::shared_ptr<Object>> objects;
	};
}

#endif