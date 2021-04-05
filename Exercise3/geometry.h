#pragma once

#include <ostream>
#include <SFML/System/Vector2.hpp>

constexpr float pi = (float)3.141592653589793238462643383279502884L;
constexpr float to_degrees(float radians) { return radians * (180.0f / pi); }
constexpr float to_radians(float degrees) { return degrees * (pi / 180.0f); }

struct vec2
{
	float x = 0;
	float y = 0;

	vec2() = default;

	vec2(float x, float y)
		: x(x)
		, y(y) {
	}

	template<typename T>
	vec2(const sf::Vector2<T>& from)
		: x(from.x)
		, y(from.y) {
	}

	template<typename T>
	inline operator sf::Vector2<T>() const {
		return sf::Vector2<T>(x, y);
	}

	float lengthSquare();
	float length();
	vec2 normalized();
	float angle();
	void rotate(float radians);
	void clampLength(float min, float max);

	inline static vec2 zero() {
		return vec2(0, 0);
	}
	inline static vec2 one() {
		return vec2(1, 1);
	}
	inline static vec2 up() {
		return vec2(0, -1);
	}
	inline static vec2 down() {
		return vec2(0, 1);
	}
	inline static vec2 left() {
		return vec2(1, 0);
	}
	inline static vec2 right() {
		return vec2(-1, 0);
	}
};

inline vec2 operator-(const vec2& rhs) {
	return vec2(-rhs.x, -rhs.y);
}

inline vec2 operator+(const vec2& lhs, const vec2& rhs) {
	return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline vec2& operator+=(vec2& lhs, const vec2& rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

inline vec2 operator-(const vec2& lhs, const vec2& rhs) {
	return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline vec2& operator-=(vec2& lhs, const vec2& rhs) {
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

inline vec2 operator*(const vec2& lhs, float rhs) {
	return vec2(lhs.x * rhs, lhs.y * rhs);
}

inline vec2 operator*(float rhs, const vec2& lhs) {
	return vec2(lhs.x * rhs, lhs.y * rhs);
}

inline vec2& operator*=(vec2& left, float right) {
	left.x *= right;
	left.y *= right;

	return left;
}

inline vec2 operator*(const vec2& lhs, const vec2& rhs) {
	return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline vec2& operator*=(vec2& left, vec2& right) {
	left.x *= right.x;
	left.y *= right.y;

	return left;
}

inline vec2 operator/(const vec2& left, float right) {
	return vec2(left.x / right, left.y / right);
}

inline vec2& operator/=(vec2& left, float right) {
	left.x /= right;
	left.y /= right;

	return left;
}

inline vec2 operator/(const vec2& lhs, const vec2& rhs) {
	return vec2(lhs.x / rhs.x, lhs.y / rhs.y);
}

inline vec2& operator/=(vec2& left, vec2& right) {
	left.x /= right.x;
	left.y /= right.y;

	return left;
}

inline bool operator==(const vec2& left, const vec2& right) {
	return (left.x == right.x) && (left.y == right.y);
}

inline bool operator!=(const vec2& left, const vec2& right) {
	return (left.x != right.x) || (left.y != right.y);
}

inline std::ostream& operator<<(std::ostream& stream, const vec2& vector) {
	return stream << "(" << vector.x << ", " << vector.y << ")";
}

struct rect
{
	vec2 position;
	vec2 size;

	rect() = default;

	rect(const vec2& position, const vec2& size)
		: position(position)
		, size(size) {
	}

	rect(float x, float y, float w, float h)
		: rect(vec2(x, y), vec2(w, h)) {
	}

	inline float left() const { return position.x; }
	inline float right() const { return left() + size.x; }
	inline float top() const { return position.y; }
	inline float bottom() const { return top() + size.y; }

	rect overlap(const rect& other) const;
};
