#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include <list>
#include <irrlicht.h>

using namespace irr;
using namespace scene;

class EntityManager
{
private:
    std::list<ISceneNode*>  m_elements;
    std::list<ISceneNode*>  m_selected_elements;
public:
    void add(ISceneNode* e)                     { m_elements.push_back(e); }
    std::list<ISceneNode*>  getSelection()       { return m_selected_elements; }

    void                    clearSelection();

    void                    selectNode(ISceneNode* node);
};

#endif
