#include "geometry.h"
#include <algorithm>

float vec2::lengthSquare()
{
	return (x * x) + (y * y);
}

float vec2::length()
{
	return sqrt(lengthSquare());
}

vec2 vec2::normalized()
{
	float vectorLength = length();
	return vec2(x / vectorLength, y / vectorLength);
}

float vec2::angle()
{
	return atan2f(y, x);
}

void vec2::rotate(float radians)
{
	float vectorLength = length();
	float newAngle = angle() + radians;
	x = cos(newAngle) * vectorLength;
	y = sin(newAngle) * vectorLength;
}

void vec2::clampLength(float min, float max)
{
	float length = this->length();
	if (length < min) {
		float factor = (min / length);
		x *= factor;
		y *= factor;
	} else if (length > max) {
		float factor = (max / length);
		x *= factor;
		y *= factor;
	}
}

rect rect::overlap(const rect& other) const
{
	// https://stackoverflow.com/a/52194761

	float left = fmax(position.x, other.position.x);
	float right = fmin(position.x + size.x, other.position.x + other.size.x);
	float top = fmax(position.y, other.position.y);
	float bottom = fmin(position.y + size.y, other.position.y + other.size.y);

	return rect(
		left,
		top,
		right - left,
		bottom - top
	);
}
