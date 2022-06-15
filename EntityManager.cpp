#include "EntityManager.h"

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	// - add entities from m_entitiestoAdd the proper locations
	// - add them to the vector of all entities
	// - add them to the vecor inside the map, with the tag as a key 
	//for (std::shared_ptr<Entity> e : m_entitiesToAdd)
	for (std::shared_ptr<Entity> e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_entitiesToAdd.clear();

	// remove dead entities 
	removeDeadEntities(m_entities);

	// remove dead entities from map. it's c++17 way 
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}

}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// remove dead entity from vector
	for (auto it = vec.begin();it!=vec.end(); )
	{
		if ((*it)->isActive() == false)
		{
			it = vec.erase(it);
		}
		else it++;
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	// create a new entity object
	// store it in the vector of all entities 
	// and store it in the map of tag->entityvector
	// return the shared pointer pointer to that entity

	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	
	m_entitiesToAdd.push_back(entity);


	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	//return vector from the map
	return m_entityMap[tag];
}