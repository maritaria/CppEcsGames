#include "geometry.h"

float vec2::lengthSquare() {

	return (x * x) + (y * y);
}

float vec2::length() {
	return sqrt(lengthSquare());
}

vec2 vec2::normalized()
{
	float vectorLength = length();
	return vec2(x / vectorLength, y / vectorLength);
}

float vec2::angle() {
	return atan2f(y, x);
}

void vec2::rotate(float radians)
{
	float vectorLength = length();
	float newAngle = angle() + radians;
	x = cos(newAngle) * vectorLength;
	y = sin(newAngle) * vectorLength;
}

void vec2::rotateAround(vec2& sun, float radians)
{
	vec2 delta = (*this - sun);
	delta.rotate(radians);
	*this = sun + delta;
}
