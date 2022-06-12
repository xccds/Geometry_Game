#include "Game.h"

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	// read input config file

	std::ifstream inf{path};

	// If we couldn't open the output file stream for reading
	if (!inf)
	{
		// Print an error and exit
		std::cerr << "Uh oh, file not be opened for reading!\n";
	}

	// While there's still stuff left to read
	while (inf)
	{
		std::string temp_name;
		inf >> temp_name;
		if (temp_name == "Window") 
		{
			int winW, winH;
			int FL; // frame limit set to windows
			int FS; // fullscreen or not
			inf >> winW >> winH >> FL >> FS;

			m_window.create(sf::VideoMode(winW, winH), "First Game");
			m_window.setFramerateLimit(FL);
		}
		else if (temp_name == "Font")
		{
			std::string fontPath;
			int fontSize;
			int fontCR;
			int fontCG;
			int fontCB;
			inf >> fontPath >> fontSize >> fontCR >> fontCG >> fontCB;
			m_font.loadFromFile(fontPath);
			m_text.setFont(m_font);
			m_text.setCharacterSize(fontSize);
			m_text.setFillColor(sf::Color(fontCR, fontCG, fontCB));

		}
		else if (temp_name == "Player") 
		{
			inf >> m_playConfig.SR >> m_playConfig.CR >> m_playConfig.S >> m_playConfig.FR
				>> m_playConfig.FG >> m_playConfig.FB >> m_playConfig.OR >> m_playConfig.OG
				>> m_playConfig.OB >> m_playConfig.OT >> m_playConfig.V;
		}
		else if (temp_name == "Enemy")
		{
			inf >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
				>> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT
				>> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}
		else if (temp_name == "Bullet")
		{
			inf >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR
				>> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG
				>> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}

	
	m_text.setPosition(1, 1);

	spawnPlayer();
}

void Game::run()
{
	//todo : add pause function here 
	// some system shouldn't function while paused (move/ input)
	// some system still work while paused (rendering)
	while (m_running)
	{

		if (!m_paused)
		{
			m_entities.update();
			//spawnSpecialWeapon();
			sEnemySpawner();
			sMovement();
			sLifespan();
			sCollision();
			sUserInput();
			m_currentFrame++;
		}
		sUserInput();
		sRender();
	}

}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::spawnPlayer()
{
	//create player and adding properties with the config 

	auto entity = m_entities.addEntity("player");

	entity->cTransform = std::make_shared<CTransform>(Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2),
		Vec2(m_playConfig.S, m_playConfig.S), 0.0f, m_playConfig.S);

	entity->cShape = std::make_shared<CShape>(m_playConfig.SR, m_playConfig.V, 
		sf::Color(m_playConfig.FR, m_playConfig.FG, m_playConfig.FB), 
		sf::Color(m_playConfig.OR, m_playConfig.OG, m_playConfig.OB), m_playConfig.OT);

	entity->cInput = std::make_shared<CInput>();

	entity->cCollision = std::make_shared <CCollision>(m_playConfig.CR);

	m_player = entity;
}

void Game::spawnEnemy()
{
	// random positon
	srand(time(NULL)+ m_currentFrame);
    int xPosition = rand()% m_window.getSize().x ;
	int yPosition = rand()% m_window.getSize().y ;

	// random speed
	int speedDiff = 1+ m_enemyConfig.SMAX - m_enemyConfig.SMIN;
	int enemySpeed = rand() % speedDiff + m_enemyConfig.SMIN;

	// random point
	int pointDiff = 1+ m_enemyConfig.VMAX - m_enemyConfig.VMIN;
	int enemyPoint = rand() % pointDiff + m_enemyConfig.VMIN;

	// random color 
	int CR = rand() % 250;
	int CG = rand() % 250;
	int CB = rand() % 250;

	// half smart enemy
	bool isSmart = rand() % 2;
	if (isSmart)
	{
		Vec2 d = Vec2(m_player->cTransform->pos.x - xPosition, m_player->cTransform->pos.y - yPosition);
		float dist = Vec2(xPosition, yPosition).dist(m_player->cTransform->pos);
		Vec2 normal = d / dist;
		float xVelocity = enemySpeed * normal.x;
		float yVelocity = enemySpeed * normal.y;

		auto enemy = m_entities.addEntity("enemys");

		enemy->isSmart = true;

		enemy->cTransform = std::make_shared<CTransform>(Vec2(xPosition, yPosition),
			Vec2(xVelocity, yVelocity), 0.0f, enemySpeed);

		enemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR, enemyPoint,
			sf::Color(CR, CG, CB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

		enemy->cCollision = std::make_shared <CCollision>(m_enemyConfig.CR);

		enemy->cScore = std::make_shared <CScore>(100);
	}
	else
	{
		// random move
		float rangle = rand() % 360;
		const float pi = 3.1415926;
		float xVelocity = enemySpeed * cos(rangle * pi / 180);
		float yVelocity = enemySpeed * sin(rangle * pi / 180);

		auto enemy = m_entities.addEntity("enemys");

		enemy->cTransform = std::make_shared<CTransform>(Vec2(xPosition, yPosition),
			Vec2(xVelocity, yVelocity), 0.0f, enemySpeed);

		enemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR, enemyPoint,
			sf::Color(CR, CG, CB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

		enemy->cCollision = std::make_shared <CCollision>(m_enemyConfig.CR);

		enemy->cScore = std::make_shared <CScore>(100);

	}


	m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	//when create smaller enemy, we read the value of the original enemy
	// spaw a number of small enmies equal to the vertices of the original enenmy
	//  set each small enemy to the same color as original, half the size
	// small enemies are double points of the original 
	size_t smallNum = e->cShape->circle.getPointCount();
	int xPosition = e->cShape->circle.getPosition().x;
	int yPosition = e->cShape->circle.getPosition().y;
	float angleDelta = 360.0 / smallNum;
	float angle = 0;
	const float pi = 3.1415926;


	for (size_t i = 0; i < smallNum; i++) {
		angle += angleDelta;
		float xVelocity = e->cTransform->speed * cos(angle * pi / 180);
		float yVelocity = e->cTransform->speed * sin(angle * pi / 180);
		auto smallenemy = m_entities.addEntity("smallenemys");

		smallenemy->cTransform = std::make_shared<CTransform>(Vec2(xPosition, yPosition),
			Vec2(xVelocity, yVelocity), 0.0f,e->cTransform->speed);

		smallenemy->cShape = std::make_shared<CShape>(m_enemyConfig.SR/2, smallNum,
			e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(), m_enemyConfig.OT);

		smallenemy->cCollision = std::make_shared <CCollision>(m_enemyConfig.CR);

		smallenemy->cScore = std::make_shared <CScore>(e->cScore->score*2);

		smallenemy->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
		//m_lastEnemySpawnTime = m_currentFrame;
	}

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	if (entity->isActive())
	{
		Vec2 d = Vec2(target.x - entity->cTransform->pos.x, target.y - entity->cTransform->pos.y);
		float dist = entity->cTransform->pos.dist(target);
		Vec2 normal = d / dist;
		float xVelocity = m_bulletConfig.S * normal.x;
		float yVelocity = m_bulletConfig.S * normal.y;

		auto bullet = m_entities.addEntity("bullets");

		bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos,
			Vec2(xVelocity, yVelocity), 0.0f, m_bulletConfig.S);

		bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
			sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
			sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

		bullet->cCollision = std::make_shared <CCollision>(m_bulletConfig.CR);
	}


	//bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);


}

//void Game::spawnSpecialWeapon()
//{
//	
//		auto weapon = m_entities.addEntity("weapon");
//
//		auto xPosition = entity->cTransform->pos.x;
//		auto yPosition = entity->cTransform->pos.y;
//
//
//		weapon->cTransform = std::make_shared<CTransform>(Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2),
//			Vec2(m_playConfig.S, m_playConfig.S), 0.0f, m_playConfig.S);
//
//		weapon->cShape = std::make_shared<CShape>(m_playConfig.SR*2, m_playConfig.V,
//			sf::Color(m_playConfig.FR, m_playConfig.FG, m_playConfig.FB,0),
//			sf::Color(m_playConfig.OR, m_playConfig.OG, m_playConfig.OB,70), m_playConfig.OT);
//
//		weapon->cInput = std::make_shared<CInput>();
//
//		weapon->cCollision = std::make_shared <CCollision>(m_playConfig.CR*2);
//
//
//		m_weapon = weapon;
//
//}



void Game::sMovement()
{
	// implement all entity movement 
	// read m_play->cinput to determine if player is moving
	Vec2 playervelocity;
	if (m_player->cInput->left)
	{
		playervelocity.x -= m_playConfig.S;
	}
	if (m_player->cInput->right)
	{
		playervelocity.x += m_playConfig.S;
	}
	if (m_player->cInput->up)
	{
		playervelocity.y -= m_playConfig.S;
	}
	if (m_player->cInput->down)
	{
		playervelocity.y += m_playConfig.S;
	}

	m_player->cTransform->velocity = playervelocity;

	for (std::shared_ptr<Entity> e : m_entities.getEntities())
	{
		e->cTransform->pos += e->cTransform->velocity;
	}

	// update smart enemy
	for (std::shared_ptr<Entity> e : m_entities.getEntities("enemys"))
	{
		if (e->isSmart)
		{
			Vec2 d = m_player->cTransform->pos - e->cTransform->pos;
			float dist = e->cTransform->pos.dist(m_player->cTransform->pos);
			Vec2 normal = d / dist;
			e->cTransform->velocity = normal * e->cTransform->speed;
		}

	}



}

void Game::sLifespan()
{
	//implement all lifespan 
	// if entities has lifespan and is alive
	// scale its alpha properly 
	// if its time is up 
	// destroy the entity
	for (std::shared_ptr<Entity> e : m_entities.getEntities())
	{
		if ((e->cLifespan) && (e->isActive()))
		{
			if (e->cLifespan->remaining)
			{
				e->cLifespan->remaining--;

				sf::Color cf = e->cShape->circle.getFillColor();
				sf::Color co = e->cShape->circle.getOutlineColor();
				int newAlpha = 200* e->cLifespan->remaining/ e->cLifespan->total;
				e->cShape->circle.setFillColor(sf::Color(cf.r, cf.g, cf.b, newAlpha));
				//e->cShape->circle.setOutlineColor(sf::Color(cf.r, cf.g, cf.b, newAlpha));
			}
			else e->destroy();
		}
	}

}

void Game::sCollision()
{
	for (auto& b : m_entities.getEntities("bullets"))
	{
		for (auto& e : m_entities.getEntities("enemys"))
		{
			if (b->cTransform->pos.dist(e->cTransform->pos) 
				< b->cCollision->radius + e->cCollision->radius)
			{
				spawnSmallEnemies(e);
				m_score += e->cScore->score;
				b->destroy();
				e->destroy();
			}
		}
	}

	for (auto& b : m_entities.getEntities("bullets"))
	{
		for (auto& e : m_entities.getEntities("smallenemys"))
		{
			if (b->cTransform->pos.dist(e->cTransform->pos)
				< b->cCollision->radius + e->cCollision->radius)
			{
				m_score += e->cScore->score;
				b->destroy();
				e->destroy();

			}
		}
	}

	//for (auto& e : m_entities.getEntities("enemys"))
	//{
	//	if (m_weapon->cTransform->pos.dist(e->cTransform->pos)
	//		< m_weapon->cCollision->radius + e->cCollision->radius)
	//	{
	//		m_score += e->cScore->score;
	//		spawnSmallEnemies(e);
	//		e->destroy();
	//	}
	//}


		for (auto& e : m_entities.getEntities("enemys"))
		{
			if (m_player->cTransform->pos.dist(e->cTransform->pos)
				< m_player->cCollision->radius + e->cCollision->radius)
			{
				spawnSmallEnemies(e);
				e->destroy();
				if (m_lives)
				{
					m_lives--;
					spawnPlayer();
						
				}
				else
				{
					for (std::shared_ptr<Entity> e : m_entities.getEntities())
					{
						e->destroy();
					}
					m_score = 0;
					m_lives = 3;
					m_shields = 3;
					spawnPlayer();
				}

			}
		}

		for (auto& e : m_entities.getEntities("smallenemys"))
		{
			if (m_player->cTransform->pos.dist(e->cTransform->pos)
				< m_player->cCollision->radius + e->cCollision->radius)
			{
				e->destroy();
				if (m_lives)
				{
					m_lives--;
					spawnPlayer();

				}
				else
				{
					for (std::shared_ptr<Entity> e : m_entities.getEntities())
					{
						e->destroy();
					}
					m_score = 0;
					m_lives = 3;
					m_shields = 3;
					spawnPlayer();
				}

			}
		}

	for (std::shared_ptr<Entity> e : m_entities.getEntities("enemys"))
	{
		if ((e->cTransform->pos.x > m_window.getSize().x) || (e->cTransform->pos.x < 0))
		{
			e->cTransform->velocity.x = - e->cTransform->velocity.x;
		}
		if ((e->cTransform->pos.y > m_window.getSize().y) || (e->cTransform->pos.y < 0))
		{
			e->cTransform->velocity.y = - e->cTransform->velocity.y;
		}
	}


	if (m_player->cTransform->pos.x > m_window.getSize().x)
	{
		m_player->cTransform->pos.x = 0;
	}
	if (m_player->cTransform->pos.x < 0)
	{
		m_player->cTransform->pos.x = m_window.getSize().x;
	}
	if (m_player->cTransform->pos.y > m_window.getSize().y)
	{
		m_player->cTransform->pos.y = 0;
	}
	if (m_player->cTransform->pos.y < 0)
	{
		m_player->cTransform->pos.y = m_window.getSize().y;
	}

}

void Game::sEnemySpawner()
{
	// implement enemy spawning should go here
	// use m_currentframe - m_lastenemyspawntime to determine how long 
	if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
	{
		spawnEnemy();
	}
}

void Game::sRender()
{

	m_window.clear();

	// Update the  text
	if (m_best < m_score) m_best = m_score;
	std::stringstream ss;
	ss << "Score: " << m_score << "    Lives: " << m_lives << "    Best Score: " << m_best;
	m_text.setString(ss.str());

	m_window.draw(m_text);

	if (m_player->isActive())
	{
		m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

		m_player->cTransform->angle += 1.0f;
		m_player->cShape->circle.setRotation(m_player->cTransform->angle);

		m_window.draw(m_player->cShape->circle);
	}



	for (std::shared_ptr<Entity> e : m_entities.getEntities("enemys"))
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		m_window.draw(e->cShape->circle);
	}

	for (std::shared_ptr<Entity> e : m_entities.getEntities("smallenemys"))
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		m_window.draw(e->cShape->circle);
	}

	for (std::shared_ptr<Entity> e : m_entities.getEntities("bullets"))
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
		m_window.draw(e->cShape->circle);
	}

	//if (m_weapon->isActive())
	//{
	//	m_weapon->cShape->circle.setPosition(m_weapon->cTransform->pos.x, m_weapon->cTransform->pos.y);

	//	m_weapon->cTransform->angle += 1.0f;
	//	m_weapon->cShape->circle.setRotation(m_weapon->cTransform->angle);

	//	m_window.draw(m_weapon->cShape->circle);
	//}

	//for (std::shared_ptr<Entity> e : m_entities.getEntities("weapon"))
	//{
	//	e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

	//	e->cTransform->angle += 1.0f;
	//	e->cShape->circle.setRotation(e->cTransform->angle);
	
	//	m_window.draw(e->cShape->circle);
	//}

	m_window.display();
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{

		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}


		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				//std::cout << "W pressed" << std::endl;
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				//std::cout << "S pressed" << std::endl;
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				//std::cout << "A pressed" << std::endl;
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				//std::cout << "D pressed" << std::endl;
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::Escape:
				//std::cout << "ESCAP " << std::endl;
				setPaused(!m_paused);
			}

		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				//std::cout << "W released" << std::endl;
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				//std::cout << "S released" << std::endl;
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				//std::cout << "A released" << std::endl;
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				//std::cout << "D released" << std::endl;
				m_player->cInput->right = false;
				break;
			}

		}


		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//std::cout << "left mouse clicked at " << event.mouseButton.x << "," << event.mouseButton.y << std::endl;
				if (!m_paused)
				{
					spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
				}

			}

			//if (event.mouseButton.button == sf::Mouse::Right)
			//{
			//	spawnSpecialWeapon(m_player);
			//}
		}
	}
}