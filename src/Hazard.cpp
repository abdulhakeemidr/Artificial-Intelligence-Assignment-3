#include "Hazard.h"
#include "Game.h"
#include "Util.h"

Hazard::Hazard(bool friendly, glm::vec2 startPosition)
{
	setPosition(startPosition);
	SetFriendly(friendly);
	setHeight(30);
	SetBounds(SDL_Rect{ (int)getPosition().x - getHeight() / 2, (int)getPosition().y - getHeight() / 2, getHeight(), getHeight() });
	SetActive(true);
	setType(MELEE);
}

void Hazard::update()
{
	setPosition(playerPosition + ((float)range * Util::normalize(glm::vec2(mousePosition.x - playerPosition.x, mousePosition.y - playerPosition.y))));
	SetActiveFrames(GetActiveFrames() - 1);
	if (GetActiveFrames() <= 0)
		SetActive(false);
}

void Hazard::getUpdateInfo(glm::vec2 playerPosition, glm::vec2 mousePosition, int range)
{
	this->playerPosition = playerPosition;
	this->mousePosition = mousePosition;
	this->range = range;
}

void Hazard::draw()
{
	if (GetFriendly()) {
		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), 107, 142, 35, 100);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &GetBounds());
	}
	else {
		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), 220, 20, 60, 100);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &GetBounds());
	}
}

Bullet::Bullet(bool friendly, glm::vec2 startPosition, glm::vec2 velocity) : Hazard(friendly, startPosition)
{
	setVelocity(velocity);
	setAcceleration(glm::vec2(10, 10));
	setHeight(4);
	SetBounds(SDL_Rect{ (int)getPosition().x - getHeight() / 2, (int)getPosition().y - getHeight() / 2, getHeight(), getHeight() });
	setType(BULLET);
}

void Bullet::update()
{
	setPosition(getPosition() + (getVelocity() * getAcceleration()));
	SetBounds(SDL_Rect{ (int)getPosition().x - getHeight() / 2, (int)getPosition().y - getHeight() / 2, getHeight(), getHeight() });
}

void Bullet::draw()
{
	if (GetFriendly()) {
		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), 107, 142, 35, 255);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &GetBounds());
	}
	else {
		SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), 220, 20, 60, 255);
		SDL_RenderFillRect(Game::Instance()->getRenderer(), &GetBounds());
	}
}
