#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "imgui.h"

// Game Objects
#include "Player.h"
#include "Enemy.h"
#include "Hazard.h"
#include "Wall.h"
#include "Planet.h"
#include "ship.h"
#include "Tile.h"
#include "Heuristic.h"


class PlayScene final : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Inherited via Scene
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:
	// GameObjects
	//Ship* m_pShip;
	//Planet* m_pPlanet;
	Player* m_pPlayer;
	std::vector<Enemy*> m_vEnemies;
	std::vector<Hazard*> m_vHazards;

	// Tile & Grid members
	std::vector<Tile*> m_pGrid;

	// walls
	int m_wallNum = Config::WALL_NUM;
	std::vector<DestructibleWall*> m_walls;
	void m_buildWalls();
	void m_eraseWalls();
	void m_resetImpassableTiles();
	void m_spawnWalls();
	void m_wallPassAdjustment();

	void m_resetGrid();
	void m_buildGrid();
	void m_mapTiles();

	int m_spawnObject(PathFindingDisplayObject* object);
	void m_spawnEnemy();
	void m_spawnPlayer();
	void m_spawnMeleeAttack(bool friendly, glm::vec2 position);
	void m_spawnBullet(bool friendly, glm::vec2 position, glm::vec2 velocity);

	void m_computeTileValues();
	void m_cleanInactive();

	// pathfinding functions & variables
	Tile* m_findLowestCostTile(Tile* current_tile);
	void m_findShortestPath(Tile* start_tile);
	std::vector<Tile*> m_openList;
	std::vector<Tile*> m_closedList;
	Heuristic m_heuristic;
	void m_selectHeuristic(Heuristic heuristic, int enemyIndex);
	int m_enemyBackIndex;

	glm::vec2 m_mousePosition;
	bool mouseRight, mouseLeft,
		key_w, key_s, key_a, key_d;

	// ImGui utility functions
	void m_ImGuiKeyMap();
	void m_ImGuiSetStyle();
	void m_updateUI();

	// ImGui menu variables
	bool m_exitApp = false;
	bool m_displayAbout = false;
	bool m_displayUI = true;

	// ImGui button variables
	ImVec4 m_manhattanButtonColour;
	ImVec4 m_euclideanButtonColour;

	// ImGui visibility variables
	bool m_enemiesVisible = true;
	bool m_playerVisible = true;
	bool m_wallsVisible = true;
	
	void m_resetAll();

	int pathLength;
	int pathPar = 18;
	int parForCourse;

	int calculateParForCourse();

};

#endif defined (__PLAY_SCENE__)