#include "engine.h"
#include "scenes/mainmenu.h"
#include "scenes/playlevel.h"

#include <iostream>
#include <memory>

GameEngine::GameEngine() {
	// Load assets
	assets.loadResources("./resources");

	// Key bindings
	actions[sf::Keyboard::W] = Command::Up;
	actions[sf::Keyboard::A] = Command::Left;
	actions[sf::Keyboard::S] = Command::Down;
	actions[sf::Keyboard::D] = Command::Right;
	actions[sf::Keyboard::Space] = Command::Fire;
	actions[sf::Keyboard::Escape] = Command::Pause;
	actions[sf::Keyboard::Num1] = Command::DebugTextures;
	actions[sf::Keyboard::Num2] = Command::DebugBoxes;
	actions[sf::Keyboard::Num3] = Command::DebugGrid;

	// Window
	window.create(sf::VideoMode(1980, 1080), "Exercise 3", sf::Style::Close | sf::Style::Titlebar);
	window.setFramerateLimit(60);

	// Setup scenes
	scenes["MainMenu"] = std::make_shared<Scene_MainMenu>(this);
	scenes["PlayLevel"] = std::make_shared<Scene_PlayLevel>(this);
	playMainMenu();
}

void GameEngine::run() {
	while (activeScene) {
		// Parse inputs
		processInput();

		// Run game tick
		activeScene->tick();
	}
}

void GameEngine::playLevel(const std::string& name) {
	if (activeScene) {
		activeScene->leave();
	}
	std::shared_ptr<Scene_PlayLevel> scene = std::dynamic_pointer_cast<Scene_PlayLevel, Scene>(scenes.at("PlayLevel"));
	scene->setLevel(assets.getLevel(name));
	activeScene = scene;
	activeScene->enter();
}

void GameEngine::playMainMenu() {
	if (activeScene) {
		activeScene->leave();
	}
	activeScene = scenes.at("MainMenu");
	activeScene->enter();
}

const Assets& GameEngine::getAssets() {
	return assets;
}

sf::RenderWindow& GameEngine::getWindow() {
	return window;
}

void GameEngine::processInput() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
			activeScene->leave();
			return std::exit(0);
		} else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
			Command command;
			auto pair = actions.find(event.key.code);
			if (pair != actions.end()) {
				command.type = pair->second;
				command.ended = event.type == sf::Event::KeyReleased;
				activeScene->perform(command);
			}
		}
	}
}
