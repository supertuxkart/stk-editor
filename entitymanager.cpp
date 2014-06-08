#include "entitymanager.hpp"

#include <iostream>



void EntityManager::clearSelection()
{
    std::list<ISceneNode*>::iterator it;
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
        (*it)->setDebugDataVisible(0);
    m_selected_elements.clear();
}


/** This function is called whenever the user tries to select /
*  unselect a node. If it is in the nodelist, it can be selected.
*  If it is already selected, it is released
*  \param node The node the user clicked on
*/
void EntityManager::selectNode(ISceneNode* node)
{
    // we check if it is selected already
    std::list<ISceneNode*>::iterator it;
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
    {
        // if it is selected, it will be released
        if ((*it)==node)
        {
            (*it)->setDebugDataVisible(0);
            m_selected_elements.erase(it);
            return;
        }
    }

    // it isn't in our selection list, it is added to it
    m_selected_elements.push_back(node);
    node->setDebugDataVisible(EDS_BBOX);
}

