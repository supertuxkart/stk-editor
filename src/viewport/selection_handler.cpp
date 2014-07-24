#include "viewport/selection_handler.hpp"

#include "editor.hpp"

#include "input/keys.hpp"
#include "input/mouse.hpp"

#include "mesh/road.hpp"
#include "viewport/viewport.hpp"

// ----------------------------------------------------------------------------
ISceneNode* SelectionHandler::closerToCamera(ISceneNode* n1, ISceneNode* n2)
{
    vector3df pos = Viewport::get()->getAztecPos();

    if ((pos - n1->getPosition()).getLength() <
        (pos - n2->getPosition()).getLength())
        return n1;
    
    return n2;
} // closerToCamera

// ----------------------------------------------------------------------------
void SelectionHandler::clearSelection()
{
    list<ISceneNode*>::Iterator it;
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
        (*it)->setDebugDataVisible(0);
    m_selected_elements.clear();
} // clearSelection

// ----------------------------------------------------------------------------
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
} // selectNode

// ----------------------------------------------------------------------------
void SelectionHandler::animate(u32 id)
{
    if (m_mouse->leftPressed())
    {
        if (!m_keys->state(CTRL_PRESSED)) clearSelection();

        ISceneCollisionManager* iscm;
        iscm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
        line3d<f32> ray;
        ray = iscm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->x, m_mouse->y));

        ISceneNode* selected_road = 0;
        u32 ix = 1;
        ISceneNode* road_node;
        while (road_node = (Road*)Viewport::get()->getTrack()->getRoadByID(ix))
        {
            ix++;
            vector3df   cp;
            triangle3df ot;
            if (iscm->getCollisionPoint(ray, road_node->getTriangleSelector(), cp, ot, road_node))
            {
                if (!selected_road) selected_road = road_node;
                else selected_road = closerToCamera(road_node, selected_road);
            }
        } // while road

        ISceneNode* node;
        node = iscm->getSceneNodeFromScreenCoordinatesBB(
                                       vector2d<s32>(m_mouse->x, m_mouse->y), MAGIC_NUMBER);
        if (node && selected_road)
            node = closerToCamera(node, selected_road);
        if ((node && (node == selected_road)) || selected_road)
            Viewport::get()->setActiveRoad((Road*)selected_road);
        if (node)
        {
            if (node->getID() < ANOTHER_MAGIC_NUMBER && id == ANOTHER_MAGIC_NUMBER)
            {
                Viewport::get()->setSplineMode(false);
            }
            selectNode(node);
        }
    } // left mouse button pressed
} // animate


// ----------------------------------------------------------------------------
list<ISceneNode*> SelectionHandler::getSelectedSplinePoints()
{
    list<ISceneNode*> sel;
    list<ISceneNode*>::Iterator it;
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
        if ((*it)->getID() >= ANOTHER_MAGIC_NUMBER) sel.push_back(*it);
    return sel;
} // getSelectedSplinePoints

// ----------------------------------------------------------------------------
list<ISceneNode*> SelectionHandler::getSelectedObjects()
{
    list<ISceneNode*> sel;
    list<ISceneNode*>::Iterator it;
    for (it = m_selected_elements.begin(); it != m_selected_elements.end(); it++)
        if ((*it)->getID() < ANOTHER_MAGIC_NUMBER) sel.push_back(*it);
    return sel;
} // getSelectedObjects
