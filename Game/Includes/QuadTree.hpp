#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "QuadNode.hpp"

class QuadTree
{
public:
	explicit QuadTree(float x, float y, float width, float height)
		: QuadTree(x, y, width, height, 64.f, 64.f, 4U, 0U, 0U) {}

	explicit QuadTree(float x, float y, float width, float height, float minWidth, float minHeight, unsigned capacity, unsigned level, unsigned quadrant)
		: x(x), y(y), width(width), height(height), minWidth(minWidth), minHeight(minHeight), capacity(capacity), level(level), quadrant(quadrant) {
	
		grid.setOutlineThickness(1.f);
		grid.setOutlineColor(sf::Color::Black);
		grid.setFillColor(sf::Color::Transparent);
		grid.setSize(sf::Vector2f(width, height));
		grid.setPosition(x, y);
	}

	void draw(sf::RenderTarget& target) {
		if (!nodes.empty())
			target.draw(grid);
		if (northwest) northwest->draw(target);
		if (northeast) northeast->draw(target);
		if (southwest) southwest->draw(target);
		if (southeast) southeast->draw(target);
	}

	void clear() {
		nodes.clear();
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

		nodes.emplace_back(new CircleNode(id, cx, cy, radius));
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

		nodes.emplace_back(new RectangleNode(id, x, y, width, height));
		split();
	}

	void query(float x, float y, float radius, std::vector<std::shared_ptr<QuadNode>>& result) {
		// Skip quad if it does not intersect with the search boundary
		if (Geometry::rectangleIntersectsCircle(this->x, this->y, this->width, this->height, x, y, radius)) {
			for (auto& node : nodes) {
				if (node->intersects(x, y, radius)) {
					result.push_back(node);
				}
			}

			if (northwest) northwest->query(x, y, radius, result);
			if (northeast) northeast->query(x, y, radius, result);
			if (southwest) southwest->query(x, y, radius, result);
			if (southeast) southeast->query(x, y, radius, result);
		}
	}

	void query(float x, float y, float width, float height, std::vector<std::shared_ptr<QuadNode>>& result) {
		// Skip quad if it does not intersect with the search boundary
		if (Geometry::rectangleIntersectsRectangle(this->x, this->y, this->width, this->height, x, y, width, height)) {
			for (auto& node : nodes) {
				if (node->intersects(x, y, width, height)) {
					result.push_back(node);
				}
			}

			if (northwest) northwest->query(x, y, width, height, result);
			if (northeast) northeast->query(x, y, width, height, result);
			if (southwest) southwest->query(x, y, width, height, result);
			if (southeast) southeast->query(x, y, width, height, result);
		}
	}

private:
	bool overlaps(const std::shared_ptr<QuadNode>& node) {
		return node->fits(x, y, width, height);
	}

	bool overlaps(float x, float y, float radius) {
		return Geometry::rectangleOverlapsCircle(this->x, this->y, this->width, this->height, x, y, radius);
	}

	bool overlaps(float x, float y, float width, float height) {
		return Geometry::rectangleOverlapsRectangle(this->x, this->y, this->width, this->height, x, y, width, height);
	}	

	void split() {
		auto subWidth = width / 2.f;
		auto subHeight = height / 2.f;
		auto unsplit = !northwest && !northeast && !southwest && !southeast;

		// Check whether it's time to split (if not already split) - TODO add acceptable margin percentage for min width/height
		if (nodes.size() > capacity && subWidth > minWidth && subHeight > minHeight && unsplit) {
			northwest = std::make_shared<QuadTree>(x, y, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 1U);
			northeast = std::make_shared<QuadTree>(x + subWidth, y, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 2U);
			southwest = std::make_shared<QuadTree>(x, y + subHeight, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 4U);
			southeast = std::make_shared<QuadTree>(x + subWidth, y + subHeight, subWidth, subHeight, minWidth, minHeight, capacity, level + 1U, 3U);

			// Move nodes to their corresponding new quads
			for (auto iterator = nodes.begin(); iterator != nodes.end(); /* iterator++ */) {
				auto& node = *iterator;

				if (northwest->overlaps(node)) {
					northwest->nodes.push_back(std::move(node));
					iterator = nodes.erase(iterator);
					continue;
				}
				if (northeast->overlaps(node)) {
					northeast->nodes.push_back(std::move(node));
					iterator = nodes.erase(iterator);
					continue;
				}
				if (southwest->overlaps(node)) {
					southwest->nodes.push_back(std::move(node));
					iterator = nodes.erase(iterator);
					continue;
				}
				if (southeast->overlaps(node)) {
					southeast->nodes.push_back(std::move(node));
					iterator = nodes.erase(iterator);
					continue;
				}

				++iterator; // Does not fit. Move on.
			}
		}
	}

	float x;
	float y;
	float width;
	float height;
	float minWidth;
	float minHeight;
	unsigned level;
	unsigned capacity;
	unsigned quadrant;

	sf::RectangleShape grid;
	std::shared_ptr<QuadTree> northwest;
	std::shared_ptr<QuadTree> northeast;
	std::shared_ptr<QuadTree> southwest;
	std::shared_ptr<QuadTree> southeast;
	std::vector<std::shared_ptr<QuadNode>> nodes;
};
