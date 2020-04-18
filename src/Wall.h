#pragma once
#include "PathFindingDisplayObject.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "Tile.h"
#include "Scene.h"

class Wall : public PathFindingDisplayObject {
private:
	SDL_Rect m_bounds;
public:
	Wall();
	~Wall() {}
	SDL_Rect GetBounds() { return m_bounds; }
	void SetBounds(SDL_Rect bounds) { m_bounds = bounds; }
	void CalculateBounds();
	void update() {}
	void draw();
	void clean() {}
};

class DestructibleWall : public Wall {
private:
	int m_health;
public:
	DestructibleWall();
	~DestructibleWall() {}
	int GetHealth() { return m_health; }
	void SetHealth(int health) { m_health = health; }
	void update() {}
	void draw();
	void clean() {}
};