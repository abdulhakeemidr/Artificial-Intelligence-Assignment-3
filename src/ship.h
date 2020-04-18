#pragma once
#ifndef __SHIP__
#define __SHIP__

#include "PathFindingDisplayObject.h"
#include "TextureManager.h"

class Ship final : public PathFindingDisplayObject
{
public:
	Ship();
	~Ship();

	// Inherited via GameObject
	void draw() override;
	void update() override;
	void clean() override;

	void turnRight();
	void turnLeft();
	void move();

	void m_checkArrival();

	glm::vec2 getTarget();
	void setTarget();
	bool atGoal = false;
	void forceAnimate() { animate(); }
	void setScale(glm::vec2 scale) { m_scale = scale; }
private:


	void m_checkState();
	void m_checkBounds();

	void m_reset();

	// steering behaviours
	void m_seek();
	glm::vec2 m_findNextPathSpot();
	glm::vec2 m_scale;
	void animate();
	float m_maxSpeed;
	double m_currentDirection;
	float m_turnSpeed;
	float m_steerForce;
	glm::vec2 m_target;

	int g_frame = 0;
	int g_frameMax = 5; // Number of frame to display each up/down of tongue.
	int g_sprite = 0; // Sprite counter.
	int g_spriteMax = 5;
	int direction = 3;

	Tile* nextTile = nullptr;
};


#endif /* defined (__SHIP__) */

