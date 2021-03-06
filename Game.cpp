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

			m_window.create(sf::VideoMode(winW, winH), "First Game", sf::Style::Fullscreen);
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
	spawnHouse();
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
			sEnemySpawner();
			sBoxSpawner();
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

void Game::spawnHouse()
{
	auto house = m_entities.addEntity("house");

	house->cTransform = std::make_shared<CTransform>(Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2),
		Vec2(0, 0), 0.0f, 0);

	house->cShape = std::make_shared<CShape>(m_playConfig.SR*2, 4,
		sf::Color(m_playConfig.FR, m_playConfig.FG, m_playConfig.FB,0),
		sf::Color(m_playConfig.OR, m_playConfig.OG, m_playConfig.OB,128), m_playConfig.OT);

	house->cCollision = std::make_shared <CCollision>(m_playConfig.CR*2);

	m_house = house;


}

void Game::spawnBox()
{
	// random positon
	srand(time(NULL) + m_currentFrame);
	int xPosition = rand() % m_window.getSize().x;
	int yPosition = rand() % m_window.getSize().y;

	float rangle = rand() % 360;
	const float pi = 3.1415926;
	float xVelocity = m_playConfig.S * cos(rangle * pi / 180);
	float yVelocity = m_playConfig.S * sin(rangle * pi / 180);

	int index = rand() % 2;
	std::string names[] = {"W","L" };
	std::string boxName = names[index];

	auto Box = m_entities.addEntity(boxName);

	Box->cTransform = std::make_shared<CTransform>(Vec2(xPosition, yPosition),
		Vec2(xVelocity, yVelocity), 0.0f, m_playConfig.S);

	Box->cShape = std::make_shared<CShape>(m_playConfig.SR , index+3,
		sf::Color(m_playConfig.FR, m_playConfig.FG, m_playConfig.FB),
		sf::Color(m_playConfig.OR, m_playConfig.OG, m_playConfig.OB), m_playConfig.OT);

	Box->cCollision = std::make_shared <CCollision>(m_playConfig.CR );

	m_box = Box;


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

		enemy->cScore = std::make_shared <CScore>(50*enemyPoint);

		enemy->cBlood = std::make_shared <CBlood>(enemyPoint-1);
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

		enemy->cBlood = std::make_shared <CBlood>(1);

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

		smallenemy->cScore = std::make_shared <CScore>(100);

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
		bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
	}


	


}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	
	if ((entity->isActive()) && (m_bulletSpecialWeapon >0))
	{
		float angleDelta = 360.0 / 8;
		float angle = 0;
		const float pi = 3.1415926;

		for (size_t i = 0; i < 8; i++) {
			angle += angleDelta;
			float xVelocity = m_bulletConfig.S * cos(angle * pi / 180);
			float yVelocity = m_bulletConfig.S * sin(angle * pi / 180);

			auto specialbullet = m_entities.addEntity("bullets");

			specialbullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos,
				Vec2(xVelocity, yVelocity), 0.0f, m_bulletConfig.S);

			specialbullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
				sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
				sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

			specialbullet->cCollision = std::make_shared <CCollision>(m_bulletConfig.CR);
		}
		m_bulletSpecialWeapon--;
		if (!m_bulletSpecialWeapon)
		{
			m_lastSpecialSpawnTime = m_currentFrame;
			m_specialWeapon--;
			m_bulletSpecialWeapon = 20;
		}

	}

}



void Game::sMovement()
{
	// implement all entity movement 
	// read m_play->cinput to determine if player is moving
	float speedup = 0;
	if (m_player->cInput->speedup)
	{
		speedup = 2;
	}
	else speedup = 1;

	Vec2 playervelocity;
	float playSpeed = speedup * m_player->cTransform->speed;
	const float pi = 3.1415926;
	if (m_player->cInput->left)
	{
		playervelocity.x -= playSpeed;
	}
	if (m_player->cInput->right)
	{
		playervelocity.x += playSpeed;
	}
	if (m_player->cInput->up)
	{
		playervelocity.y -= playSpeed;
	}
	if (m_player->cInput->down)
	{
		playervelocity.y += playSpeed;
	}
	if ((m_player->cInput->left) && (m_player->cInput->up))
	{
		playervelocity.x = -playSpeed * cos(45 * pi / 180);
		playervelocity.y = -playSpeed * cos(45 * pi / 180);
	}
	if ((m_player->cInput->left) && (m_player->cInput->down))
	{
		playervelocity.x = -playSpeed * cos(45 * pi / 180);
		playervelocity.y = +playSpeed * cos(45 * pi / 180);
	}
	if ((m_player->cInput->right) && (m_player->cInput->up))
	{
		playervelocity.x = +playSpeed * cos(45 * pi / 180);
		playervelocity.y = -playSpeed * cos(45 * pi / 180);
	}
	if ((m_player->cInput->right) && (m_player->cInput->down))
	{
		playervelocity.x = +playSpeed * cos(45 * pi / 180);
		playervelocity.y = +playSpeed * cos(45 * pi / 180);
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
				int newAlpha = 255* e->cLifespan->remaining/ e->cLifespan->total;
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
				if (e->cBlood->blood>2)
				{
					e->cBlood->blood--;
					e->cShape->circle.setPointCount(e->cBlood->blood+1);
					b->destroy();
				}
				else
				{
					spawnSmallEnemies(e);
					m_score += e->cScore->score;
					b->destroy();
					e->destroy();
				}

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

	for (auto& b : m_entities.getEntities("bullets"))
	{
		
	if (m_house->cTransform->pos.dist(b->cTransform->pos)
		< m_house->cCollision->radius + b->cCollision->radius)
		{
			b->destroy();
		}

	}


	//if (m_box->cTransform->pos.dist(m_player->cTransform->pos)
	//	< m_box->cCollision->radius + m_player->cCollision->radius)
	//{
	//	m_box->destroy();
	//	m_lives++;
	//}


	for (auto& e : m_entities.getEntities())
		{
			if ((e->tag() == "enemys") || (e->tag() == "smallenemys"))
			{
				if (m_house->cTransform->pos.dist(e->cTransform->pos)
					< m_house->cCollision->radius + e->cCollision->radius)
				{
					if (e->tag() == "enemys")
					{
						spawnSmallEnemies(e);
						e->destroy();
					}
					else if (e->tag() == "smallenemys") e->destroy();
				}

				if (m_player->cTransform->pos.dist(e->cTransform->pos)
					< m_player->cCollision->radius + e->cCollision->radius)
				{
					if (e->tag() == "enemys") spawnSmallEnemies(e);
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
						m_specialWeapon = 3;
						m_boxScore = 0;
						spawnPlayer();
						spawnHouse();
					}

				}
			}

			if (e->tag() == "L" || e->tag() == "W")
			{
				if (e->cTransform->pos.dist(m_player->cTransform->pos)
					< e->cCollision->radius + m_player->cCollision->radius)
				{
					e->destroy();
					if (e->tag() == "L") m_lives++;
					if (e->tag() == "W") m_specialWeapon++;
				}
				if ((e->cTransform->pos.x > m_window.getSize().x) || (e->cTransform->pos.x < 0))
				{
					e->cTransform->velocity.x = -e->cTransform->velocity.x;
				}
				if ((e->cTransform->pos.y > m_window.getSize().y) || (e->cTransform->pos.y < 0))
				{
					e->cTransform->velocity.y = -e->cTransform->velocity.y;
				}
			}
			
		}

		

	for (std::shared_ptr<Entity> e : m_entities.getEntities("enemys"))
		{
			if ((e->cTransform->pos.x > m_window.getSize().x) || (e->cTransform->pos.x < 0))
			{
				e->cTransform->velocity.x = -e->cTransform->velocity.x;
			}
			if ((e->cTransform->pos.y > m_window.getSize().y) || (e->cTransform->pos.y < 0))
			{
				e->cTransform->velocity.y = -e->cTransform->velocity.y;
			}
		}

	if (m_player->cInput->speedup)
	{
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
	else
	{
		if (m_player->cTransform->pos.x > m_window.getSize().x - m_player->cCollision->radius)
		{
			m_player->cTransform->pos.x = m_window.getSize().x - m_player->cCollision->radius;
		}
		if (m_player->cTransform->pos.x < m_player->cCollision->radius)
		{
			m_player->cTransform->pos.x = m_player->cCollision->radius;
		}
		if (m_player->cTransform->pos.y > m_window.getSize().y - m_player->cCollision->radius)
		{
			m_player->cTransform->pos.y = m_window.getSize().y - m_player->cCollision->radius;
		}
		if (m_player->cTransform->pos.y < m_player->cCollision->radius)
		{
			m_player->cTransform->pos.y = m_player->cCollision->radius;
		}
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

void Game::sBoxSpawner()
{

	if (m_score - m_boxScore > 10000)
	{
		spawnBox();
		m_boxScore += 10000;
	}
}


void Game::sRender()
{

	m_window.clear();

	// Update the  text
	if (m_best < m_score) m_best = m_score;
	std::stringstream ss;
	ss << "Score: " << m_score << "    Lives: " << m_lives 
		<< "     Weapon: "<< m_specialWeapon
		<< "    Best Score: " << m_best;
	m_text.setString(ss.str());


	m_window.draw(m_text);

	if (m_player->isActive())
	{
		m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

		m_player->cTransform->angle += 1.0f;
		m_player->cShape->circle.setRotation(m_player->cTransform->angle);

		m_window.draw(m_player->cShape->circle);

	}


	
	for (std::shared_ptr<Entity> e : m_entities.getEntities())
	{
		if ((e->tag() == "enemys") || (e->tag() == "smallenemys") || (e->tag() == "L")|| (e->tag() == "W"))
		{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		m_window.draw(e->cShape->circle);
		}

		if (e->tag() == "bullets")
		{
			e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
			m_window.draw(e->cShape->circle);
		}
		if (e->tag() == "house")
		{
			e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
			m_window.draw(e->cShape->circle);
		}

	}


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
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::Space:
				m_player->cInput->speedup = true;
				break;
			case sf::Keyboard::P:
				setPaused(!m_paused);
				break;
			case sf::Keyboard::Escape:
				m_running = false;
				break;
			}

		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			case sf::Keyboard::Space:
				m_player->cInput->speedup = false;
				break;
			}

		}


		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//std::cout << "left mouse clicked at " << event.mouseButton.x << "," << event.mouseButton.y << std::endl;
				if ((!m_paused) && 
					(m_player->cTransform->pos.dist(m_house->cTransform->pos) >
						m_player->cCollision->radius + m_house->cCollision->radius))
				{
					spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
				}

			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				if ((!m_paused) &&
					(m_player->cTransform->pos.dist(m_house->cTransform->pos) >
						m_player->cCollision->radius + m_house->cCollision->radius))
				{
					if ((m_currentFrame - m_lastSpecialSpawnTime > 600) && (m_specialWeapon > 0))
						spawnSpecialWeapon(m_player);
				}

			}
		}
	}
}