#pragma once

#include "../scene.h"

struct MenuItem
{
	std::string name;
	sf::Text label;
};

class Scene_MainMenu : public Scene
{
public:
	Scene_MainMenu(GameEngine* game) : Scene(game) {}

	void enter();
	void leave();
	void perform(const Command& action);
	void tick();

private:
	void sysRender();

	std::vector<MenuItem> options;
	size_t selected = 0;
};
