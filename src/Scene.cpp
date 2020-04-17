#include "Scene.h"
#include "DisplayObject.h"

Scene::Scene()
= default;

Scene::~Scene()
{
	removeAllChildren();
}


void Scene::addChild(DisplayObject * child)
{
	m_displayList.push_back(child);
}

void Scene::removeAllChildren()
{
	if (!m_displayList.empty()) {
		for (int i = 0; i < (int)m_displayList.size(); i++)
		{
			m_displayList[i] = nullptr;
		}
		m_displayList.clear();
		m_displayList.resize(0);
		m_displayList.shrink_to_fit();
	}
}


int Scene::numberOfChildren() const
{
	return m_displayList.size();
}
