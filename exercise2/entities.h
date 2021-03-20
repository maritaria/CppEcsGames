#pragma once

#include <vector>
#include <map>
#include <memory>
#include <string>

#include "components.h"

typedef unsigned int EntityId;

struct Entity
{
	enum Tag
	{
		Unknown,
		Player,
		Bullet,
		Enemy,
		EnemyChild
	};

	EntityId id;
	bool dead = false;
	Tag tag = Unknown;

	// Components
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CLifespan> cLifespan;
	std::shared_ptr<CInput> cInput;

	Entity(EntityId idEntity, Tag tag)
		: id(idEntity)
		, tag(tag)
	{}
};

class EntityManager
{
	// Note: entity id 0 is invalid
	EntityId m_idCounter = 1;
	std::vector<std::shared_ptr<Entity>> m_entities;
	std::map<Entity::Tag, std::vector<std::shared_ptr<Entity>>> m_tagTable;
	std::vector<std::shared_ptr<Entity>> m_babies;

public:
	void update();

	std::shared_ptr<Entity> create(Entity::Tag tag);
	void remove(EntityId idEntity);
	void remove(std::shared_ptr<Entity>& entity);
	size_t size();

	const std::vector<std::shared_ptr<Entity>>& getAll();
	std::shared_ptr<Entity> getById(EntityId idEntity);
	const std::vector<std::shared_ptr<Entity>> getByTag(Entity::Tag tag);
};
