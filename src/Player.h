#pragma once
#ifndef __PLAYER__
#define	__PLAYER__

#include "DisplayObject.h"

class Player : public DisplayObject
{
public:
	Player();
	~Player();

	// DisplayObject LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
private:

};

#endif