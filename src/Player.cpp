#include "Player.h"
#include "Game.h"
#include "Util.h"

Player::Player()
{
	setPosition(glm::vec2(0, 430.0f));

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("player");
	setWidth(size.x);
	setHeight(size.y);
	setIsColliding(false);
	setType(GameObjectType::PLAYER);
	m_angle = 0.0;
	SetHealth(100);
	SetMeleeRange(10);
	SetMeleeTimerMax(30);
	SetBulletTimerMax(10);
	SetMeleeTimer(0);
	SetBulletTimer(0);
}

Player::~Player()
{
}

void Player::moveX(bool reverse)
{
	if (!reverse)
		setPosition(glm::vec2(getPosition().x + getVelocity().x * Config::MOVE_SPEED, getPosition().y));
	else setPosition(glm::vec2(GetOldPosition().x, getPosition().y));
}

void Player::moveY(bool reverse)
{
	if (!reverse)
		setPosition(glm::vec2(getPosition().x, getPosition().y + getVelocity().y * Config::MOVE_SPEED));
	else setPosition(glm::vec2(getPosition().x, GetOldPosition().y));
}

void Player::draw()
{
	if (GetMeleeTimer() == 0)
		TextureManager::Instance()->animate("player", getPosition().x, getPosition().y, 32, 32, 2, 0, m_angle, Game::Instance()->getRenderer());
	else TextureManager::Instance()->animate("player", getPosition().x, getPosition().y, 32, 32, 2, 1, m_angle, Game::Instance()->getRenderer());
}

void Player::update()
{
	
	SetOldPosition(getPosition());
	m_angle = Util::angle(glm::vec2(0, -1), getRotation()); //needs to be fixed at x<0
	SetBulletTimer(GetBulletTimer() - 1);
	if (GetBulletTimer() < 0)
		SetBulletTimer(0);
	SetMeleeTimer(GetMeleeTimer() - 1);
	if (GetMeleeTimer() < 0)
		SetMeleeTimer(0);
}

void Player::clean()
{
}
