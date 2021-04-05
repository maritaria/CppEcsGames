#pragma once

#include "../assets.h"
#include "../scene.h"

struct PlayerConfig
{
	float movementSpeed = 0;
	float jumpVelocity = 0;
	float maxSpeed = 0;
	float gravity = 0;
	std::string bulletAnimation;
};

class Scene_PlayLevel : public Scene
{
public:
	Scene_PlayLevel(GameEngine* game) : Scene(game) {}

	void enter();
	void leave();
	void perform(const Command& action);
	void tick();

	void setLevel(const LevelConfig& config);
	void resetLevel();

	vec2 gridToPixel(const vec2& gridPos) const;
	vec2 pixelToGrid(const vec2& screenPos) const;
	vec2 getTileSize() const { return tileSize; }

private:
	// Systems
	void sysEntities();
	void sysGravity();
	void sysInput();
	void sysMovement();
	void sysCollision();
	void sysAnimation();
	void sysRender();
	void sysPreviousPosition();

	void onShootBullet(const EntityPtr& player);
	void onCollision(const EntityPtr& player, const EntityPtr& tile, const rect& overlap);
	void onCoinBoxHit(const EntityPtr& player, const EntityPtr& tile);

	// Data
	int frame = 0;
	bool paused = false;
	LevelConfig levelConfig;
	vec2 tileSize = vec2(128, 128);
	vec2 windowScroll = vec2::zero();
	vec2 levelSize = vec2::zero();
	PlayerConfig playerConfig;
	bool drawTextures = true;
	bool drawBoxes = false;
	bool drawGrid = false;
};
