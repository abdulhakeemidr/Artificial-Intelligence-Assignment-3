#pragma once

#include "PathFindingDisplayObject.h"

class Enemy : public PathFindingDisplayObject {
public:
	Enemy() {}
	~Enemy() {}
	void update() {}
	void draw() {}
	void clean() {}


	bool GetActive() { return m_active; }
	void SetActive(bool active) { m_active = active; }
	int GetHealth() { return m_health; }
	void SetHealth(int health) { m_health = health; }

private:
	bool m_active;
	int m_health;
};