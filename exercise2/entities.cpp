#include "entities.h"

bool isEntityMarkedDead(const std::shared_ptr<Entity>& entity)
{
	return entity->dead;
}

void removeDeadEntities(std::vector<std::shared_ptr<Entity>>& list)
{
	auto it = std::remove_if(list.begin(), list.end(), isEntityMarkedDead);
	list.erase(it, list.end());
}

void EntityManager::update()
{
	// Clean up dead entities
	removeDeadEntities(m_entities);
	// TODO: Check that smart pointer gets cleaned up correctly
	for (auto& pair : m_tagTable) {
		removeDeadEntities(pair.second);
	}
	
	// Mature babies into population
	for (auto entity : m_babies) {
		m_entities.push_back(entity);
		m_tagTable[entity->tag].push_back(entity);
	}
	m_babies.clear();
}

std::shared_ptr<Entity> EntityManager::create(Entity::Tag tag)
{
	auto entity = std::make_shared<Entity>(m_idCounter++, tag);
	m_babies.push_back(entity);
	return entity;
}

void EntityManager::remove(EntityId idEntity)
{
	auto entity = getById(idEntity);
	remove(entity);
}

void EntityManager::remove(std::shared_ptr<Entity>& entity)
{
	if (entity) {
		entity->dead = true;
	}
}

size_t EntityManager::size()
{
	return m_entities.size();
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::getAll()
{
	return m_entities;
}

std::shared_ptr<Entity> EntityManager::getById(EntityId idEntity)
{
	for (auto entity : getAll()) {
		if (entity->id == idEntity) {
			return entity;
		}
	}
	return nullptr;
}

const std::vector<std::shared_ptr<Entity>> EntityManager::getByTag(Entity::Tag tag)
{
	return m_tagTable[tag];
}
