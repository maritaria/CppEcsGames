#include "mainmenu.h"
#include "../engine.h"

void Scene_MainMenu::enter() {
	selected = 0;

	float y = 60;
	float spacing = 20;
	int windowWidth = game->getWindow().getSize().x;

	auto& assets = game->getAssets();
	auto& font = assets.getFont("Mario");

	options.clear();
	auto& names = assets.getLevelNames();
	options.reserve(names.size());
	for (auto& levelName : names) {
		MenuItem item;
		item.name = levelName;
		auto& label = item.label;
		label.setString(levelName);
		label.setFont(font);
		auto bounds = label.getLocalBounds();
		label.setPosition(vec2((windowWidth - bounds.width) / 2, y));
		options.push_back(item);
		y += bounds.height + spacing;
	}
}

void Scene_MainMenu::leave() {
	options.clear();
}

void Scene_MainMenu::tick() {
	sysRender();
}

void Scene_MainMenu::perform(const Command& action) {
	if (action.ended) return;
	switch (action.type) {
		case Command::Up:
		{
			selected--;
			if (selected < 0) {
				selected = options.size() - 1;
			}
			break;
		}
		case Command::Down:
		{
			selected++;
			if (selected >= options.size()) {
				selected = 0;
			}
			break;
		}
		case Command::Fire:
		{
			// Note: Create copy on stack because the Scene_MainMenu::leave() method clears the vector to which the reference holds.
			auto item(options.at(selected));
			game->playLevel(item.name);
			break;
		}
	}
}

void Scene_MainMenu::sysRender() {
	auto& window = game->getWindow();
	window.clear();
	for (auto it = options.begin(); it != options.end(); ++it) {
		auto index = std::distance(options.begin(), it);
		auto& label = it->label;

		if (index == selected) {
			label.setFillColor(sf::Color::Red);
		} else {
			label.setFillColor(sf::Color::White);
		}

		window.draw(label);
	}
	window.display();
}
