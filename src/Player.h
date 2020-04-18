#pragma once
#ifndef __Player__
#define __Player__

#include "PathFindingDisplayObject.h"
#include "TextureManager.h"
#include "SoundManager.h"

class Player : public PathFindingDisplayObject {
public:
	Player();
	~Player();
	int GetHealth() { return m_health; }
	void SetHealth(int health) { m_health = health; }
	int GetMeleeRange() { return m_meleeRange; }
	void SetMeleeRange(int range) { m_meleeRange = range; }
	int GetMeleeTimer() { return m_meleeTimer; }
	void SetMeleeTimer(int timer) { m_meleeTimer = timer; }
	int GetBulletTimer() { return m_bulletTimer; }
	void SetBulletTimer(int timer) { m_bulletTimer = timer; }
	int GetMeleeTimerMax() { return m_meleeTimerMax; }
	void SetMeleeTimerMax(int timer) { m_meleeTimerMax = timer; }
	int GetBulletTimerMax() { return m_bulletTimerMax; }
	void SetBulletTimerMax(int timer) { m_bulletTimerMax = timer; }
	glm::vec2 GetOldPosition() { return m_oldPosition; }
	void SetOldPosition(glm::vec2 old) { m_oldPosition = old; }
	double GetAngle() { return m_angle; }

	void moveX(bool reverse);
	void moveY(bool reverse);

	// Draw the object
	void draw();
	// Update the object
	void update();
	// remove anything that needs to be deleted
	void clean();
private:
	int m_health, m_meleeRange,
		m_meleeTimer, m_meleeTimerMax,
		m_bulletTimer, m_bulletTimerMax;
	double m_angle;
	glm::vec2 m_oldPosition;
};


#endif /* defined (__Player__) */