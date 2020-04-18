#include "Planet.h"
#include "Game.h"

Planet::Planet()
{
	TheTextureManager::Instance()->load("../Assets/textures/goald.png",
		"goald", TheGame::Instance()->getRenderer());

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("goald");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(glm::vec2(0.0f, 0.0f));
	setVelocity(glm::vec2(0.0f, 0.0f));
	setType(GameObjectType::PLANET);
}

Planet::~Planet()
= default;

void Planet::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	TheTextureManager::Instance()->draw("goald", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), 0, 255, true);
}

void Planet::update()
{
}

void Planet::clean()
{

}