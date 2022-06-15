#pragma once

#include "Common.h"
#include "Entity.h"
#include "EntityManager.h"


struct PlayerConfig
{
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
	float S;
};

struct EnemyConig
{
	int SR, CR, OR, OG, OB, OT, VMIN, VMAX,L,SI;
	float SMIN, SMAX;
};

struct BulletConfig
{
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V,L;
	float S;
};

class Game
{
	sf::RenderWindow	m_window;		// the window we draw to
	EntityManager		m_entities;		// vector of entities to maintain
	sf::Font			m_font;			
	sf::Text			m_text;
	PlayerConfig		m_playConfig;	
	EnemyConig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	int					m_score = 0;
	int					m_boxScore = 0;
	int					m_best = 0;
	int					m_lives = 3;
	int					m_currentFrame = 0;
	int					m_lastEnemySpawnTime = 0;
	int					m_specialWeapon = 3;
	int					m_bulletSpecialWeapon = 20;
	int					m_lastSpecialSpawnTime = 0;
	bool				m_paused = false;
	bool				m_running = true; 

	std::shared_ptr<Entity> m_player;
	std::shared_ptr<Entity> m_house;
	std::shared_ptr<Entity> m_box;

	void init(const std::string& config);	//initialize the game with config file path
	void setPaused(bool paused);

	void sMovement();		// move update
	void sUserInput();		// user input
	void sLifespan();		// lifespan
	void sRender();			// render
	void sEnemySpawner();	// spawns enemies
	void sCollision();		// collisions
	void sBoxSpawner();

	// some helper function below
	void spawnPlayer();
	void spawnBox();
	void spawnEnemy();
	void spawnHouse();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:
	Game(const std::string& config);

	void run();

};

