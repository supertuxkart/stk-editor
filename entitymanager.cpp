#include "entitymanager.hpp"
#include "entity.hpp"

#include <iostream>



void EntityManager::clearSelection()
{
    std::list<Entity*>::iterator it;
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
        (*it)->clearBoundingBox();
    m_selected_elements.clear();
}


/** This function is called whenever the user tries to select /
*  unselect a node. If it is in the entity list, it can be selected.
*  If it is already selected, it is released
*  \param node The node the user clicked on
*/
void EntityManager::selectNode(ISceneNode* node)
{

    std::cout << node->getID() << std::endl;

    std::list<Entity*>::iterator it;
    for (it = m_elements.begin(); it != m_elements.end() && !(*it)->hasSceneNode(node);)
    {
        it++;
    }
    
    // if there is no entity with the node, there is nothing to do here
    if (it == m_elements.end()) return;

    Entity *e = (*it);

    // we check if it is selected already
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
    {
        // if it is selected, it will be released
        if ((*it)->hasSceneNode(node))
        {
            (*it)->clearBoundingBox();
            m_selected_elements.erase(it);
            return;
        }
    }

    // it isn't in our selection list, it is added to it
    m_selected_elements.push_back(e);
    e->drawBoundingBox();
}

