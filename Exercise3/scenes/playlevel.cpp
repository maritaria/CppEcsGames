#include "playlevel.h"
#include "../engine.h"
#include "../parser.h"
#include "../geometry.h"
#include <algorithm>
#include <deque>

void Scene_PlayLevel::enter() {
	frame = 0;
	paused = false;
	drawTextures = true;
	drawBoxes = false;
	drawGrid = false;
}

void Scene_PlayLevel::leave() {
	entities = Entities();
}

void Scene_PlayLevel::perform(const Command& action) {
	switch (action.type) {
		case Command::Pause:
			if (!action.ended) {
				paused = !paused;
			}
			break;
		case Command::DebugTextures:
			if (!action.ended) {
				drawTextures = !drawTextures;
			}
			break;
		case Command::DebugBoxes:
			if (!action.ended) {
				drawBoxes = !drawBoxes;
			}
			break;
		case Command::DebugGrid:
			if (!action.ended) {
				drawGrid = !drawGrid;
			}
			break;

		case Command::Left:
		case Command::Right:
		case Command::Up:
		case Command::Fire:
		{
			for (auto& entity : entities.list(Entity::Tag::Player)) {
				auto& input = entity->getComponent<CInput>();
				auto& state = entity->getComponent<CPlayerState>();

				switch (action.type) {
					case Command::Left: input.left = !action.ended; break;
					case Command::Right: input.right = !action.ended; break;
					case Command::Up: input.jump = !action.ended && state.canJump; break;
					case Command::Fire: input.shoot = !action.ended; break;
				}
			}
			break;
		}
	}
}

void Scene_PlayLevel::tick() {
	sysEntities();
	if (!paused) {
		sysGravity();
		sysInput();
		sysMovement();
		sysCollision();
		sysAnimation();
	}
	sysRender();
	sysPreviousPosition();
}

vec2 Scene_PlayLevel::gridToPixel(const vec2& gridPos) const {
	return (gridPos + vec2(0.5, 0.5)) * getTileSize();
}

vec2 Scene_PlayLevel::pixelToGrid(const vec2& screenPos) const {
	return (screenPos / getTileSize()) - vec2(0.5, 0.5);
}

void Scene_PlayLevel::setLevel(const LevelConfig& config) {
	levelConfig = config;
	resetLevel();
}

void Scene_PlayLevel::resetLevel() {
	entities.clear();
	std::cout << "Loading level " << std::quoted(levelConfig.name) << " from " << std::quoted(levelConfig.path) << std::endl;

	parser_map parsers;
	auto& assets = game->getAssets();

	levelSize = vec2::zero();

	parsers["Player"] = [=](auto& instruction, auto& stream) {
		float gridX = 0, gridY = 0, bboxX = 0, bboxY = 0, bboxW = 0, bboxH = 0;
		if (!(stream
			>> gridX
			>> gridY
			>> bboxX
			>> bboxY
			>> bboxW
			>> bboxH
			>> playerConfig.movementSpeed
			>> playerConfig.jumpVelocity
			>> playerConfig.maxSpeed
			>> playerConfig.gravity
			>> playerConfig.bulletAnimation
			)) {
			throw std::runtime_error("Level included faulty player config");
		}

		auto& player = entities.create({ Entity::Tag::Player });
		auto& cTransform = player->addComponent<CTransform>();
		cTransform.position = gridToPixel(vec2(gridX, gridY));
		auto& cAnimation = player->addComponent<CAnimation>();
		cAnimation.animation = assets.getAnimation("Stand");
		cAnimation.loop = true;
		auto& cBoundingBox = player->addComponent<CBoundingBox>();
		cBoundingBox.box = rect(bboxX, bboxY, bboxW, bboxH);
		player->addComponent<CInput>();
		player->addComponent<CPlayerState>();
	};
	parsers["Tile"] = [&](auto& instruction, auto& stream) {
		float gridX = 0, gridY = 0;
		std::string animation;
		if (!(stream
			>> gridX
			>> gridY
			>> animation
			)) {
			throw std::runtime_error("Level included faulty tile config");
		}
		levelSize.x = fmaxf(levelSize.x, gridX);
		levelSize.y = fmaxf(levelSize.y, gridY);

		auto& entity = entities.create({ Entity::Tag::World });
		auto& cTransform = entity->addComponent<CTransform>();
		cTransform.position = gridToPixel(vec2(gridX, gridY));
		auto& cAnimation = entity->addComponent<CAnimation>();
		cAnimation.animation = assets.getAnimation(animation);
		cAnimation.loop = true;
		auto& cBoundingBox = entity->addComponent<CBoundingBox>();
		cBoundingBox.box.position = cAnimation.animation.getSize() / -2;
		cBoundingBox.box.size = cAnimation.animation.getSize();
		if (animation == "Question") {
			entity->addComponent<CCoinBox>();
		}
	};
	parsers["Dec"] = [&](auto& instruction, auto& stream) {
		float gridX = 0, gridY = 0;
		std::string animation;
		if (!(stream
			>> gridX
			>> gridY
			>> animation
			)) {
			throw std::runtime_error("Level included faulty dec config");
		}
		levelSize.x = fmaxf(levelSize.x, gridX);
		levelSize.y = fmaxf(levelSize.y, gridY);

		auto& entity = entities.create({ Entity::Tag::World });
		auto& cTransform = entity->addComponent<CTransform>();
		cTransform.position = gridToPixel(vec2(gridX, gridY));
		auto& cAnimation = entity->addComponent<CAnimation>();
		cAnimation.animation = assets.getAnimation(animation);
		cAnimation.loop = true;
	};

	generic_parser(levelConfig.path, parsers);
}

void Scene_PlayLevel::sysEntities() {
	// TODO: Check if there are any entities outside the bounds of the level

	// TODO: Check if players have dropped of the screen

	// Update entity listings for the next frame
	entities.update();
}

void Scene_PlayLevel::sysGravity() {
	for (auto& player : entities.list(Entity::Tag::Player)) {
		auto& transform = player->getComponent<CTransform>();
		auto& input = player->getComponent<CInput>();
		if (!input.jump) {
			transform.velocity.y = playerConfig.gravity;
		}
	}
}

void Scene_PlayLevel::sysInput() {
	for (auto& player : entities.list(Entity::Tag::Player)) {
		auto& playerTrans = player->getComponent<CTransform>();
		auto& input = player->getComponent<CInput>();
		auto& state = player->getComponent<CPlayerState>();

		playerTrans.velocity.x = 0;
		if (input.left) {
			playerTrans.velocity.x -= playerConfig.movementSpeed;
		}
		if (input.right) {
			playerTrans.velocity.x += playerConfig.movementSpeed;
		}

		if (playerTrans.velocity.x > 0) {
			playerTrans.scale.x = 1.0f;
		} else if (playerTrans.velocity.x < 0) {
			playerTrans.scale.x = -1.0f;
		}

		if (input.jump) {
			playerTrans.velocity.y = playerConfig.jumpVelocity;
		}

		if (input.shoot) {
			input.shoot = false;
			onShootBullet(player);
		}
	}
}

void Scene_PlayLevel::sysMovement() {
	// Apply velocities
	for (auto& entity : entities.list()) {
		auto& transform = entity->getComponent<CTransform>();

		// Apply player speed limit
		if (entity->hasTag(Entity::Tag::Player)) {
			transform.velocity.clampLength(0, playerConfig.maxSpeed);
		}

		// Update position
		transform.position += transform.velocity;
		transform.angle += transform.spin;
	}
}

template<Entity::Tag t1, Entity::Tag t2>
bool collisionLogic(const EntityPtr& e1, const EntityPtr& e2, std::function<void(const EntityPtr& a, const EntityPtr& b)> callback) {
	if (e1->hasTag(t1) && e2->hasTag(t2)) {
		callback(e1, e2);
		return true;
	} else if (e1->hasTag(t2) && e2->hasTag(t1)) {
		callback(e2, e1);
		return true;
	} else {
		return false;
	}
}

rect entityWorldBox(const EntityPtr& entity) {
	auto& entityPos = entity->getComponent<CTransform>().position;
	auto entityBox = entity->getComponent<CBoundingBox>().box;
	entityBox.position += entityPos;
	return entityBox;
}

void Scene_PlayLevel::sysCollision() {
	// Bullet collissions
	for (auto& bullet : entities.list(Entity::Tag::Bullet)) {
		auto bulletBox = entityWorldBox(bullet);
		for (auto& tile : entities.list(Entity::Tag::World)) {
			auto tileBox = entityWorldBox(tile);
			auto overlap = bulletBox.overlap(tileBox);
			if (overlap.size.x > 0 && overlap.size.y > 0) {
				entities.remove(bullet);
				entities.remove(tile);
				break;
			}
		}
	}

	// Player collissions
	for (auto& player : entities.list(Entity::Tag::Player)) {
		auto& playerTrans = player->getComponent<CTransform>();
		auto previousBox = player->getComponent<CBoundingBox>().box;
		previousBox.position += player->getComponent<CTransform>().previousPosition;

		std::vector<EntityPtr> cornerCases;

		// First pass to find
		for (auto& tile : entities.list(Entity::Tag::World)) {
			if (
				!tile->alive() ||
				!tile->hasComponent<CTransform>() ||
				!tile->hasComponent<CBoundingBox>()
			) continue;
			auto playerBox = entityWorldBox(player);
			auto tileBox = entityWorldBox(tile);

			bool newLeft = previousBox.right() <= tileBox.left();
			bool newRight = previousBox.left() >= tileBox.right();
			bool newTop = previousBox.top() >= tileBox.bottom();
			bool newBottom = previousBox.bottom() <= tileBox.top();
			bool cornerCase = (newLeft || newRight) && (newTop || newBottom);

			auto overlap = playerBox.overlap(tileBox);
			if (overlap.size.x > 0 && overlap.size.y > 0) {
				if (cornerCase) {
					cornerCases.push_back(tile);
				} else {
					onCollision(player, tile, overlap);
				}
			}
		}

		// Second pass for corner cases, used in case the
		for (auto& tile : cornerCases) {
			auto playerBox = entityWorldBox(player);
			auto tileBox = entityWorldBox(tile);
			auto overlap = playerBox.overlap(tileBox);
			if (overlap.size.x > 0 && overlap.size.y > 0) {
				onCollision(player, tile, overlap);
			}
		}
	}
}

void Scene_PlayLevel::sysAnimation() {
	for (auto& entity : entities.list()) {
		if (entity->hasComponent<CAnimation>()) {
			auto& animation = entity->getComponent<CAnimation>();
			if (animation.animation.hasEnded()) {
				// Handle end of animation
				if (animation.loop) {
					// Looping animations automatically reset
					animation.animation.reset();
				} else {
					// Non-looping animations kill entity when finished
					entities.remove(entity);
				}
			} else {
				// Continue animation
				animation.animation.update();
			}
		}
	}
}

void Scene_PlayLevel::sysRender() {
	auto& window = game->getWindow();

	window.clear();

	if (drawTextures) {
		for (auto& entity : entities.list()) {
			if (entity->hasComponent<CAnimation>()) {
				auto& animation = entity->getComponent<CAnimation>();
				sf::Sprite sprite = animation.animation.getSprite();

				// Apply transform
				if (entity->hasComponent<CTransform>()) {
					auto& transform = entity->getComponent<CTransform>();
					sprite.setRotation(to_degrees(transform.angle));
					sprite.setScale(transform.scale);
					sprite.setPosition(transform.position);
				}

				window.draw(sprite);
			}
		}
	}
	if (drawBoxes) {
		sf::CircleShape pointShape(10.0f, 12);
		pointShape.setFillColor(sf::Color::Transparent);
		pointShape.setOutlineColor(sf::Color::Green);
		pointShape.setOutlineThickness(1);
		pointShape.setOrigin(pointShape.getRadius(), pointShape.getRadius());
		sf::RectangleShape boxShape;
		boxShape.setFillColor(sf::Color::Transparent);
		boxShape.setOutlineColor(sf::Color::Red);
		boxShape.setOutlineThickness(1);
		for (auto& entity : entities.list()) {
			if (entity->hasComponent<CTransform>()) {
				auto& pos = entity->getComponent<CTransform>().position;
				pointShape.setPosition(pos);
				window.draw(pointShape);
				if (entity->hasComponent<CBoundingBox>()) {
					auto& box = entity->getComponent<CBoundingBox>().box;
					boxShape.setPosition(pos + box.position);
					boxShape.setSize(box.size);
					window.draw(boxShape);
				}
			}
		}
	}
	if (drawGrid) {
		sf::RectangleShape shape;
		shape.setSize(tileSize);
		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineColor(sf::Color::Yellow);
		shape.setOutlineThickness(1);
		sf::Text label;
		label.setFont(game->getAssets().getFont("Arial"));
		label.setFillColor(sf::Color::Yellow);

		auto windowSize = window.getSize() + vec2::one();

		for (int x = 0; x < windowSize.x; x += tileSize.x) {
			for (int y = 0; y < windowSize.y; y += tileSize.y) {
				shape.setPosition(x, y);
				window.draw(shape);

				auto gridPos = pixelToGrid(vec2(x, y) + (tileSize / 2));

				if (gridPos.x <= levelSize.x && gridPos.y <= levelSize.y) {
					std::ostringstream stringStream;
					stringStream << "(" << gridPos.x << ", " << gridPos.y << ")";
					label.setString(stringStream.str());
					label.setPosition(x, y);
					window.draw(label);
				}
			}
		}
	}

	window.display();
}

void Scene_PlayLevel::sysPreviousPosition() {
	for (auto& entity : entities.list()) {
		if (entity->hasComponent<CTransform>()) {
			auto& transform = entity->getComponent<CTransform>();
			transform.previousPosition = transform.position;
		}
	}
}

void Scene_PlayLevel::onShootBullet(const EntityPtr& player) {
	auto& playerTrans = player->getComponent<CTransform>();
	auto& bullet = entities.create({ Entity::Tag::Bullet });
	auto& bulletTrans = bullet->addComponent<CTransform>();
	bulletTrans.position = playerTrans.position;
	bulletTrans.scale = playerTrans.scale;
	if (playerTrans.scale.x > 0) {
		bulletTrans.velocity.x = 1.0f;
	} else {
		bulletTrans.velocity.x = -1.0f;
	}
	bulletTrans.spin = to_radians(360.0 / 60.0) * bulletTrans.velocity.x;

	auto& bulletAnim = bullet->addComponent<CAnimation>();
	bulletAnim.loop = true;
	bulletAnim.animation = game->getAssets().getAnimation(playerConfig.bulletAnimation);

	auto& bulletBox = bullet->addComponent<CBoundingBox>();
	auto bulletSize = vec2(bulletAnim.animation.getSize());
	bulletBox.box = rect(bulletSize / -2.0f, bulletSize);
}

void Scene_PlayLevel::onCollision(const EntityPtr& player, const EntityPtr& tile, const rect& overlap) {
	// Get current position
	auto& playerTrans = player->getComponent<CTransform>();
	auto playerBox = player->getComponent<CBoundingBox>().box;
	playerBox.position += playerTrans.position;
	auto previousBox = player->getComponent<CBoundingBox>().box;
	previousBox.position += playerTrans.previousPosition;
	auto worldBox = tile->getComponent<CBoundingBox>().box;
	worldBox.position += tile->getComponent<CTransform>().position;

	if (previousBox.bottom() <= worldBox.top()) {
		// Player is landing onto the box
		playerTrans.position.y -= overlap.size.y;
		player->getComponent<CPlayerState>().canJump = true;
	} else if (previousBox.top() >= worldBox.bottom()) {
		// Player is head banging into the box
		playerTrans.position.y += overlap.size.y;
		if (tile->hasComponent<CCoinBox>()) {
			onCoinBoxHit(player, tile);
		}
	} else if (previousBox.right() <= worldBox.left()) {
		// Player walked right into a wall
		playerTrans.position.x -= overlap.size.x;
	} else if (previousBox.left() >= worldBox.right()) {
		// Player walked left into a wall
		playerTrans.position.x += overlap.size.x;
	} else {
		// No idea how to handle, teleport player on top of the box
		playerTrans.position.y -= (worldBox.top() - playerBox.bottom());
	}
}

void Scene_PlayLevel::onCoinBoxHit(const EntityPtr& player, const EntityPtr& tile) {
	auto& assets = game->getAssets();
	auto& tileTrans = tile->getComponent<CTransform>();
	auto& tileAnim = tile->getComponent<CAnimation>();
	tileAnim.animation = assets.getAnimation("Question2");

	// TODO: Show coin
	auto& coin = entities.create({ Entity::Tag::World });
	auto& coinAnim = coin->addComponent<CAnimation>();
	coinAnim.animation = assets.getAnimation("Coin");

	auto& coinTrans = coin->addComponent<CTransform>();
	auto tileHeight = tile->getComponent<CAnimation>().animation.getSize().y;
	auto coinHeight = coinAnim.animation.getSize().y;
	coinTrans.position = tileTrans.position;
	coinTrans.position.y -= (tileHeight / 2) + (coinHeight / 2);

	tile->removeComponent<CCoinBox>();
}
