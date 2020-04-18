#include "EndScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"

EndScene::EndScene()
{
	EndScene::start();
}

EndScene::~EndScene()
= default;

void EndScene::draw()
{
	TheTextureManager::Instance()->draw("end", Config::SCREEN_WIDTH/2, Config::SCREEN_HEIGHT/2,
		TheGame::Instance()->getRenderer(), 0, 255, true);
	//m_pShip->draw();
}

void EndScene::update()
{
	//m_pShip->forceAnimate();
}

void EndScene::clean()
{
	delete m_label;
	//delete m_pShip;
	//m_pShip = nullptr;
	removeAllChildren();
}

void EndScene::handleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				TheGame::Instance()->quit();
				break;
			case SDLK_1:
				TheGame::Instance()->changeSceneState(SceneState::PLAY_SCENE);
				break;
			case SDLK_2:
				TheGame::Instance()->changeSceneState(SceneState::START_SCENE);
				break;
			}
			break;
		default:
			break;
		}
	}
}

void EndScene::start()
{
	/*m_pShip = new Ship();
	m_pShip->setPosition(glm::vec2(190, 600));
	m_pShip->setScale(glm::vec2(200, 200));*/
	TheTextureManager::Instance()->load("../Assets/textures/end_screen.png",
		"end", TheGame::Instance()->getRenderer());
}
