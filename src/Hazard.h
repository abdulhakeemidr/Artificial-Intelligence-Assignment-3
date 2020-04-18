#pragma once
#include "DisplayObject.h"
#include "SoundManager.h"
#include "SDL.h"


class Hazard : public DisplayObject {
public:
	Hazard(bool friendly, glm::vec2 startPosition);
	~Hazard() {}

	bool GetFriendly() { return m_friendly; }
	void SetFriendly(bool friendly) { m_friendly = friendly; }
	bool GetActive() { return m_active; }
	void SetActive(bool active) { m_active = active; }
	bool GetUsed() { return m_used; }
	void SetUsed(bool used) { m_used = used; }
	int GetActiveFrames() { return m_activeFrames; }
	void SetActiveFrames(int frames) { m_activeFrames = frames; }
	SDL_Rect GetBounds() { return m_bounds; }
	void SetBounds(SDL_Rect bounds) { m_bounds = bounds; }
	void getUpdateInfo(glm::vec2 playerPosition, glm::vec2 mousePosition, int range);

	void update();
	void draw();
	void clean() {}
private:
	bool m_friendly, m_active, m_used;
	int m_activeFrames;
	SDL_Rect m_bounds;
	//melee update info
	glm::vec2 playerPosition, mousePosition;
	int range;
};

class Bullet : public Hazard {
public:
	Bullet(bool firendly, glm::vec2 startPosition, glm::vec2 velocity);
	~Bullet() {}
	void update();
	void draw();
	void clean() {}
private:
};
