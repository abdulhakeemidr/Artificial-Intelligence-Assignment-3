#include "PFDispObj.h"

PFDispObj::PFDispObj() : m_currentTile(nullptr)
{
}

PFDispObj::~PFDispObj() = default;

Tile * PFDispObj::getTile()
{
	return m_currentTile;
}

void PFDispObj::setTile(Tile * newTile)
{
	m_currentTile = newTile;
}

void PFDispObj::clean()
{
	delete m_currentTile;
	m_currentTile = NULL;
}
