#include "assets.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

void Assets::loadResources(const std::string& basePath) {
	std::cout << "Loading resources from " << std::quoted(basePath) << std::endl;
	fs::path dir(basePath);

	// Open assets file
	std::ifstream file(dir / "assets.txt");
	if (!file.is_open()) {
		throw std::exception("Failed to open assets file");
	}

	// Parse line-by-line
	std::string lineText;
	while (file.good() && std::getline(file, lineText)) {
		std::istringstream line(lineText);
		std::string instruction;
		// Check for reasons to skip the line
		if (
			!(line >> instruction) || // line is empty
			(instruction[0] == '#') // line is comment (starts with #)
		) {
			continue;
		}
		// Handle instruction types
		if (instruction == "Texture") {
			TextureConfig config;
			line >> config;
			auto& texture = textures[config.name];
			if (texture.loadFromFile((dir / config.path).string())) {
				auto size = texture.getSize();
				std::cout << "Loaded texture " << std::quoted(config.path) << " as " << std::quoted(config.name) << " (" << size.x << "x" << size.y << ")" << std::endl;
			} else {
				throw std::runtime_error("Failed to load texture '" + config.name + "' from " + config.path);
			}
		} else if (instruction == "Animation") {
			AnimationConfig config;
			line >> config;
			const sf::Texture& texture = getTexture(config.texture);
			auto size = texture.getSize();
			animations[config.name] = Animation(config.name, texture, vec2(size.x / config.length, size.y), config.delay);
			std::cout << "Loaded animation " << std::quoted(config.name) << " using " << std::quoted(config.texture) << ", " << config.length << " frames" << std::endl;
		} else if (instruction == "Font") {
			FontConfig config;
			line >> config;
			sf::Font& font = fonts[config.name];
			if (font.loadFromFile((dir / config.path).string())) {
				std::cout << "Loaded font " << std::quoted(config.name) << " from " << std::quoted(config.path) << std::endl;
			} else {
				throw std::runtime_error("Failed to load font '" + config.name + "' from " + config.path);
			}
		} else if (instruction == "Level") {
			LevelConfig config;
			line >> config;
			config.path = (dir / config.path).string();
			levels[config.name] = config;
			std::cout << "Registered level " << std::quoted(config.name) << " as " << std::quoted(config.path) << std::endl;
		} else {
			std::cout << "Unknown instruction type: " << std::quoted(instruction) << std::endl;
		}
	}
	std::cout << "Assets loaded ("
		<< textures.size() << " textures, "
		<< animations.size() << " animations, "
		<< fonts.size() << " fonts, "
		<< levels.size() << " levels)"
		<< std::endl;
}

const sf::Texture& Assets::getTexture(const std::string& name) const {
	try {
		return textures.at(name);
	} catch (std::out_of_range& exception) {
		throw MissingAssetException("Texture", name);
	}
}

const sf::Font& Assets::getFont(const std::string& name) const
{
	try {
		return fonts.at(name);
	} catch (std::out_of_range& exception) {
		throw MissingAssetException("Font", name);
	}
}

const Animation& Assets::getAnimation(const std::string& name) const {
	try {
		return animations.at(name);
	} catch (std::out_of_range& exception) {
		throw MissingAssetException("Animation", name);
	}
}

const LevelConfig& Assets::getLevel(const std::string& name) const {
	try {
		return levels.at(name);
	} catch (std::out_of_range& exception) {
		throw MissingAssetException("Level", name);
	}
}

const std::vector<std::string> Assets::getLevelNames() const
{
	std::vector<std::string> result;
	for (auto& pair : levels) {
		result.push_back(pair.first);
	}
	return result;
}
