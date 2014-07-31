#include "viewport/selection_handler.hpp"

#include "editor.hpp"

#include "input/keys.hpp"
#include "input/mouse.hpp"

#include "mesh/road.hpp"
#include "viewport/viewport.hpp"

// ----------------------------------------------------------------------------
bool SelectionHandler::firstIsCloser(vector3df v1, vector3df v2)
{
    vector3df pos = Viewport::get()->getAztecPos();

    if ((pos - v1).getLength() < (pos - v2).getLength()) return true;

    return false;
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
void SelectionHandler::selectRoad(IRoad* r)
{
    m_selected_elements.clear();
    ISpline* spline = r->getSpline();
    u32 ix = 0;
    ISceneNode* n;
    while ((n = spline->getNode(ix)))
    {
        m_selected_elements.push_back(n);
        n->setDebugDataVisible(EDS_BBOX);
        ix++;
    }

} // selectNode

// ----------------------------------------------------------------------------
void SelectionHandler::animate(u32 id)
{
    if (m_mouse->leftPressed())
    {
        if (!m_keys->state(SHIFT_PRESSED)) clearSelection();

        ISceneCollisionManager* iscm;
        iscm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
        line3d<f32> ray;
        ray = iscm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->x, m_mouse->y));

        ISceneNode* selected_road = 0;
        u32 ix = 1;
        vector3df   cp_end;
        ISceneNode* road_node;
        while ((road_node = (Road*)Viewport::get()->getTrack()->getRoadByID(ix)))
        {
            ix++;
            vector3df   cp;
            triangle3df ot;
            if (iscm->getCollisionPoint(ray, road_node->getTriangleSelector(), cp, ot, road_node))
            {
                if (!selected_road)
                {
                    selected_road = road_node;
                    cp_end = cp;
                }
                else selected_road = firstIsCloser(cp, cp_end) ? road_node : selected_road;
            }
        } // while road

        ISceneNode* node;
        node = iscm->getSceneNodeFromScreenCoordinatesBB(
                                       vector2d<s32>(m_mouse->x, m_mouse->y), MAGIC_NUMBER);
        /*
        if (node && selected_road)
            node = firstIsCloser(node->getPosition(), cp_end) ? node : selected_road;
        */
        if ((node && (node == selected_road)) || (!node && selected_road))
        {
            selectRoad((IRoad*)selected_road);
            Viewport::get()->setActiveRoad((Road*)selected_road);   
        } // road is selected
        else if (node)
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
list<ISceneNode*> SelectionHandler::getSelectedSpherePoints()
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
