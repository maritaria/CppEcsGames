#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "geometry.h"

class Animation
{
public:
	Animation() = default;
	Animation(const std::string& name, const sf::Texture& source, vec2 frameSize, int delay);
	const std::string& getName() const { return name; };
	const vec2& getSize() const { return size; };
	void update();
	void reset();
	bool hasEnded() const;

	const sf::Sprite& getSprite() const;

private:
	void updateSprite();

	std::string name;
	sf::Sprite sprite;
	vec2 size;
	int length = 1;
	int index = 0;
	int delay = 0;
	int timer = 0;
};
