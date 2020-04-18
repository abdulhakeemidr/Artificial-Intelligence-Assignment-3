#pragma once
#ifndef __PFDISPOBJ__
#define __PFDISPOBJ__

#include "DisplayObject.h"

class PFDispObj : public DisplayObject
{
public:
	friend class Tile;
	PFDispObj();
	virtual ~PFDispObj();
	virtual void draw() override = 0;
	virtual void update() override = 0;
	virtual void clean() override = 0;

	Tile* getTile();
	void setTile(Tile* newTile);

private:
	Tile* m_currentTile;

};

#endif // !__PFDISPOBJ__

