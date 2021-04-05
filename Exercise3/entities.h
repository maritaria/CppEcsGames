#pragma once

#include <map>
#include <vector>
#include <tuple>
#include <initializer_list>
#include <unordered_set>
#include "components.h"

typedef size_t EntityID;

struct Entity
{
	enum class Tag
	{
		World,
		Player,
		Enemy,
		Bullet,
	};

	EntityID id() {
		return m_id;
	}
	bool dead() {
		return m_dead;
	}
	bool alive() {
		return !dead();
	}
	bool hasTag(Tag tag) {
		return m_tags.find(tag) != m_tags.end();
	}

	template<typename T>
	bool hasComponent() {
		return std::get<T>(m_data).active;
	}

	template<typename T>
	T& getComponent() {
		return std::get<T>(m_data);
	}

	template<typename T>
	T& addComponent() {
		auto& component = std::get<T>(m_data);
		component = T();
		component.active = true;
		return component;
	}

	template<typename T>
	void removeComponent() {
		getComponent<T>().active = false;
	}

private:
	EntityID m_id;
	bool m_dead = false;
	std::unordered_set<Tag> m_tags;
	ComponentTuple m_data;

	Entity(EntityID id, std::initializer_list<Tag> tags)
		: m_id(id)
		, m_tags(tags) {
	}

	friend class Entities;
};

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::vector<EntityPtr> EntityList;

class Entities
{
public:
	const EntityPtr& get(EntityID id);
	const EntityList& list();
	const EntityList& list(Entity::Tag tag);
	const EntityList list(std::initializer_list<Entity::Tag> tags);

	const EntityPtr create(std::initializer_list<Entity::Tag> tags);
	void remove(const EntityPtr& entity);
	void remove(EntityID id);
	void clear();

	void update();

private:
	size_t m_counter = 1;
	EntityList m_alive;
	EntityList m_babies;
	std::map<Entity::Tag, EntityList> m_tagTable;
};
