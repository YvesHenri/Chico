#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace math
{
	class Angle final
	{
	public:
		Angle() = delete;
		Angle(const Angle&) = default;
		Angle(Angle&&) = default;

		static const Angle D0;
		static const Angle D45;
		static const Angle D90;
		static const Angle D135;
		static const Angle D180;
		static const Angle D225; // 225 - 360 = -135
		static const Angle D270; // 270 - 360 = -90
		static const Angle D315; // 315 - 360 = -45
		static const Angle D360; // 360 - 360 = 0
		
		static Angle radians(float radians) {
			return Angle(radians);
		}

		static Angle degrees(float degrees) {
			return Angle(degrees * 0.0174533f);
		}

		float radians() const {
			return value;
		}

		float degrees() const {
			return value * 57.2957795f;
		}

		Angle& operator=(const Angle& other) {
			value = other.value;
			return *this;
		}

	private:
		Angle(float radians) : value(radians) {}

	private:
		float value;
	};

	const Angle Angle::D0 = Angle::radians(0.f);
	const Angle Angle::D45 = Angle::radians(0.7853985f);
	const Angle Angle::D90 = Angle::radians(1.570797f);
	const Angle Angle::D135 = Angle::radians(2.3561955f);
	const Angle Angle::D180 = Angle::radians(3.141594f);
	const Angle Angle::D225 = Angle::radians(3.9269925f);
	const Angle Angle::D270 = Angle::radians(4.712391f);
	const Angle Angle::D315 = Angle::radians(5.4977895f);
	const Angle Angle::D360 = Angle::radians(6.283188f);

	struct Vector final
	{
		Vector(const Vector&) = default;
		Vector(Vector&&) = default;
		Vector(const float& x = 0, const float& y = 0) : x(x), y(y) {}

		static const Vector Up;
		static const Vector Down;
		static const Vector Left;
		static const Vector Right;
		static const Vector Zero;

		static Vector polar(const float& magnitude, const Angle& angle) {
			auto x = magnitude * std::cosf(angle.radians());
			auto y = magnitude * std::sinf(angle.radians());
			return Vector(x, y);
		}

		float magnitude() const {
			return std::sqrtf(x * x + y * y);
		}

		Angle angle() const {
			auto radians = std::atan2f(y, x);
			return Angle::radians(radians);
		}

		Angle angle(const Vector& vector) const {
			auto d = dot(vector);
			auto m = magnitude() * vector.magnitude();
			auto r = std::acosf(d / m);
			return Angle::radians(r);
		}

		Vector clone() const {
			return Vector(x, y);
		}

		Vector rotated(const Angle& angle) const {
			float rx = x * std::cosf(angle.radians()) - y * std::sinf(angle.radians());
			float ry = x * std::sinf(angle.radians()) + y * std::cosf(angle.radians());
			return Vector(rx, ry);
		}

		Vector& rotate(const Angle& angle) {
			auto ox = x;
			x = ox * std::cosf(angle.radians()) - y * std::sinf(angle.radians());
			y = ox * std::sinf(angle.radians()) + y * std::cosf(angle.radians());
			return *this;
		}

		Vector clamped(const float& maxMagnitude) const {
			auto m = magnitude();
			
			if (m < std::numeric_limits<float>::epsilon()) {
				m = 1.f; // Make no change
			}
			else {
				m = std::fminf(m, maxMagnitude) / m;
			}
			
			return Vector(x * m, y * m);
		}

		Vector& clamp(const float& maxMagnitude) {
			auto m = magnitude();

			if (m > std::numeric_limits<float>::epsilon()) {
				m = std::fminf(m, maxMagnitude) / m;
				x *= m;
				y *= m;
			}

			return *this;
		}

		Vector normalized() const {
			auto m = magnitude(); 
			return m > std::numeric_limits<float>::epsilon() ? Vector(x / m, y / m) : Vector(x, y);
		}

		Vector& normalize() {
			auto m = magnitude();

			if (m > std::numeric_limits<float>::epsilon()) {
				x /= m;
				y /= m;
			}

			return *this;
		}

		Vector reversed() const {
			return Vector(-x, -y);
		}

		Vector& reverse() {
			x = -x;
			y = -y;
			return *this;
		}

		Vector scaled(const float& scalar) const {
			return Vector(x * scalar, y * scalar);
		}

		Vector& scale(const float& scalar) {
			x *= scalar;
			y *= scalar;
			return *this;
		}

		Vector tangented(const Vector& vector) const {
			return Vector(-vector.y, vector.x);
		}

		Vector& tangent(const Vector& vector) {
			x = -vector.y;
			y = vector.x;
			return *this;
		}

		Vector crossed(const Vector& vector) const {
			throw "NOT_IMPLEMENTED";
		}

		Vector& cross(const Vector& vector) {
			throw "NOT_IMPLEMENTED";
		}

		float dot(const Vector& vector) const {
			return x * vector.x + y * vector.y;
		}

		Vector& operator*=(const float& scalar) {
			return scale(scalar);
		}

		Vector& operator*=(const Vector& rhs) {
			return cross(rhs);
		}

		Vector& operator+=(const Vector& rhs) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		Vector& operator-=(const Vector& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		Vector& operator/=(const Vector& rhs) {
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}

		Vector& operator/=(const float& scalar) {
			x /= scalar;
			y /= scalar;
			return *this;
		}

		Vector& operator=(const Vector& other) {
			x = other.x;
			y = other.y;
			return *this;
		}

		friend Vector operator*(Vector lhs, const float& scalar) {
			return lhs.scaled(scalar);
		}

		friend Vector operator*(Vector lhs, const Vector& rhs) {
			return lhs.crossed(rhs);
		}

		friend Vector operator+(Vector lhs, const Vector& rhs) {
			lhs += rhs;
			return lhs;
		}

		friend Vector operator-(Vector lhs, const Vector& rhs) {
			lhs -= rhs;
			return lhs;
		}

		friend Vector operator/(Vector lhs, const Vector& rhs) {
			lhs /= rhs;
			return lhs;
		}

		friend Vector operator/(Vector lhs, const float& scalar) {
			lhs /= scalar;
			return lhs;
		}

		friend bool operator==(const Vector& lhs, const Vector& rhs) {
			return lhs.x == rhs.x && lhs.y == rhs.y;
		}

		friend bool operator!=(const Vector& lhs, const Vector& rhs) {
			return !(lhs == rhs);
		}

		float x;
		float y;
	};

	const Vector Vector::Up = Vector(0.f, 1.f);
	const Vector Vector::Down = Vector(0.f, -1.f);
	const Vector Vector::Left = Vector(-1.f, 0.f);
	const Vector Vector::Right = Vector(1.f, 0.f);
	const Vector Vector::Zero = Vector(0.f, 0.f);
}