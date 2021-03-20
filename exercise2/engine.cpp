#include "engine.h"
#include "components.h"
#include "random.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
#include <sstream>

Game::Game(const std::string& configPath)
{
	init(configPath);
	setupNewGame();
}

void Game::init(const std::string& configPath)
{
	// Open config file
	std::ifstream file("resources/config-exercise2.txt");
	if (!file.is_open()) {
		std::cerr << "Error: Failed to load configuration file!" << std::endl;
		exit(-1);
	}

	// Parse line-by-line
	std::string lineText;
	while (file.good() && std::getline(file, lineText)) {
		std::istringstream line(lineText);
		std::string instruction;
		if (!(line >> instruction)) { continue; }
		if (instruction == "Window") {
			/**
			 * Window W H FL FS
			 * W - SFML window width
			 * H - SFML window height
			 * FL - Frame limit of SFML window
			 * FS - Fullscreen mode (0 = windowed, 1 = fullscreen)
			 */
			int width, height, frameLimit, fullScreen;
			line >> width >> height >> frameLimit >> fullScreen;
			m_window.create(sf::VideoMode(width, height), "Hemroids destroyer", fullScreen ? sf::Style::Fullscreen : (sf::Style::Titlebar | sf::Style::Close));
			m_window.setFramerateLimit(frameLimit);
			m_window.setKeyRepeatEnabled(false);
		}
		if (instruction == "Font") {
			/**
			 * Font F S R G B
			 * F (string) - Path to font (.ttf) file
			 * S (int) - Size to render text by
			 * R G B (int) - Color to render text by (0-255 each)
			 */
			std::string path;
			int characterSize;
			int r, g, b;
			line >> path >> characterSize >> r >> g >> b;
			if (!m_font.loadFromFile(path)) {
				std::cerr << "Error: Failed to load font file!" << std::endl;
				exit(-1);
			}
			m_text.setFont(m_font);
			m_text.setFillColor(sf::Color(r, g, b));
		}
		if (instruction == "Player") {
			/**
			 * Player SR CR S FR FG FB OR OG OB OT V
			 * SR (int) - Shape radius
			 * CR (int) - Collision radius
			 * S (float) - Speed
			 * FR, FG, FB (int) - Fill color
			 * OR, OG, OB (int) - Outline color
			 * OT (int) - Outline thickness
			 * V (int) - Shape vertices
			 */
			line
				>> m_playerConfig.SR
				>> m_playerConfig.CR
				>> m_playerConfig.S
				>> m_playerConfig.FR
				>> m_playerConfig.FG
				>> m_playerConfig.FB
				>> m_playerConfig.OR
				>> m_playerConfig.OG
				>> m_playerConfig.OB
				>> m_playerConfig.OT
				>> m_playerConfig.V
				;
		}
		if (instruction == "Enemy") {
			/**
			 * Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
			 * SR (int) - Shape radius
			 * CR (int) - Collision radius
			 * SMIN, SMAX (float) - Minimum & maximum speed
			 * OR, OG, OB (int) - Outline color
			 * OT (int) - Outline thickness
			 * VMIN, VMAX (int) - Minimum & maximum vertices
			 * L (int) - Lifespan (in frames)
			 * SP (int) - Spawn interval (in frames)
			 */
			line
				>> m_enemyConfig.SR
				>> m_enemyConfig.CR
				>> m_enemyConfig.sMin
				>> m_enemyConfig.sMax
				>> m_enemyConfig.OR
				>> m_enemyConfig.OG
				>> m_enemyConfig.OB
				>> m_enemyConfig.OT
				>> m_enemyConfig.vMin
				>> m_enemyConfig.vMax
				>> m_enemyConfig.L
				>> m_enemyConfig.SP
				;
		}
		if (instruction == "Bullet") {
			/**
			 * Bullet SR CR S FR FG FB OR OG OB OT V L
			 * SR (int) - Shape radius
			 * CR (int) - Collision radius
			 * S (float) - Speed
			 * FR, FG, FB (int) - Fill color
			 * OR, OG, OB (int) - Outline color
			 * OT (int) - Outline thickness
			 * V (int) - Shape vertices
			 * L (int) - Lifespan (in frames)
			 */
			line
				>> m_bulletConfig.SR
				>> m_bulletConfig.CR
				>> m_bulletConfig.S
				>> m_bulletConfig.FR
				>> m_bulletConfig.FG
				>> m_bulletConfig.FB
				>> m_bulletConfig.OR
				>> m_bulletConfig.OG
				>> m_bulletConfig.OB
				>> m_bulletConfig.OT
				>> m_bulletConfig.V
				>> m_bulletConfig.L
				;
		}
	}
}

void Game::setupNewGame() {
	m_entities.clear();
	m_score = 0;
	m_currentFrame = 0;
	m_spawnCounter = 0;
	m_killed = false;
	m_paused = false;
	spawnPlayer();
}

void Game::run()
{
	// TODO: Pause functionality
	// Some systems function during pause, some dont
	while (m_running)
	{
		sLifespan();
		m_entities.update();

		sUserInput();

		if (!m_paused) {
			sEnemySpawner();
			sMovement();
			sCollision();
		}

		sRender();

		m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::sMovement()
{
	vec2 windowSize = m_window.getSize();
	for (auto& entity : m_entities.getAll())
	{
		if (entity->cTransform)
		{
			auto& transform = *entity->cTransform;
			transform.position += transform.velocity;
			transform.angle += transform.twist;
			float radius = entity->cCollision ? entity->cCollision->radius : 0;
			auto policy = entity->cCollision ? entity->cCollision->policy : CCollision::Kill;

			// Determine trigger bounds
			bool smallerBounds = policy == CCollision::Contain || policy == CCollision::Bounce;
			vec2 boundsMin = smallerBounds
				? vec2(radius, radius)
				: vec2(-radius, -radius);
			vec2 boundsMax = smallerBounds
				? vec2(windowSize.x - radius, windowSize.y - radius)
				: vec2(windowSize.x + radius, windowSize.y + radius);

			// Apply policy logic
			switch (policy) {
				case CCollision::Contain:
				{
					if (transform.position.x < boundsMin.x) {
						transform.position.x = boundsMin.x;
					}
					if (transform.position.x > boundsMax.x) {
						transform.position.x = boundsMax.x;
					}
					if (transform.position.y < boundsMin.y) {
						transform.position.y = boundsMin.y;
					}
					if (transform.position.y > boundsMax.y) {
						transform.position.y = boundsMax.y;
					}
					break;
				}
				case CCollision::Bounce:
				{
					if (transform.position.x < boundsMin.x) {
						transform.position.x = boundsMin.x + (boundsMin.x - transform.position.x);
						transform.velocity.x *= -1;
					}
					if (transform.position.x > boundsMax.x) {
						transform.position.x = boundsMax.x + (boundsMax.x - transform.position.x);
						transform.velocity.x *= -1;
					}
					if (transform.position.y < boundsMin.y) {
						transform.position.y = boundsMin.y + (boundsMin.y - transform.position.y);
						transform.velocity.y *= -1;
					}
					if (transform.position.y > boundsMax.y) {
						transform.position.y = boundsMax.y + (boundsMax.y - transform.position.y);
						transform.velocity.y *= -1;
					}
					break;
				}
				case CCollision::Wrap:
				{
					if (transform.position.x < boundsMin.x) {
						transform.position.x = boundsMax.x;
					}
					if (transform.position.x > boundsMax.x) {
						transform.position.x = boundsMin.x;
					}
					if (transform.position.y < boundsMin.y) {
						transform.position.y = boundsMax.y;
					}
					if (transform.position.y > boundsMax.y) {
						transform.position.y = boundsMin.y;
					}
					break;
				}
				case CCollision::Kill:
				{
					if (
						(transform.position.x < boundsMin.x) ||
						(transform.position.x > boundsMax.x) ||
						(transform.position.y < boundsMin.y) ||
						(transform.position.y > boundsMax.y)
					) {
						m_entities.remove(entity->id);
					}
					break;
				}
			}
		}
	}
}

void Game::sLifespan()
{
	for (auto& entity : m_entities.getAll())
	{
		if (entity->cLifespan)
		{
			auto& lifespan = *entity->cLifespan;
			if (m_paused)
			{
				lifespan.frameCreated++;
			}
			if (lifespan.frameCreated + lifespan.lifespan < m_currentFrame)
			{
				m_entities.remove(entity);
			}
		}
	}
}

void Game::sUserInput()
{
	sf::Event event;
	auto& input = *m_player->cInput;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
			m_running = false;
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::A:
				input.left = true;
				break;
			case sf::Keyboard::D:
				input.right = true;
				break;
			case sf::Keyboard::W:
				input.up = true;
				break;
			case sf::Keyboard::S:
				input.down = true;
				break;
			case sf::Keyboard::Space:
			case sf::Keyboard::Escape:
				if (m_killed) {
					setupNewGame();
				} else {
					setPaused(!m_paused);
				}
				break;
			}
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::A:
				input.left = false;
				break;
			case sf::Keyboard::D:
				input.right = false;
				break;
			case sf::Keyboard::W:
				input.up = false;
				break;
			case sf::Keyboard::S:
				input.down = false;
				break;
			}
		}
		else if (event.type == sf::Event::MouseButtonPressed)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:
				input.shoot = true;
				break;
			case sf::Mouse::Right:
				input.special = true;
				break;
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:
				input.shoot = false;
				break;
			case sf::Mouse::Right:
				input.special = false;
				break;
			}
		}
	}
	// Apply active user inputs
	auto& transform = *m_player->cTransform;
	transform.velocity = vec2::zero();
	if (input.left) {
		transform.velocity.x -= 1;
	}
	if (input.right) {
		transform.velocity.x += 1;
	}
	if (input.up) {
		transform.velocity.y -= 1;
	}
	if (input.down) {
		transform.velocity.y += 1;
	}
	// Scale the movement speed to the config's velocity setting
	if (transform.velocity.lengthSquare() > 0) {
		transform.velocity = transform.velocity.normalized() * m_playerConfig.V;
	}

	if (m_paused) return;

	// Shoot
	if (input.shoot)
	{
		auto mousePos = sf::Mouse::getPosition(m_window);
		spawnBullet(m_player, mousePos);
		input.shoot = false;
	}
}

void Game::sRender()
{
	m_window.clear(sf::Color::Black);

	// Render entities
	for (auto entity : m_entities.getAll())
	{
		if (entity->cShape)
		{
			auto ptr = entity->cShape.get();
			sf::CircleShape& shape = entity->cShape->circle;

			// Check if the position can be updated
			if (entity->cTransform) {
				shape.setPosition(entity->cTransform->position);
				shape.setRotation(to_degrees(entity->cTransform->angle));
			}

			m_window.draw(shape);
		}
	}

	// Render UI
	std::stringstream scoreText;
	if (m_paused) {
		scoreText << "Game paused. Your score: " << m_score;
		m_text.setString(scoreText.str());
		auto rect = m_text.getLocalBounds();
		auto windowSize = m_window.getSize();
		m_text.setPosition((windowSize.x - rect.width) / 2, 20);
	} else {
		scoreText << m_score;
		m_text.setString(scoreText.str());
		auto rect = m_text.getLocalBounds();
		auto playerPos = m_player->cTransform->position;
		m_text.setPosition(playerPos.x - rect.width / 2, playerPos.y - rect.height / 2);
	}
	m_window.draw(m_text);

	// Swap buffers
	m_window.display();
}

void Game::sEnemySpawner()
{
	// Increase spawn speed as score increases
	int increment = log10(10 + m_score);
	int maxEnemies = 1 + (m_score / 100);
	m_spawnCounter += increment;
	if (m_spawnCounter >= m_enemyConfig.SP) {
		if (m_entities.countByTag(Entity::Enemy) < maxEnemies) {
			spawnEnemy();
		}
		m_spawnCounter = 0;
	}
}

bool checkTagPair(Entity& a, Entity& b, Entity::Tag tag1, Entity::Tag tag2)
{
	return
		(a.tag == tag1 && b.tag == tag2) ||
		(a.tag == tag2 && b.tag == tag1);
}

bool runTagLogic(std::shared_ptr<Entity>& a, std::shared_ptr<Entity>& b, Entity::Tag tag1, Entity::Tag tag2, std::function<void(std::shared_ptr<Entity>&, std::shared_ptr<Entity>&)> callback)
{
	if (a->tag == tag1 && b->tag == tag2)
	{
		callback(a, b);
		return true;
	}
	else if (b->tag == tag1 && a->tag == tag2)
	{
		callback(b, a);
		return true;
	}
	else
	{
		return false;
	}
}

void Game::sCollision()
{
	auto entities = m_entities.getAll();
	std::vector<std::shared_ptr<Entity>> priorCollidables;
	for (auto entity : entities)
	{
		if (entity->cTransform && entity->cCollision) {
			// Check for collisions with other entities
			for (auto other : priorCollidables) {
				vec2 delta = entity->cTransform->position - other->cTransform->position;
				float distanceSqr = delta.length();
				float maxDistance = entity->cCollision->radius + other->cCollision->radius;

				if (distanceSqr < maxDistance)
				{
					// The two entities are colliding
					// TODO: Logic ;)
					runTagLogic(entity, other, Entity::Enemy, Entity::Bullet, [this](std::shared_ptr<Entity>& enemy, std::shared_ptr<Entity>& bullet) {
						if (enemy->dead || bullet->dead) return;
						m_score += enemy->cScore->score;
						spawnSmallEnemies(enemy);
						m_entities.remove(enemy->id);
						m_entities.remove(bullet->id);
					});
					runTagLogic(entity, other, Entity::EnemyChild, Entity::Bullet, [this](std::shared_ptr<Entity>& enemy, std::shared_ptr<Entity>& bullet) {
						if (enemy->dead || bullet->dead) return;
						m_score += enemy->cScore->score;
						m_entities.remove(enemy->id);
						m_entities.remove(bullet->id);
					});
					runTagLogic(entity, other, Entity::Player, Entity::Enemy, [this](std::shared_ptr<Entity>& player, std::shared_ptr<Entity>& enemy) {
						if (enemy->dead) return;
						setPaused(true);
					});
					runTagLogic(entity, other, Entity::Player, Entity::EnemyChild, [this](std::shared_ptr<Entity>& player, std::shared_ptr<Entity>& enemy) {
						if (enemy->dead) return;
						setPaused(true);
						m_killed = true;
					});
				}
			}

			// Add to list of other collisions to check
			priorCollidables.push_back(entity);
		}
	}
}

void Game::spawnPlayer()
{
	// TODO: Finish adding all properties of the player with the correct values from the config

	// We create every entity by calling EntityManager.create(tag)
	auto entity = m_entities.create(Entity::Player);
	auto startingPosition = vec2(200.0f, 200.0f);
	entity->cTransform = std::make_shared<CTransform>(startingPosition, vec2::zero(), 0.0f, to_radians(-1));
	entity->cShape = std::make_shared<CShape>(
		m_playerConfig.SR,
		m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT
	);
	entity->cInput = std::make_shared<CInput>();
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR, CCollision::Contain);
	m_player = entity;
}

void Game::spawnEnemy()
{
	std::cout << "spawnEnemy()" << std::endl;

	// Find a spot for the new enemy
	auto size = m_window.getSize();
	vec2 pos;
	while (true) {
		pos = vec2(
			randomFromRange(0, size.x),
			randomFromRange(0, size.y)
		);
		float distanceFromPlayer = (pos - m_player->cTransform->position).length();
		float clearance = (m_player->cCollision->radius + m_enemyConfig.SR) * 5;
		if (distanceFromPlayer > clearance) {
			break;
		}
	}

	// Pick random direction for enemy to move in
	float speed = randomFloat(m_enemyConfig.sMin, m_enemyConfig.sMax);
	vec2 velocity = vec2(randomFloat() - 0.5, randomFloat() - 0.5).normalized() * speed;

	// Pick vertices count
	int vertices = randomFromRange(m_enemyConfig.vMin, m_enemyConfig.vMax);

	// Setup the entity
	auto entity = m_entities.create(Entity::Enemy);
	auto spin = (360 / vertices) * (speed / m_enemyConfig.sMax);
	entity->cTransform = std::make_shared<CTransform>(
		pos,
		velocity,
		0.0f,
		to_radians(spin) / 60
	);
	entity->cShape = std::make_shared<CShape>(
		m_enemyConfig.SR,
		vertices,
		sf::Color::Transparent,
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
		m_enemyConfig.OT
	);
	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR, CCollision::Bounce);
	entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L, m_currentFrame);
	entity->cScore = std::make_shared<CScore>(vertices);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> killedEnemy)
{
	float scale = 0.5f;
	auto circle = killedEnemy->cShape->circle;
	auto transform = killedEnemy->cTransform;
	auto pos = transform->position;
	auto vertices = circle.getPointCount();
	float childRotation = pi * 2 / vertices;
	for (int index = 0; index < vertices; index++) {
		auto velocity = vec2::up();
		velocity.rotate(childRotation * index);
		auto childOffset = vec2::up() * (circle.getRadius() * scale);
		childOffset.rotate(childRotation * index);
		auto child = m_entities.create(Entity::EnemyChild);
		child->cTransform = std::make_shared<CTransform>(
			pos + childOffset,
			velocity,
			transform->angle,
			transform->twist * vertices
		);
		child->cShape = std::make_shared<CShape>(
			circle.getRadius() * scale,
			vertices,
			circle.getFillColor(),
			circle.getOutlineColor(),
			circle.getOutlineThickness()
		);
		child->cCollision = std::make_shared<CCollision>(killedEnemy->cCollision->radius * scale, CCollision::Wrap);
		child->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L / vertices, m_currentFrame);
		child->cScore = std::make_shared<CScore>(1);
	}
}

void Game::spawnBullet(std::shared_ptr<Entity> shooter, const vec2& mouse)
{
	auto bullet = m_entities.create(Entity::Bullet);
	auto transform = shooter->cTransform;
	vec2 pos = transform->position;
	vec2 aimDelta = (mouse - pos).normalized();
	float shooterRadius = shooter->cCollision ? shooter->cCollision->radius : 1;
	bullet->cTransform = std::make_shared<CTransform>(
		pos + (aimDelta * shooterRadius),
		aimDelta * m_bulletConfig.S,
		transform->angle,
		transform->twist * (shooterRadius / m_bulletConfig.SR)
	);
	bullet->cShape = std::make_shared<CShape>(
		m_bulletConfig.SR,
		m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		m_bulletConfig.OT
	);
	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR, CCollision::Kill);
	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L, m_currentFrame);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> shooter)
{
}
