#include "PlayScene.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "SceneState.h"
#include "Util.h"
#include "IMGUI_SDL/imgui_sdl.h"
#include "TileComparators.h"


// Pathfinding & Steering functions ***********************************************

void PlayScene::m_buildWalls()
{
	for (int index = 0; index < m_wallNum; ++index)
	{
		auto wall = new DestructibleWall;
		addChild(wall);
		m_walls.push_back(wall);
		wall = nullptr;
	}
}

void PlayScene::m_eraseWalls()
{
	for (auto mine : m_walls)
	{
		delete mine;
		mine = nullptr;
	}
	m_walls.clear();
	m_walls.resize(0);
	m_walls.shrink_to_fit();
}

void PlayScene::m_resetImpassableTiles()
{
	for (auto tile : m_pGrid)
	{
		if(tile->getTileState() == IMPASSABLE)
		{
			tile->setTileState(UNVISITED);
		}
	}
}

void PlayScene::m_spawnWalls()
{
	m_resetImpassableTiles();
	for (int i = 0; i < m_wallNum; ++i)
	{
		m_spawnObject(m_walls[i]);
		m_walls[i]->getTile()->setTileState(IMPASSABLE);
	}

	// first pass
	m_wallPassAdjustment();

	//second pass
	m_wallPassAdjustment();

	//third pass
	m_wallPassAdjustment();

	//just to be safe!
}


// This utility function checks for mines that create a dead end
// and set the middle tile to IMPASSABLE

void PlayScene::m_wallPassAdjustment()
{
	for (auto tile : m_pGrid)
	{
		auto wallCount = 0;
		auto nullCount = 0;
		auto neighbours = tile->getNeighbours();
		
		for (auto i = 0; i < 4; ++i)
		{
			if (neighbours[i] != nullptr)
			{
				if (neighbours[i]->getTileState() == IMPASSABLE)
				{
					wallCount++;
				}
			}
			else
			{
				nullCount++;
			}

			if (wallCount + nullCount > 2)
			{
				if((tile->getTileState() != START) && (tile->getTileState() != GOAL))
				{
					tile->setTileState(IMPASSABLE);
				}
			}
		}
	}
}

void PlayScene::m_resetGrid()
{
		for (auto tile : m_openList)
		{
			tile->setTileState(UNVISITED);
			m_openList.pop_back();
		}

		for (auto tile : m_closedList)
		{
			tile->setTileState(UNVISITED);
			m_closedList.pop_back();
		}
}

void PlayScene::m_buildGrid()
{
	const auto size = Config::TILE_SIZE;
	const auto offset = size * 0.5f;
	
	m_pGrid = std::vector<Tile*>(); // instantiates a structure of type vector<Tile*>

	for (auto row = 0; row < Config::ROW_NUM; ++row)
	{
		for (auto col = 0; col < Config::COL_NUM; ++col)
		{
			auto tile = new Tile(glm::vec2(offset + size * col, offset + size * row), 
				glm::vec2(col, row));
			addChild(tile);
			tile->setTileState(UNVISITED);
			m_pGrid.push_back(tile);
		}
	}
}

void PlayScene::m_mapTiles()
{
	for (auto tile : m_pGrid)
	{
		const auto x = tile->getGridPosition().x;
		const auto y = tile->getGridPosition().y;

		if(y != 0)                   { tile->setUp   (m_pGrid[x + ((y - 1) * Config::COL_NUM)]); }
		if(x != Config::COL_NUM - 1) { tile->setRight(m_pGrid[(x + 1) + (y * Config::COL_NUM)]); }
		if(y != Config::ROW_NUM - 1) { tile->setDown (m_pGrid[x + ((y + 1) * Config::COL_NUM)]); }
		if(x != 0)					 { tile->setLeft (m_pGrid[(x - 1) + (y * Config::COL_NUM)]); }
	}
}

int PlayScene::m_spawnObject(PathFindingDisplayObject* object)
{
	m_resetGrid();
	
	Tile* randomTile = nullptr;
	auto randomTileIndex = 0;
	do
	{
		randomTileIndex = int(Util::RandomRange(0, m_pGrid.size() - 1));
		randomTile = m_pGrid[randomTileIndex];
	} while (randomTile->getTileState() != UNVISITED); // search for empty tile


	if (object->getTile() != nullptr)
	{
		object->getTile()->setTileState(UNVISITED);
	}

	object->setPosition(randomTile->getPosition());
	object->setTile(randomTile);

	return randomTileIndex;
}

void PlayScene::m_spawnEnemy()
{
	m_vEnemies.push_back(new Enemy());
	m_enemyBackIndex++;
	addChild(m_vEnemies[m_enemyBackIndex]);
	m_spawnObject(m_vEnemies[m_enemyBackIndex]);
	m_vEnemies[m_enemyBackIndex]->getTile()->setTileState(START);
}

void PlayScene::m_spawnPlayer()
{
	m_spawnObject(m_pPlayer);
	m_computeTileValues();
	m_pPlayer->getTile()->setTileState(GOAL);
}

void PlayScene::m_spawnMeleeAttack(bool friendly, glm::vec2 position)
{
	m_vHazards.push_back(new Hazard(friendly, position));
}

void PlayScene::m_spawnBullet(bool friendly, glm::vec2 position, glm::vec2 velocity)
{
	m_vHazards.push_back(new Bullet(friendly, position, m_pPlayer->getRotation()));
}

void PlayScene::m_computeTileValues()
{
	for (auto tile : m_pGrid)
	{
		tile->setHeuristic(m_heuristic);
		tile->setTargetDistance(m_pPlayer->getTile()->getGridPosition());
	}
}

void PlayScene::m_cleanInactive()
{
	//remove hazards
	for (int i = 0; i < (int)m_vHazards.size(); i++) {
		if (!m_vHazards[i]->GetActive()) {
			delete m_vHazards[i];
			m_vHazards[i] = nullptr;
		}
	}
	m_vHazards.erase(remove(m_vHazards.begin(), m_vHazards.end(), nullptr), m_vHazards.end());
	m_vHazards.shrink_to_fit();

	//remove enemies
	for (int i = 0; i < (int)m_vEnemies.size(); i++) {
		if (!m_vEnemies[i]->GetActive()) {
			delete m_vEnemies[i];
			m_vEnemies[i] = nullptr;
		}
	}
	m_vEnemies.erase(remove(m_vEnemies.begin(), m_vEnemies.end(), nullptr), m_vEnemies.end());
	m_vEnemies.shrink_to_fit();

}

Tile* PlayScene::m_findLowestCostTile(Tile* current_tile)
{
	Tile* minTile = nullptr;
	auto min = INFINITY;
	int tile_num = 0;

	std::vector<Tile*> adjacent = current_tile->getNeighbours();

	std::cout << "+-- New Tile ------------------->" << std::endl;
	std::cout << "+-                             ->" << std::endl;
	
	current_tile->displayTile();

	std::cout << "+-- Selecting Minimum Tile ----->" << std::endl;
	std::cout << "+------------------------------->" << std::endl;
	
	for (int i = 0; i < 4; ++i)
	{
		if(adjacent[i] != nullptr)
		{
			if(adjacent[i]->getTileState() == GOAL)
			{
				minTile = adjacent[i];
				return minTile;
			}

			if (adjacent[i]->getTileState() == UNVISITED)
			{
				if (min > adjacent[i]->getTileValue())
				{
					min = adjacent[i]->getTileValue();
					minTile = adjacent[i];

					if (minTile->getTileState() == GOAL)
					{
						return minTile;
					}
				}
			}
		}
	}

	if(min == INFINITY)
	{
		std::cout << "+-- No Min Found ---- Return --->" << std::endl;
		std::cout << "+------------------------------->" << std::endl;
		current_tile->setTileState(NO_PATH);
		return current_tile;
	}

	std::cout << "+------------------------------->" << std::endl;
	std::cout << "+- Minimum value is: " << min << " ------>" << std::endl;
	std::cout << "+------------------------------->" << std::endl;
	std::cout << "+- Marking Tiles --------------->" << std::endl;
	std::cout << "+------------------------------->" << std::endl;

	for (int i = 0; i < 4; ++i)
	{
		if ((adjacent[i] != nullptr) && (adjacent[i]->getTileState() != IMPASSABLE))
		{
			if (minTile == adjacent[i])
			{
				adjacent[i]->setTileState(OPEN);
				m_openList.push_back(adjacent[i]);
			}
			else
			{
				if (adjacent[i]->getTileState() == UNVISITED)
				{
					adjacent[i]->setTileState(CLOSED);
					m_closedList.push_back(adjacent[i]);
				}
				
			}
		}
	}

	return minTile;
}

void PlayScene::m_findShortestPath(Tile* start_tile)
{
	pathLength = 0;

	while(start_tile->getTileState() != GOAL)
	{
		start_tile = m_findLowestCostTile(start_tile);
		if(start_tile->getTileState() == NO_PATH)
		{
			std::cout << "Dead end - find another path" << std::endl;
			m_resetGrid();
			m_findShortestPath(start_tile);
			//std::cout << "No Path found" << std::endl;
			break;
		}
		
		if(pathLength > Config::COL_NUM + Config::ROW_NUM)
		{
			std::cout << "No Path found" << std::endl;
			break;
		}
		pathLength++;
	}
	std::cout << "Path Length: " << pathLength << std::endl;
}

void PlayScene::m_selectHeuristic(Heuristic heuristic, int enemyIndex)
{
	// recalculate grid
	m_heuristic = heuristic;
	auto start = m_vEnemies[enemyIndex]->getTile();
	auto goal = m_pPlayer->getTile();
	m_resetGrid();
	m_computeTileValues();
	start->setTileState(START);
	goal->setTileState(GOAL);
	m_findShortestPath(m_vEnemies[enemyIndex]->getTile());

	// change button colour depending on heuristic chosen
	switch(heuristic)
	{
	case MANHATTAN:
		m_manhattanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 1.0f);
		m_euclideanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		break;
	case EUCLIDEAN:
		m_manhattanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		m_euclideanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 1.0f);
		break;
	}
	
}

// ImGui functions ***********************************************

void PlayScene::m_ImGuiKeyMap()
{
	auto& io = ImGui::GetIO();

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;

	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

void PlayScene::m_ImGuiSetStyle()
{
	auto& style = ImGui::GetStyle();

	style.Alpha = 0.8f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void PlayScene::m_updateUI()
{
	// Prepare Window Frame
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // use for debug purposes

	std::string windowString = "Settings ";

	ImGui::Begin(&windowString[0], NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

	// set window to top getLeft corner
	ImGui::SetWindowPos(ImVec2(0, 0), true);

	/*************************************************************************************************/
	/* MENU                                                                                          */
	/*************************************************************************************************/

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Separator();
			ImGui::MenuItem("Exit", NULL, &m_exitApp);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Separator();
			ImGui::MenuItem("About", NULL, &m_displayAbout);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (m_exitApp)
	{
		TheGame::Instance()->quit();
	}

	if (m_displayAbout)
	{
		ImGui::Begin("About Pathfinding Simulator", &m_displayAbout, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Separator();
		ImGui::Text("Authors:");
		ImGui::Text("Placeholder Team Name (tm) (c)");
		ImGui::End();
	}

	/*************************************************************************************************/
	/*if (ImGui::Button("Respawn Enemy"))
	{
		m_spawnShip();
	}

	ImGui::SameLine();

	if (ImGui::Button("Respawn Gold"))
	{
		m_spawnPlanet();
	}

	ImGui::SameLine();

	if (ImGui::Button("Respawn Trees"))
	{
		m_spawnMines();
	}*/

	ImGui::SameLine();

	/*if (ImGui::Button("Find Shortest Path"))
	{
		m_findShortestPath(m_pShip->getTile());
	}

	if(ImGui::CollapsingHeader("Heuristic Options"))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, m_manhattanButtonColour);
		if (ImGui::Button("Manhattan Distance"))
		{
			m_selectHeuristic(MANHATTAN);
		}
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, m_euclideanButtonColour);
		if (ImGui::Button("Euclidean Distance"))
		{
			m_selectHeuristic(EUCLIDEAN);
		}
		ImGui::PopStyleColor();
	}*/

	if(ImGui::SliderInt("Number of Trees", &m_wallNum, 1, 298))
	{
		m_eraseWalls();
		m_buildWalls();
		m_spawnWalls();
	}

	if(ImGui::CollapsingHeader("Visibility Options"))
	{
		if(ImGui::Checkbox("Ship", &m_enemiesVisible))
		{ }
		ImGui::SameLine();

		if (ImGui::Checkbox("Gold", &m_playerVisible))
		{
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("Mines", &m_wallsVisible))
		{
		}
		
	}
	

	// Main Window End
	ImGui::End();
}

/*** SCENE FUNCTIONS ***/
void PlayScene::m_resetAll()
{
	
}

int PlayScene::calculateParForCourse()
{
	int parForHole = pathLength - pathPar;
	parForCourse += parForHole;

	return parForCourse;

	std::cout << "Congratulations, you scored a " << parForHole << "on this hole!" << std::endl;
	std::cout << "Your total score is " << parForCourse << ". :)" << std::endl;
}

void PlayScene::start()
{
	TheTextureManager::Instance()->load("../Assets/textures/map.png",
		"map", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/Player.png",
		"player", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/Enemy.png",
		"enemy", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/Destructible_Wall.png",
		"d_wall", TheGame::Instance()->getRenderer());

	// setup default heuristic options
	m_heuristic = MANHATTAN;
	m_manhattanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 0.40f);
	m_euclideanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	m_enemyBackIndex = -1;
	
	m_buildGrid();
	m_mapTiles();

	// instantiate planet and add it to the DisplayList
	m_pPlayer = new Player();
	addChild(m_pPlayer);
	m_spawnPlayer();
	
	//m_spawnEnemy();

	m_buildWalls();
	m_spawnWalls();
	for (auto wall : m_walls) {
		wall->CalculateBounds();
	}
}

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	TheTextureManager::Instance()->draw("map", 1024 / 2, 768 / 2,
		TheGame::Instance()->getRenderer(), 0.0f, 255, true);


	if (m_displayUI)
	{
		for (auto tile : m_pGrid)
		{
			tile->draw();
		}
	}
	
	if(m_enemiesVisible)
	{
		for (auto enemy : m_vEnemies) {
			enemy->draw();
		}
	}

	if(m_wallsVisible)
	{
		for (auto wall : m_walls)
		{
			wall->draw();
		}
	}

	if (m_playerVisible)
	{
		m_pPlayer->draw();
	}

	for (auto hazard : m_vHazards) {
		hazard->draw();
	}
	

	// ImGui Rendering section - DO NOT MOVE OR DELETE
	if (m_displayUI)
	{
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
		SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 255, 255, 255, 255);
	}
}

void PlayScene::update()
{
	const auto size = Config::TILE_SIZE;
	const auto offset = size * 0.5f;
	for (auto tile : m_pGrid) {
		tile->update();
		//enemies will need to be changed, currently just seeking player
		for (auto enemy : m_vEnemies) {
			if (Util::distance(enemy->getPosition(), enemy->getTile()->getPosition()) <= 1.5f) {
				enemy->getTile()->setTileState(CLOSED);
				setState(SEEK);
			}
		}
	}

	for (auto enemy : m_vEnemies) {
		enemy->update();
	}

	for (auto hazard : m_vHazards) {
		hazard->getUpdateInfo(m_pPlayer->getPosition(), m_mousePosition, m_pPlayer->GetMeleeRange());
		hazard->update();
	}

	//player - wall collisions
	m_pPlayer->setVelocity(Util::normalize(m_pPlayer->getVelocity()));
	m_pPlayer->setRotation(Util::normalize(m_mousePosition - m_pPlayer->getPosition()));
	m_pPlayer->update();
	m_pPlayer->moveX(false);
	for (int i = 0; i < (int)m_walls.size(); i++) {
		if (m_walls[i]->GetHealth() > 0 && CollisionManager::CircleRect(m_pPlayer, m_walls[i]->GetBounds())) {
			m_pPlayer->moveX(true);
		}
	}
	m_pPlayer->moveY(false);
	for (int i = 0; i < (int)m_walls.size(); i++) {
		if (m_walls[i]->GetHealth() > 0 && CollisionManager::CircleRect(m_pPlayer, m_walls[i]->GetBounds())) {
			m_pPlayer->moveY(true);
		}
	}
	m_pPlayer->setVelocity(glm::vec2(0.0f, 0.0f));
	for (auto hazard : m_vHazards) {
		if (!(hazard->GetFriendly()) && CollisionManager::squaredRadiusCheck(m_pPlayer, hazard)) {
			if (hazard->getType() == MELEE) {
				m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 15);
			}
			else m_pPlayer->SetHealth(m_pPlayer->GetHealth() - 5);
			hazard->SetActive(false); 
		}
	}

	for (auto hazard : m_vHazards) {
		for (auto enemy : m_vEnemies) {
			if (hazard->GetFriendly() && CollisionManager::squaredRadiusCheck(hazard, enemy)) {
				if (hazard->getType() == MELEE) {
					enemy->SetHealth(enemy->GetHealth() - 50);
				}
				else enemy->SetHealth(enemy->GetHealth() - 20);
				if (enemy->GetHealth() <= 0)
					enemy->SetActive(false);
				hazard->SetActive(false);
			}
		}
	}

	for (auto hazard : m_vHazards) {
		for (auto wall : m_walls) {
			if (wall->GetHealth() > 0 && CollisionManager::CircleRect(hazard, wall->GetBounds())) {
				if (hazard->getType() == MELEE) {
					wall->SetHealth(wall->GetHealth() - 30);
				}
				else wall->SetHealth(wall->GetHealth() - 15);
				if (wall->GetHealth() < 0)
					wall->SetHealth(0);
				hazard->SetActive(false);
			}
		}
	}

	if (mouseRight) {
		if (m_pPlayer->GetMeleeTimer() == 0) {
			m_spawnMeleeAttack(true, m_pPlayer->getPosition());
			m_pPlayer->SetMeleeTimer(m_pPlayer->GetMeleeTimerMax());
		}
	}
	if (mouseLeft) {
		if (m_pPlayer->GetBulletTimer() == 0) {
			m_spawnBullet(true, m_pPlayer->getPosition(), m_pPlayer->getRotation());
			m_pPlayer->SetBulletTimer(m_pPlayer->GetBulletTimerMax());
		}
	}

	if (m_displayUI)
	{
		m_updateUI();
	}

	m_cleanInactive();
	if (m_pPlayer->GetHealth() <= 0)
		TheGame::Instance()->changeSceneState(SceneState::END_SCENE);
}

void PlayScene::clean()
{
	std::cout << "PlayScene Clean Called" << std::endl;
	delete m_pPlayer;

	for (auto tile : m_pGrid)
	{
		delete tile;
		tile = nullptr;
	}
	m_pGrid.clear();
	m_pGrid.resize(0);
	m_pGrid.shrink_to_fit();

	for (auto wall : m_walls)
	{
		delete wall;
		wall = nullptr;
	}
	m_walls.clear();
	m_walls.resize(0);
	m_walls.shrink_to_fit();

	for (auto enemy : m_vEnemies)
	{
		delete enemy;
		enemy = nullptr;
	}
	m_vEnemies.clear();
	m_vEnemies.resize(0);
	m_vEnemies.shrink_to_fit();

	for (auto hazard : m_vHazards)
	{
		delete hazard;
		hazard = nullptr;
	}
	m_vHazards.clear();
	m_vHazards.resize(0);
	m_vHazards.shrink_to_fit();

	m_openList.clear();
	m_openList.resize(0);
	m_openList.shrink_to_fit();

	m_closedList.clear();
	m_closedList.resize(0);
	m_openList.shrink_to_fit();

	removeAllChildren();
}

void PlayScene::handleEvents()
{
	auto& io = ImGui::GetIO();
	auto wheel = 0;

	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;
		case SDL_MOUSEMOTION:
			m_mousePosition.x = event.motion.x;
			m_mousePosition.y = event.motion.y;
			break;
		case SDL_MOUSEWHEEL:
			wheel = event.wheel.y;
			break;
		case SDL_TEXTINPUT:
			io.AddInputCharactersUTF8(event.text.text);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == 1) {
				mouseRight = true;
			}
			if (event.button.button == 3) {
				mouseLeft = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == 1) {
				mouseRight = false;
			}
			if (event.button.button == 3) {
				mouseLeft = false;
			}
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				TheGame::Instance()->quit();
				break;
			case SDLK_1:
				TheGame::Instance()->changeSceneState(SceneState::START_SCENE);
				break;
			/*case SDLK_2:
				TheGame::Instance()->changeSceneState(SceneState::END_SCENE);
				break;*/
			case SDLK_BACKQUOTE:
				m_displayUI = (m_displayUI) ? false : true;
				break;
			case SDLK_p:
				m_resetAll();
				break;

				/************************************************************************/
			case SDLK_w:
				m_pPlayer->setVelocity(m_pPlayer->getVelocity() - glm::vec2(0.0f, 1.0f));
				break;
			case SDLK_s:
				m_pPlayer->setVelocity(m_pPlayer->getVelocity() + glm::vec2(0.0f, 1.0f));
				break;
			case SDLK_a:
				m_pPlayer->setVelocity(m_pPlayer->getVelocity() - glm::vec2(1.0f, 0.0f));
				break;
			case SDLK_d:
				m_pPlayer->setVelocity(m_pPlayer->getVelocity() + glm::vec2(1.0f, 0.0f));
				break;
			default:
				break;
			}
			{
				const int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				m_pPlayer->setVelocity(glm::vec2(m_pPlayer->getVelocity().x, 0));
				break;
			case SDLK_s:
				m_pPlayer->setVelocity(glm::vec2(m_pPlayer->getVelocity().x, 0));
				break;
			case SDLK_a:
				m_pPlayer->setVelocity(glm::vec2(0, m_pPlayer->getVelocity().y));
				break;
			case SDLK_d:
				m_pPlayer->setVelocity(glm::vec2(0, m_pPlayer->getVelocity().y));
				break;
			default:
				
				break;
			}
			{
				const int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		default:
			
			break;
		}
	}

	io.DeltaTime = 1.0f / 60.0f;
	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);

	io.DisplaySize.x = 1280;
	io.DisplaySize.y = 720;

	m_ImGuiKeyMap();
	m_ImGuiSetStyle();
}
