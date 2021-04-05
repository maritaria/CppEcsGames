#pragma once

#include <exception>
#include <map>
#include <string>
#include <istream>
#include <exception>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "animation.h"
#include "geometry.h"

struct TextureConfig
{
	// "Texture" <name> <path/image.png>
	std::string name;
	std::string path;
};

inline std::istream& operator>>(std::istream& input, TextureConfig& config) {
	return input
		>> config.name
		>> config.path;
}

struct AnimationConfig
{
	// "Animation" <name> <texture> <length> <delay>
	std::string name;
	std::string texture;
	int length;
	int delay;
};

inline std::istream& operator>>(std::istream& input, AnimationConfig& config) {
	return input
		>> config.name
		>> config.texture
		>> config.length
		>> config.delay;
}

struct FontConfig
{
	// "Font" <name> <path/font.ttf>
	std::string name;
	std::string path;
};

inline std::istream& operator>>(std::istream& input, FontConfig& config) {
	return input
		>> config.name
		>> config.path;
}

struct LevelConfig
{
	// "Level" <name> <level.txt>
	std::string name;
	std::string path;
};

inline std::istream& operator>>(std::istream& input, LevelConfig& config) {
	return input
		>> config.name
		>> config.path;
}

class MissingAssetException : public std::runtime_error
{
public:
	MissingAssetException(const std::string& type, const std::string& name)
		: std::runtime_error("Missing " + type + " asset '" + name)
		, assetType(type)
		, assetName(name) {
	}

	const std::string& getAssetType() { return assetType; }
	const std::string& getAssetName() { return assetName; }

private:
	const std::string assetType;
	const std::string assetName;
};

class Assets
{
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, sf::Sound> sounds;
	std::map<std::string, sf::Font> fonts;
	std::map<std::string, Animation> animations;
	std::map<std::string, LevelConfig> levels;

public:
	void loadResources(const std::string& path);

	const sf::Texture& getTexture(const std::string& name) const;
	const sf::Sound& getSound(const std::string& name) const;
	const sf::Font& getFont(const std::string& name) const;
	const Animation& getAnimation(const std::string& name) const;
	const LevelConfig& getLevel(const std::string& name) const;

	const std::vector<std::string> getLevelNames() const;

};
