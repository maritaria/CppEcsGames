#include "entities.h"

const EntityPtr& Entities::get(EntityID id) {
	for (auto& entity : m_alive) {
		if (entity->id() == id) {
			return entity;
		}
	}
	for (auto& entity : m_babies) {
		if (entity->id() == id) {
			return entity;
		}
	}
	return nullptr;
}

const EntityList& Entities::list() {
	return m_alive;
}

const EntityList& Entities::list(Entity::Tag tag) {
	return m_tagTable[tag];
}

const EntityList Entities::list(std::initializer_list<Entity::Tag> tags) {
	if (tags.size() == 1) {
		return list(*tags.begin());
	}
	EntityList matches(list());
	auto matchEnd = matches.end();
	for (auto tag : tags) {
		// Remove upto the end of all matched entities so far
		// This means that previously rejected entities will not be looped over
		matchEnd = std::remove_if(matches.begin(), matchEnd, [tag](EntityPtr entity) {
			return entity->hasTag(tag);
		});
	}
	matches.erase(matchEnd, matches.end());
	return matches;
}

bool isEntityDead(EntityPtr entity) {
	return entity->dead();
}

void Entities::update() {
	// Remove dead entities
	const auto reaper = [](EntityList& list) {
		auto removed = std::remove_if(list.begin(), list.end(), isEntityDead);
		list.erase(removed, list.end());
	};

	reaper(m_alive);
	reaper(m_babies);
	for (auto& pair : m_tagTable) {
		reaper(pair.second);
	}

	// Promote babies
	for (auto& baby : m_babies) {
		if (!baby->dead()) {
			m_alive.push_back(baby);
			for (auto& tag : baby->m_tags) {
				m_tagTable[tag].push_back(baby);
			}
		}
	}
	m_babies.clear();
}

const EntityPtr Entities::create(std::initializer_list<Entity::Tag> tags) {

	EntityPtr entity(new Entity(m_counter++, tags));
	m_babies.push_back(entity);
	return entity;
}

void Entities::remove(const EntityPtr& entity) {
	if (entity) {
		entity->m_dead = true;
	}
}

void Entities::remove(EntityID id) {
	auto& entity = get(id);
	remove(entity);
}

void Entities::clear() {
	for (auto& entity : m_alive) {
		entity->m_dead = true;
	}
	m_babies.clear();
}
