#pragma once

#include <string>
#include <memory>

#include "entities.h"
#include "commands.h"

class GameEngine;

class Scene
{
public:
	Scene(GameEngine* game)
		: game(game) {
	}
	virtual void enter() = 0;
	virtual void leave() = 0;
	virtual void perform(const Command& action) = 0;
	virtual void tick() = 0;
protected:
	GameEngine* game;
	Entities entities;
};
