#include "animation.h"

Animation::Animation(const std::string& name, const sf::Texture& source, vec2 frameSize, int delay)
	: name(name)
	, sprite(source, sf::IntRect(vec2::zero(), frameSize))
	, size(frameSize)
	, length(source.getSize().x / frameSize.x)
	, delay(delay) {
	sprite.setOrigin(frameSize / 2);
	updateSprite();
}

void Animation::update()
{
	timer++;
	if (timer > delay) {
		timer = 0;
		if (index < length) {
			index++;
			updateSprite();
		}
	}
}

void Animation::reset()
{
	index = 0;
	timer = 0;
}

bool Animation::hasEnded() const
{
	return (index >= length);
}

const sf::Sprite& Animation::getSprite() const
{
	return sprite;
}

void Animation::updateSprite()
{
	if (index >= 0 && index < length) {
		auto rect = sf::IntRect(index * size.x, 0, size.x, size.y);
		sprite.setTextureRect(rect);
	}
}
