#pragma once

#include <memory>
#include <vector>

#include "QuadNode.hpp"

namespace qdt
{
	class Quad final : public Rectangle
	{
	public:
		explicit Quad(float x, float y, float width, float height, float minWidth, float minHeight, unsigned capacity, unsigned level, unsigned quadrant) : Rectangle(width, height), Shape(x, y)
		{
			this->minWidth = minWidth;
			this->minHeight = minHeight;
			this->capacity = capacity;
			this->level = level;
			this->quadrant = quadrant;
		}

		void clear() {
			items.clear();
			northwest.reset();
			northeast.reset();
			southwest.reset();
			southeast.reset();
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

			items.emplace_back(new CircleShapedNode(id, cx, cy, radius));
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

			items.emplace_back(new RectangleShapedNode(id, x, y, width, height));
			split();
		}

		void query(float cx, float cy, float radius, std::vector<std::shared_ptr<Node>>& result) {
			// Skip quad if it does not intersect with the search boundary
			if (intersects(cx, cy, radius)) {
				for (auto& item : items) {
					// Select only the items that does really intersect the search boundary
					if (item->intersects(cx, cy, radius)) {
						result.push_back(item);
					}
				}

				if (northwest) northwest->query(cx, cy, radius, result);
				if (northeast) northeast->query(cx, cy, radius, result);
				if (southwest) southwest->query(cx, cy, radius, result);
				if (southeast) southeast->query(cx, cy, radius, result);
			}
		}

		void query(float x, float y, float width, float height, std::vector<std::shared_ptr<Node>>& result) {
			// Skip quad if it does not intersect with the search boundary
			if (intersects(x, y, width, height)) {
				for (auto& item : items) {
					//printf("Testing item %d from quadrant %d at level %d\n", item->id, quadrant, level);

					// Select only the items that does really intersect the search boundary
					if (item->intersects(x, y, width, height)) {
						result.push_back(item);
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
			if (items.size() > capacity && subWidth > minWidth && subHeight > minHeight && leaf()) {
				northwest = std::make_shared<Quad>(x, y, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 1U);
				northeast = std::make_shared<Quad>(x + subWidth, y, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 2U);
				southeast = std::make_shared<Quad>(x + subWidth, y + subHeight, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 3U);
				southwest = std::make_shared<Quad>(x, y + subHeight, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 4U);
				quadrant = 0U;

				// Move items to their corresponding new quads
				for (auto iterator = items.begin(); iterator != items.end(); /* iterator++ */) {
					auto& item = *iterator;

					if (item->fits(northwest->x, northwest->y, northwest->width, northwest->height)) {
						northwest->items.push_back(std::move(item));
						iterator = items.erase(iterator);
						continue;
					}
					if (item->fits(northeast->x, northeast->y, northeast->width, northeast->height)) {
						northeast->items.push_back(std::move(item));
						iterator = items.erase(iterator);
						continue;
					}
					if (item->fits(southwest->x, southwest->y, southwest->width, southwest->height)) {
						southwest->items.push_back(std::move(item));
						iterator = items.erase(iterator);
						continue;
					}
					if (item->fits(southeast->x, southeast->y, southeast->width, southeast->height)) {
						southeast->items.push_back(std::move(item));
						iterator = items.erase(iterator);
						continue;
					}

					++iterator; // Does not fit. Move on.
				}
			}
		}

		float minWidth;
		float minHeight;

		unsigned level;
		unsigned capacity;
		unsigned quadrant;

		std::shared_ptr<Quad> northwest;
		std::shared_ptr<Quad> northeast;
		std::shared_ptr<Quad> southwest;
		std::shared_ptr<Quad> southeast;
		std::vector<std::shared_ptr<Node>> items;
	};
}