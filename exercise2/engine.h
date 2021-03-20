#pragma once
#include <SFML/Graphics.hpp>
#include "entities.h"
#include "geometry.h"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, vMin, vMax, L, SP; float sMin, sMax; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow m_window;
	bool m_paused = false;
	bool m_running = true;
	bool m_killed = false;
	EntityManager m_entities;
	sf::Font m_font;
	sf::Text m_text;
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	BulletConfig m_bulletConfig;
	int m_score = 0;
	int m_spawnCounter = 0;
	int m_currentFrame = 0;
	sf::Clock m_clock;
	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);
	void setupNewGame();
	void setPaused(bool paused);

	void sMovement();
	void sLifespan();
	void sUserInput();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> shooter, const vec2& mouse);
	void spawnSpecialWeapon(std::shared_ptr<Entity> shooter);

public:
	Game(const std::string& configPath);
	void run();
};
