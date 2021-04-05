#pragma once

#include <unordered_map>
#include <SFML/Window.hpp>

#include "commands.h"
#include "scene.h"
#include "assets.h"

class GameEngine
{
public:
	GameEngine();

	void run();

	void playLevel(const std::string& name);
	void playMainMenu();

	const Assets& getAssets();
	sf::RenderWindow& getWindow();

private:
	Assets assets;
	std::map<std::string, std::shared_ptr<Scene>> scenes;
	std::unordered_map<sf::Keyboard::Key, Command::Type> actions;
	std::shared_ptr<Scene> activeScene;
	sf::RenderWindow window;

	void processInput();
};
