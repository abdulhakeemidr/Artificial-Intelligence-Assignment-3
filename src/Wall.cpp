#include "Wall.h"
#include "Game.h"

Wall::Wall()
{
	setType(WALL);
	setHeight(32);
	CalculateBounds();
}

void Wall::CalculateBounds()
{
	SetBounds(SDL_Rect{ (int)(getPosition().x - getHeight()/2), (int)(getPosition().y - getHeight()/2), getHeight(), getHeight() });
}

void Wall::draw()
{
	TextureManager::Instance()->draw("wall", getPosition().x, getPosition().y, Game::Instance()->getRenderer(), true);
}

DestructibleWall::DestructibleWall()
{
	SetHealth(100);
}

void DestructibleWall::draw()
{
	if (GetHealth() > 65)
		TextureManager::Instance()->animate("d_wall", getPosition().x, getPosition().y, 32, 32, 3, 0, 0, Game::Instance()->getRenderer());
	else if (GetHealth()>0 && GetHealth() <= 65)
		TextureManager::Instance()->animate("d_wall", getPosition().x, getPosition().y, 32, 32, 3, 1, 0, Game::Instance()->getRenderer());
	else TextureManager::Instance()->animate("d_wall", getPosition().x, getPosition().y, 32, 32, 3, 2, 0, Game::Instance()->getRenderer());
}
