#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include <list>
#include <irrlicht.h>

using namespace irr;
using namespace scene;

class Entity;

class EntityManager
{
private:
    std::list<Entity*>  m_elements;
    std::list<Entity*>  m_selected_elements;
public:
    void add(Entity* e)                     { m_elements.push_back(e);     }
    std::list<Entity*> getSelection()       { return m_selected_elements;  }

    void               clearSelection();

    void               selectNode(ISceneNode* node);
};

#endif
