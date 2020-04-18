#include "StartScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"

StartScene::StartScene()
{
	StartScene::start();
}

void StartScene::draw()
{
	TheTextureManager::Instance()->draw("start", Config::SCREEN_WIDTH / 2, Config::SCREEN_HEIGHT / 2,
		TheGame::Instance()->getRenderer(), 0, 255, true);
}

void StartScene::update()
{
}

void StartScene::clean()
{
	std::cout << "Clean called on StartScene" << std::endl;
}

void StartScene::handleEvents()
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
				TheGame::Instance()->changeSceneState(SceneState::END_SCENE);
				break;
			}
			break;

		default:
			break;
		}
	}
}

void StartScene::start()
{
	TheTextureManager::Instance()->load("../Assets/textures/start_screen.png",
		"start", TheGame::Instance()->getRenderer());
}
