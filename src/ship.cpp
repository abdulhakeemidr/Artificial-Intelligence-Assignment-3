#include "ship.h"
#include "Game.h"
#include "Util.h"
#include "PlayScene.h"
#include "glm/gtx/string_cast.hpp"


Ship::Ship() :
	m_maxSpeed(5.0f), m_currentDirection(0.0f), m_turnSpeed(2.0f), m_steerForce(0.1f)
{
	TheTextureManager::Instance()->load("../Assets/textures/warrior.png",
		"warrior", TheGame::Instance()->getRenderer());

	auto size = TheTextureManager::Instance()->getTextureSize("warrior");
	setWidth(size.x / 5);
	setHeight(size.y / 12);
	setPosition(glm::vec2(400.0f, 300.0f));
	setVelocity(glm::vec2(0.0f, 0.0f));
	setAcceleration(glm::vec2(0.0f, 0.0f));
	setIsColliding(false);
	setType(SHIP);
	setState(IDLE);
	setScale(glm::vec2(0, 0));
}


Ship::~Ship()
= default;

void Ship::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	TheTextureManager::Instance()->drawWarrior("warrior", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), m_currentDirection, 255, true, SDL_FLIP_NONE, direction, g_sprite, m_scale.x, m_scale.y);
}

void Ship::m_checkState()
{
	switch (getState())
	{
	case IDLE:
		break;
	case SEEK:
		//seek state loops back into checkArrival
		move();
		m_checkArrival();
		m_checkBounds();
		break;
	case ARRIVE:
		break;
	case AVOID:
		break;
	case FLEE:
		break;
	default:
		std::cout << "unknown or unused case" << std::endl;
		break;
	}
}

void Ship::update()
{
	m_checkState();
}

void Ship::clean()
{
}

void Ship::turnRight()
{
	m_currentDirection += m_turnSpeed;
	if (m_currentDirection >= 360) 
	{
		m_currentDirection = 0;
	}

}

void Ship::turnLeft()
{
	m_currentDirection -= m_turnSpeed;
	if (m_currentDirection < 0)
	{
		m_currentDirection = 359.0;
	}
}

void Ship::move()
{
	if (Util::distance(getPosition(), m_target) > 1.0f) {
		const glm::vec2 desired = Util::normalize(m_target - getPosition()) * m_maxSpeed;
		setVelocity(desired);


		const glm::vec2 newPosition = getPosition() + getVelocity();
		setPosition(newPosition);
		animate();
	}
}

//Added arrive function, moved some tile getters/setters
void Ship::m_checkArrival()
{
	const auto size = Config::TILE_SIZE;
	const auto offset = size * 0.5f;
	if (Util::distance(glm::vec2(getPosition().x - offset, getPosition().y - offset), glm::vec2(m_target.x - offset, m_target.y - offset)) <= 2.5f)
	{
		setTile(nextTile);
		nextTile = nullptr;
		setState(IDLE);
		setTarget();
	}
}

glm::vec2 Ship::getTarget()
{
	return nextTile->getPosition();
}


//SetTarget now loops into seek state
void Ship::setTarget()
{
	m_target = m_findNextPathSpot();
	m_seek();
	setState(SEEK);
}


void Ship::m_checkBounds()
{

	if (getPosition().x > Config::SCREEN_WIDTH)
	{
		setPosition(glm::vec2(0.0f, getPosition().y));
	}

	if (getPosition().x < 0)
	{
		setPosition(glm::vec2(800.0f, getPosition().y));
	}

	if (getPosition().y > Config::SCREEN_HEIGHT)
	{
		setPosition(glm::vec2(getPosition().x, 0.0f));
	}

	if (getPosition().y < 0)
	{
		setPosition(glm::vec2(getPosition().x, 600.0f));
	}

}

void Ship::m_reset()
{
	setIsColliding(false);
	const int halfWidth = getWidth() * 0.5f;
	const auto xComponent = rand() % (640 - getWidth()) + halfWidth + 1;
	const auto yComponent = -getHeight();
	setPosition(glm::vec2(xComponent, yComponent));
}


void Ship::m_seek()
{
	getTarget();
	const auto desired = Util::normalize(m_target - getPosition()) * m_maxSpeed;
	auto steer = (desired - getVelocity());

	steer = Util::limitMagnitude(steer, m_steerForce);
	setAcceleration(steer);
}

glm::vec2 Ship::m_findNextPathSpot()
{
	std::vector<Tile*> adjacent = getTile()->getNeighbours();

	for (int i = 0; i < 4; ++i)
	{
		if (adjacent[i] != nullptr)
		{
			if (adjacent[i]->getTileState() == OPEN || adjacent[i]->getTileState() == GOAL)
			{
				nextTile = adjacent[i];
				getTile()->setTileState(CLOSED);
				//added anim triggers, moved setTile to arrive
				if (getTile()->getUp() == nextTile)
					direction = 0;
				if (getTile()->getLeft() == nextTile)
					direction = 1;
				if (getTile()->getRight() == nextTile)
					direction = 2;
				if (getTile()->getDown() == nextTile)
					direction = 3;
			}
		}
	}
	return nextTile->getPosition();
}

void Ship::animate()
{
	if (g_frame == g_frameMax)
	{
		g_frame = 0; // Roll the frame ctr back to 0.
		g_sprite++; // Increment the sprite index.
		if (g_sprite == g_spriteMax)
			g_sprite = 0;
		// Don't need anything.
	}
	g_frame++; // Increment frame counter.
}
