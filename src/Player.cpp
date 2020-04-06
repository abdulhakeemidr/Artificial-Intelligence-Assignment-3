#include "Player.h"
#include "Game.h"

Player::Player()
{
	TheTextureManager::Instance()->load(
		"../Assets/sprites/megaman-idle.png",
		"player-idle", TheGame::Instance()->getRenderer());

	// define framewidth
	setWidth(81);

	// define frameheight
	setHeight(58);

	setPosition(400.0f, 300.0f);
	setType(PLAYER);
	setIsColliding(false);
}

Player::~Player() = default;

void Player::draw()
{
}

void Player::update()
{
}

void Player::clean()
{
}
