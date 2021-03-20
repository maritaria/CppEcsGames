#pragma once
#include <SFML/Graphics.hpp>

#include "geometry.h"

struct CTransform
{
	vec2 position;
	vec2 velocity;
	float angle;
	float twist;

	CTransform(const vec2& position, const vec2& velocity, float angle, float twist)
		: position(position)
		, velocity(velocity)
		, angle(angle)
		, twist(twist)
	{}
};

struct CShape
{
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, int thickness)
		:circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

struct CCollision
{
	enum BoundsPolicy {
		Contain,
		Wrap,
		Bounce,
		Kill,
	};

	float radius;
	BoundsPolicy policy;

	CCollision(float radius, BoundsPolicy policy)
		: radius(radius)
		, policy(policy)
	{}
};

struct CScore
{
	int score;

	CScore(int score)
		: score(score)
	{}
};

struct CLifespan
{
	int lifespan;
	int frameCreated;

	CLifespan(int lifespan, int frameCreated)
		: lifespan(lifespan)
		, frameCreated(frameCreated)
	{}
};

struct CInput
{
	bool up;
	bool left;
	bool right;
	bool down;
	bool shoot;
	bool special;
};
