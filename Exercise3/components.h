#pragma once

#include <tuple>
#include "animation.h"
#include "geometry.h"

struct Component
{
	bool active = false;
};

struct CTransform : public Component
{
	vec2 position;
	vec2 previousPosition;
	vec2 scale = vec2::one();
	vec2 velocity;
	float angle = 0.0f;
	float spin = 0.0f;

	CTransform() = default;
};

struct CInput : public Component
{
	bool left = false;
	bool right = false;
	bool jump = false;
	bool shoot = false;

	CInput() = default;
};

struct CPlayerState : public Component
{
	bool canJump = false;
	int jumpStart = 0;
	int maxJumpLength = 1; // frames
};

struct CBoundingBox : public Component
{
	rect box;

	CBoundingBox() = default;
};

struct CAnimation : public Component
{
	Animation animation;
	bool loop = false;
	CAnimation() = default;
};

struct CCoinBox : public Component
{
	bool hit = false;
};

typedef std::tuple<
	CTransform,
	CInput,
	CPlayerState,
	CBoundingBox,
	CAnimation,
	CCoinBox
> ComponentTuple;
