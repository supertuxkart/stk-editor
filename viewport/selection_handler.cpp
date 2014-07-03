#include "viewport/selection_handler.hpp"

#include "editor.hpp"

#include "input/keys.hpp"
#include "input/mouse.hpp"


void SelectionHandler::clearSelection()
{
    list<ISceneNode*>::Iterator it;
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
        (*it)->setDebugDataVisible(0);
    m_selected_elements.clear();
}


/** This function is called whenever the user tries to select /
*  unselect a node. If it is in the nodelist, it can be selected.
*  If it is already selected, it is released
*  \param node The node the user clicked on
*/
void SelectionHandler::selectNode(ISceneNode* node)
{
    // we check if it is selected already
    list<ISceneNode*>::Iterator it;
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

// ----------------------------------------------------------------------------
void SelectionHandler::animate(u32 id)
{
    if (m_mouse->leftPressed())
    {
        if (!m_keys->state(CTRL_PRESSED)) clearSelection();

        ISceneNode* node;
        node = Editor::getEditor()->getSceneManager()->getSceneCollisionManager()
            ->getSceneNodeFromScreenCoordinatesBB(
            vector2d<s32>(m_mouse->x, m_mouse->y), id);
        if (node)
            selectNode(node);
    }
} // animate

