#pragma once

#include <cmath>

class Geometry
{
public:
	/*
	* Circle vs Circle intersection.
	* @param x1 Circle 1's center X
	* @param y1 Circle 1's center Y
	* @param r1 Circle 1's radius
	* @param x2 Circle 2's center X
	* @param y2 Circle 2's center Y
	* @param r2 Circle 2's radius
	*/
	static bool circleIntersectsCircle(float x1, float y1, float r1, float x2, float y2, float r2) {
		return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= (r1 + r2) * (r1 + r2);
	}

	/*
	* Circle vs Rectangle intersection.
	* @param cx Circle's center X
	* @param cy Circle's center Y
	* @param cr Circle's radius
	* @param rx Rectangle's top X
	* @param ry Rectangle's top Y
	* @param rw Rectangle's width
	* @param rh Rectangle's height
	*/
	static bool circleIntersectsRectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh) {
		auto dx = cx - std::fmaxf(rx, std::fminf(cx, rx + rw));
		auto dy = cy - std::fmaxf(ry, std::fminf(cy, ry + rh));
		return dx * dx + dy * dy < cr * cr;
	}

	/*
	* Rectangle vs Circle intersection.
	* @param cx Circle's center X
	* @param cy Circle's center Y
	* @param cr Circle's radius
	* @param rx Rectangle's top X
	* @param ry Rectangle's top Y
	* @param rw Rectangle's width
	* @param rh Rectangle's height
	*/
	static bool rectangleIntersectsCircle(float rx, float ry, float rw, float rh, float cx, float cy, float cr) {
		return circleIntersectsRectangle(cx, cy, cr, rx, ry, rw, rh);
	}

	/*
	* Rectangle vs Rectangle intersection.
	* @param x1 Rectangle 1's top X
	* @param y1 Rectangle 1's top Y
	* @param w1 Rectangle 1's width
	* @param h1 Rectangle 1's height
	* @param x2 Rectangle 2's top X
	* @param y2 Rectangle 2's top Y
	* @param w2 Rectangle 2's width
	* @param h2 Rectangle 2's height
	*/
	static bool rectangleIntersectsRectangle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
		auto lx = std::fmaxf(x1, x2);
		auto rx = std::fminf(x1 + w1, x2 + w2);
		auto ty = std::fmaxf(y1, y2);
		auto by = std::fminf(y1 + h1, y2 + h2);
		return lx <= rx && ty <= by;
	}

	/*
	* Checks whether Circle 1 overlaps Circle 2.
	* @param x1 Circle 1's center X
	* @param y1 Circle 1's center Y
	* @param r1 Circle 1's radius
	* @param x2 Circle 2's center X
	* @param y2 Circle 2's center Y
	* @param r2 Circle 2's radius
	*/
	static bool circleOverlapsCircle(float x1, float y1, float r1, float x2, float y2, float r2) {
		return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= (r1 - r2) * (r1 - r2) && r1 >= r2;
	}

	/*
	* Checks whether Circle overlaps Rectangle.
	* @param cx Circle's center X
	* @param cy Circle's center Y
	* @param cr Circle's radius
	* @param rx Rectangle's top X
	* @param ry Rectangle's top Y
	* @param rw Rectangle's width
	* @param rh Rectangle's height
	*/
	static bool circleOverlapsRectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh) {
		auto dx = std::fmaxf(std::fabsf(cx - rx), std::fabsf(rx + rw - cx));
		auto dy = std::fmaxf(std::fabsf(cy - ry), std::fabsf(ry + rh - cy));
		return cr * cr >= (dx * dx) + (dy * dy);
	}

	/*
	* Checks whether Rectangle overlaps Circle.
	* @param cx Circle's center X
	* @param cy Circle's center Y
	* @param cr Circle's radius
	* @param rx Rectangle's top X
	* @param ry Rectangle's top Y
	* @param rw Rectangle's width
	* @param rh Rectangle's height
	*/
	static bool rectangleOverlapsCircle(float rx, float ry, float rw, float rh, float cx, float cy, float cr) {
		return cx - cr >= rx && cx + cr <= rx + rw && cy - cr >= ry && cy + cr <= ry + rh;
	}

	/*
	* Checks whether Rectangle 1 overlaps Rectangle 2.
	* @param x1 Rectangle 1's top X
	* @param y1 Rectangle 1's top Y
	* @param w1 Rectangle 1's width
	* @param h1 Rectangle 1's height
	* @param x2 Rectangle 2's top X
	* @param y2 Rectangle 2's top Y
	* @param w2 Rectangle 2's width
	* @param h2 Rectangle 2's height
	*/
	static bool rectangleOverlapsRectangle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
		return x2 >= x1 && x2 + w2 <= x1 + w1 && y2 >= y1 && y2 + h2 <= y1 + h1;
	}

	/*
	* Checks whether Circle 1 fits in Circle 2.
	* @param x1 Circle 1's center X
	* @param y1 Circle 1's center Y
	* @param r1 Circle 1's radius
	* @param x2 Circle 2's center X
	* @param y2 Circle 2's center Y
	* @param r2 Circle 2's radius
	*/
	static bool circleFitsCircle(float x1, float y1, float r1, float x2, float y2, float r2) {
		return circleOverlapsCircle(x2, y2, r2, x1, y1, r1);
	}

	/*
	* Checks whether Circle fits in Rectangle.
	* @param cx Circle 1's center X
	* @param cy Circle 1's center Y
	* @param cr Circle 1's radius
	* @param rx Rectangle's top X
	* @param ry Rectangle's top Y
	* @param rw Rectangle's width
	* @param rh Rectangle's height
	*/
	static bool circleFitsRectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh) {
		return rectangleOverlapsCircle(rx, ry, rw, rh, cx, cy, cr);
	}

	/*
	* Checks whether Rectangle fits in Circle.
	* @param cx Circle's center X
	* @param cy Circle's center Y
	* @param cr Circle's radius
	* @param rx Rectangle's top X
	* @param ry Rectangle's top Y
	* @param rw Rectangle's width
	* @param rh Rectangle's height
	*/
	static bool rectangleFitsCircle(float rx, float ry, float rw, float rh, float cx, float cy, float cr) {
		throw circleOverlapsRectangle(cx, cy, cr, rx, ry, rw, rh);
	}

	/*
	* Checks whether Rectangle 1 fits in Rectangle 2.
	* @param x1 Rectangle 1's top X
	* @param y1 Rectangle 1's top Y
	* @param w1 Rectangle 1's width
	* @param h1 Rectangle 1's height
	* @param x2 Rectangle 2's top X
	* @param y2 Rectangle 2's top Y
	* @param w2 Rectangle 2's width
	* @param h2 Rectangle 2's height
	*/
	static bool rectangleFitsRectangle(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
		return rectangleOverlapsRectangle(x2, y2, w2, h2, x1, y1, w1, h1);
	}
};