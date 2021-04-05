#pragma once

#include <cmath>
#include <ostream>
#include <SFML/System/Vector2.hpp>

constexpr double pi = 3.141592653589793238462643383279502884L;
constexpr float to_degrees(float radians) { return radians * (180.0f / pi); }
constexpr float to_radians(float degrees) { return degrees * (pi / 180.0f); }

struct vec2
{
	float x = 0;
	float y = 0;

	vec2()
		: x(0)
		, y(0)
	{}

	vec2(float x, float y)
		: x(x)
		, y(y)
	{}

	vec2(const sf::Vector2f & from)
		: x(from.x)
		, y(from.y)
	{}

	vec2(const sf::Vector2i & from)
		: x(from.x)
		, y(from.y)
	{}

	vec2(const sf::Vector2u & from)
		: x(from.x)
		, y(from.y)
	{}

	inline operator sf::Vector2f() const
	{
		return sf::Vector2f(x, y);
	}

	float lengthSquare();

	float length();

	vec2 normalized();

	float angle();

	void rotate(float radians);

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


inline vec2 operator -(const vec2& rhs) {
	return vec2(-rhs.x, -rhs.y);
}

inline vec2 operator +(const vec2& lhs, const vec2& rhs)
{
	return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline vec2& operator+=(vec2& lhs, const vec2& rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

inline vec2 operator -(const vec2& lhs, const vec2& rhs) {
	return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline vec2& operator-=(vec2& lhs, const vec2& rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

inline vec2 operator * (const vec2& lhs, float rhs)
{
	return vec2(lhs.x * rhs, lhs.y * rhs);
}

inline vec2 operator * (float rhs, const vec2& lhs)
{
	return vec2(lhs.x * rhs, lhs.y * rhs);
}

inline vec2& operator *=(vec2& left, float right)
{
	left.x *= right;
	left.y *= right;

	return left;
}

inline vec2 operator * (const vec2& lhs, const vec2& rhs)
{
	return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline vec2& operator *=(vec2& left, vec2& right)
{
	left.x *= right.x;
	left.y *= right.y;

	return left;
}

inline vec2 operator /(const vec2& left, float right)
{
	return vec2(left.x / right, left.y / right);
}

inline vec2& operator /=(vec2& left, float right)
{
	left.x /= right;
	left.y /= right;

	return left;
}

inline bool operator ==(const vec2& left, const vec2& right)
{
	return (left.x == right.x) && (left.y == right.y);
}

inline bool operator !=(const vec2& left, const vec2& right)
{
	return (left.x != right.x) || (left.y != right.y);
}

inline std::ostream& operator<<(std::ostream& stream, const vec2& vector) {
	return stream << "(" << vector.x << ", " << vector.y << ")";
}
