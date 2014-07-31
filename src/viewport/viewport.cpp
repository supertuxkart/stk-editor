#include "viewport/viewport.hpp"
#include "editor.hpp"
#include "track.hpp"

#include "gui/terr_panel.hpp"
#include "gui/road_panel.hpp"

#include "commands/height_mod_cmd.hpp"
#include "commands/texmod_cmd.hpp"
#include "commands/road_cmd.hpp"
#include "commands/create_road_cmd.hpp"
#include "commands/iobject_cmd.hpp"
#include "commands/cl_cmd.hpp"

#include "input/mouse.hpp"
#include "input/keys.hpp"

#include "viewport/selection_handler.hpp"
#include "viewport/aztec_camera.hpp"
#include "viewport/indicator.hpp"

#include "mesh/terrain.hpp"
#include "mesh/sky.hpp"

#include <assert.h>

Viewport* Viewport::m_self           = 0;
int       Viewport::m_last_entity_ID = MAGIC_NUMBER;

// ----------------------------------------------------------------------------
void Viewport::animateSelection()
{
    m_selection_handler->animate(m_spline_mode ? ANOTHER_MAGIC_NUMBER
                                                        : MAGIC_NUMBER);

    // moving?
    if (m_mouse->right_btn_down && (m_mouse->dx() != 0 || m_mouse->dy() != 0))
        switch (m_edit)
        {
            case MOVE:
                move();
                return;
            case ROTATE:
                rotate();
                return;
            case SCALE:
                scale();
                return;
        } // switch

} // animateSelection

// ----------------------------------------------------------------------------
void Viewport::animateEditing()
{
    IObjectCmd* m_active_obj_cmd = dynamic_cast<IObjectCmd*>(m_active_cmd);

    bool dirty = false;

    if (m_mouse->dx() != 0 || m_mouse->dy() != 0)
    {
        m_active_obj_cmd->undo();
        m_active_obj_cmd->update(m_mouse->x,m_mouse->y);
        m_active_obj_cmd->redo();
        dirty = true;
    } // mouse moved

    m_mouse->setStorePoint();

    if (m_mouse->rightPressed())
    {
        // right mouse button cansels the operation
        m_active_cmd->undo();
        delete m_active_cmd;
        m_active_cmd = 0;
        m_state = SELECT;
        dirty = true;
    } // right mouse button pressed
    else if (m_mouse->leftPressed())
    {
        // left mouse finishes the operation
        m_command_handler.add(m_active_cmd);
        m_active_cmd = 0;
        m_state = SELECT;
        dirty = true;
    } // left mouse button clicked

    // road & spline refresh if necessary
    if (dirty && m_spline_mode)
    {
        m_active_road->getSpline()->updatePosition();
        m_active_road->refresh();
    } // road & spline refresh
} // animateEditing

// ----------------------------------------------------------------------------
void Viewport::animatePlacing()
{
    if (m_new_entity)
    {
        ISceneCollisionManager* cm = Editor::getEditor()->getSceneManager()
            ->getSceneCollisionManager();
        line3d<f32> r = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->x, m_mouse->y));

        m_new_entity->setPosition(vector3df(0, 0, 0));
        m_new_entity->updateAbsolutePosition();
        m_new_entity->setPosition(m_terrain->placeBBtoGround(m_new_entity->getTransformedBoundingBox(), r));

        if (m_mouse->leftPressed() && !m_keys->state(SPACE_PRESSED))
        {
            m_last_entity_ID++;
            m_new_entity->setID(m_last_entity_ID);
            m_selection_handler->add(m_new_entity);
            list<ISceneNode*> list;
            list.push_back(m_new_entity);
            IObjectCmd* cmd = new CreateCmd(list);
            m_command_handler.add((ICmd*)cmd);
            m_new_entity = m_new_entity->clone();
        } // m_mouse->leftPressed()
    }
    if (m_mouse->rightPressed())
        setState(SELECT);

} // animatePlacing

// ----------------------------------------------------------------------------
void Viewport::animateSplineEditing()
{
    ISceneCollisionManager* cm = Editor::getEditor()->getSceneManager()
        ->getSceneCollisionManager();
    line3d<f32> r = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->x, m_mouse->y));

    m_junk_node->setPosition(vector3df(0, 0, 0));
    m_junk_node->updateAbsolutePosition();
    vector3df p = m_terrain->placeBBtoGround(m_junk_node->getTransformedBoundingBox(), r);
    p.Y += 1.0;

    if (m_active_cmd)
    {
        dynamic_cast<RoadCmd*>(m_active_cmd)->updatePos(p);
    }

    if (!m_active_cmd||(m_mouse->leftPressed() && !m_keys->state(SPACE_PRESSED)))
    {
        if (m_active_cmd)
            m_command_handler.add(m_active_cmd);
        RoadCmd* rc = new RoadCmd(m_active_road, RoadPanel::getRoadPanel()->isInsertMode());
        rc->updatePos(p);
        m_active_cmd = rc;
    } // m_mouse->leftPressed()

    if (m_mouse->rightPressed())
        setState(SELECT);

} // animateSplineEditing


// ----------------------------------------------------------------------------
void Viewport::animateCheckLine()
{
    ISceneCollisionManager* cm = Editor::getEditor()->getSceneManager()
        ->getSceneCollisionManager();
    line3d<f32> r = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->x, m_mouse->y));

    m_junk_node->setPosition(vector3df(0, 0, 0));
    m_junk_node->updateAbsolutePosition();
    vector3df p = m_terrain->placeBBtoGround(m_junk_node->getTransformedBoundingBox(), r);
    p.Y += 0.5;

    if (m_new_entity) m_new_entity->setPosition(p);

    if (!m_active_cmd || m_mouse->leftPressed())
    {
        if (!m_active_cmd)
        {
            m_new_entity = m_clh.startPlacingNew();
            m_active_cmd = new CLCmd(&m_clh);
        }
        else
        {
            ISceneNode* node = m_clh.place();
            if (node)
            {
                m_new_entity = node;
            }
            else
            {
                m_command_handler.add(m_active_cmd);
                m_active_cmd = 0;
            }
        }
    }   

    if (m_mouse->rightPressed())
        setState(SELECT);
} // animateCheckLine

// ----------------------------------------------------------------------------
void Viewport::animateTerrainMod(long dt)
{
    if (!m_mouse->in_view) return;

    ISceneCollisionManager* cm;
    cm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
    line3d<f32> ray;
    ray = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse->x, m_mouse->y));
    TerrainMod* tm = TerrPanel::getTerrPanel()->getTerrainModData();
    tm->ray = ray;

    m_terrain->highlight(tm);

    if (!m_keys->state(SPACE_PRESSED) &&
        (m_mouse->leftPressed() || m_mouse->rightPressed()))
    {
        // new operation start
        switch (tm->type)
        {
        case HEIGHT_MOD:
            tm->cmd = new HeightModCmd(m_terrain,m_terrain->getNX(), m_terrain->getNZ());
            m_active_cmd = tm->cmd;
            break;
        default:
            tm->cmd = new TexModCmd(m_terrain);
            m_active_cmd = tm->cmd;
            break;
        }
    }

    if (m_mouse->leftReleased() || m_mouse->rightReleased())
    {
        tm->countdown = -1;
        // operation finished - if there was any
        if (m_active_cmd != 0)
        {
            (dynamic_cast<ITerrainCmd*>(m_active_cmd))->finish();
            m_command_handler.add(m_active_cmd);
        }
        m_active_cmd = 0;
    }

    tm->countdown -= dt;
    if (tm->countdown > 0) return;

    if (m_active_cmd && (m_mouse->left_btn_down || m_mouse->right_btn_down))
    {
        tm->countdown = TERRAIN_WAIT_TIME;
        if (m_mouse->right_btn_down) tm->left_click = false;
        else tm->left_click = true;
        m_terrain->modify(tm);
    }

} // animateTerrainMod

// ----------------------------------------------------------------------------
void Viewport::leaveState()
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    switch (m_state)
    {
    case PLACE:
        m_new_entity->remove();
        m_new_entity = 0;
        return;
    case SPLINE:
        if (m_active_cmd)
        {
            m_active_cmd->undo();
            delete m_active_cmd;
            m_active_cmd = 0;
        }
        setSplineMode(false);
        return;
    case FREECAM:
        m_free_camera->setInputReceiverEnabled(false);
        sm->setActiveCamera(m_aztec_cam->Cam());
        if (m_sky) m_sky->hide();
        return;
    case TERRAIN_MOD:
        m_terrain->setHighlightVisibility(false);
        return;
    case CHECK_LINE:
        if (m_active_cmd)
        {
            delete m_active_cmd;
            m_active_cmd = 0;
        }
        if (m_new_entity) m_new_entity = 0;
        m_clh.cancelActive();
        return;
    default:
        break;
    }
 
} // leaveState

// ----------------------------------------------------------------------------
Viewport* Viewport::get(ICameraSceneNode* cam, Mouse* m, Keys* k)
{
    if (m_self != 0) return m_self;

    m_self = new Viewport();
    m_self->init(cam,m,k);
    return m_self;
} // getTrack

// ----------------------------------------------------------------------------
void Viewport::init(ICameraSceneNode* cam = 0, Mouse* m = 0, Keys* k = 0)
{
    m_aztec_cam         = new AztecCamera(cam,m,k);
    m_mouse             = m;
    m_keys              = k;
    m_state             = SELECT;
    m_spline_mode       = false;
    m_active_cmd        = 0;
    m_active_road       = 0;
    m_new_entity        = 0;
    m_new_entity        = 0;
    m_terrain           = 0;
    m_track             = 0;
    m_edit              = MOVE;

    m_selection_handler = new SelectionHandler(m_mouse, m_keys);

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_junk_node = sm->addSphereSceneNode(3);
    m_junk_node->setVisible(false);
} // init

// ----------------------------------------------------------------------------
void Viewport::move()
{
    if (m_state != SELECT || m_selection_handler->getSelection().size() == 0)
        return;

    m_active_cmd = 
        new MoveCmd(m_selection_handler->getSelection(), m_mouse->x, m_mouse->y);
    m_state = EDIT;
} // move

// ----------------------------------------------------------------------------
void Viewport::rotate()
{
    bool swap = false;
    if (m_state == EDIT && m_edit == ROTATE)
    {
        m_active_cmd->undo();
        delete m_active_cmd;
        m_active_cmd = 0;
        swap = true;
    }
    else if (m_state != SELECT || m_selection_handler->getSelection().size() == 0)
        return;

    m_active_cmd = new RotateCmd(m_selection_handler->getSelection(),
                    m_mouse->x, m_mouse->y, m_aztec_cam->getTransformedXdir(),
        m_aztec_cam->getTransformedYdir(), m_aztec_cam->getTransformedZdir());

    if (swap) ((RotateCmd*)m_active_cmd)->swapZMode(m_mouse->x, m_mouse->y);

    m_state = EDIT;
} // rotate

// ----------------------------------------------------------------------------
void Viewport::scale()
{
    if (m_state != SELECT || m_selection_handler->getSelection().size() == 0)
        return;
    m_active_cmd =
        new ScaleCmd(m_selection_handler->getSelection(), m_mouse->x, m_mouse->y);
    m_state = EDIT;
} // scale

// ----------------------------------------------------------------------------
void Viewport::setState(State state)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();

    if (m_state == FREECAM && state == FREECAM) state = SELECT;

    if (state != m_state) leaveState();
    else return;

    if (state == TERRAIN_MOD)
    {
        m_terrain->setHighlightVisibility(true);
    }

    if (state == FREECAM)
    {
        m_free_camera->setInputReceiverEnabled(true);
        sm->setActiveCamera(m_free_camera);
        if (m_sky) m_sky->show();
    }

    if (state == CHECK_LINE && m_active_cmd)
    {
        delete m_active_cmd;
        m_active_cmd = 0;
    }

    m_state = state;

} // setState

// ----------------------------------------------------------------------------
void Viewport::deleteCmd()
{
    if (m_spline_mode)
    {
        list<ISceneNode*> sel = m_selection_handler->getSelectedSpherePoints();
        list<ISceneNode*>::Iterator it;
        u32 ix;
        if (m_active_road->getSpline()->getPointNum() == sel.size())
        {
            CreateRoadCmd* crd = new CreateRoadCmd(m_active_road, true);
            crd->redo();
            m_command_handler.add(crd);
            return;
        } // road is removed
        for (it = sel.begin(); it != sel.end(); it++)
        {
            ix = m_active_road->getSpline()->getCPIndexFromNodeID((*it)->getID());
            if (ix != -1)
            {
                RoadCmd* rcmd = new RoadCmd(m_active_road, true, true);
                rcmd->setIX(ix);
                rcmd->redo();
                m_command_handler.add(rcmd);
            } // if it's a cp
        } // for selected nodes
        m_selection_handler->clearSelection();
        return;
    } // road splines

    if (m_selection_handler->getSelectedSpherePoints().size() > 0)
    {
        m_command_handler.add(
            new CLRCmd(&m_clh, m_selection_handler->getSelectedSpherePoints()));
        
    } // checkline

    if (m_selection_handler->getSelectedObjects().size() > 0)
    {
        IObjectCmd* dcmd = new DelCmd(m_selection_handler->getSelectedObjects());
        dcmd->redo();
        m_command_handler.add(dcmd);
    } // objects
    m_selection_handler->clearSelection();
} //deleteCmd

// ----------------------------------------------------------------------------
void Viewport::setNewEntity(const stringw path, const stringw name)
{
    if (m_state != PLACE)
        setState(PLACE);
    if (m_new_entity != 0)
    {
        m_new_entity->remove();
        m_new_entity = 0;
    }

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_new_entity = sm->addAnimatedMeshSceneNode(sm->getMesh(path));
    if (!m_new_entity)
    {
        std::cerr << "Couldn't open object :(";
        setState(SELECT);
        return;
    }
    m_new_entity->setName(name);
} // setNewEntity

// ----------------------------------------------------------------------------
void Viewport::setNewEntity(ISceneNode* node)
{
    if (m_state != PLACE)
        setState(PLACE);
    if (m_new_entity != 0)
    {
        m_new_entity->remove();
        m_new_entity = 0;
    }

    m_new_entity = node;
} // setNewEntity

// ----------------------------------------------------------------------------
void Viewport::animate(long dt)
{
    if ((!m_track) || (!m_terrain)) return;
    if (m_state != FREECAM)
    {
        m_aztec_cam->animate((f32)dt);
        switch (m_state)
        {
        case EDIT:
            animateEditing();
            return;
        case SELECT:
            animateSelection();
            return;
        case PLACE:
            animatePlacing();
            return;
        case SPLINE:
            animateSplineEditing();
            return;
        case CHECK_LINE:
            animateCheckLine();
            return;
        case TERRAIN_MOD:
            animateTerrainMod(dt);
            return;
        default:
            return;
        }
    }

} // animate

// ----------------------------------------------------------------------------
void Viewport::setSplineMode(bool b)
{
    if (b != m_spline_mode)
        m_selection_handler->clearSelection();
    m_spline_mode = b;
    if (m_active_road)
    {
        m_active_road->getSpline()->setNodeVisibility(m_spline_mode);
        m_active_road->setWireFrame(b);
    }
} // setSplineMode

// ----------------------------------------------------------------------------
void Viewport::setActiveRoad(IRoad* r)
{
    if (m_active_road)
    {
        m_active_road->getSpline()->setNodeVisibility(false);
        m_active_road->setWireFrame(false);
    }
    m_active_road = r;
    r->getSpline()->setNodeVisibility(true);
    r->setWireFrame(true);
    if (!m_spline_mode)
        setSplineMode(true);

    RoadPanel::getRoadPanel()->setActiveRoad(m_track->getRoadID(r));

} // setActiveRoad

// ----------------------------------------------------------------------------
void Viewport::setActiveRoad(u32 id)
{
    setActiveRoad(m_track->getRoadByID(id));
} // setActiveRoad

// ----------------------------------------------------------------------------
void Viewport::roadBorn(IRoad* road)
{
    if (m_active_cmd)
    {
        m_active_cmd->undo();
        delete m_active_cmd;
        m_active_cmd = 0;
    }
    m_command_handler.add(new CreateRoadCmd(road));
    setActiveRoad(road);
    setState(SPLINE);
} // roadBorn

// ----------------------------------------------------------------------------
void Viewport::undo()
{    
    bool respline = splineInterrupt();

    m_command_handler.undo();
    if (respline) setState(SPLINE);

    if (m_spline_mode)
    {
        m_active_road->getSpline()->updatePosition();
        m_active_road->refresh();
    }

} // undo

// ----------------------------------------------------------------------------
void Viewport::redo()
{
    bool respline = splineInterrupt();
    m_command_handler.redo();
    if (respline) setState(SPLINE);
    if (m_spline_mode)
    {
        m_active_road->getSpline()->updatePosition();
        m_active_road->refresh();
    }
} // redo

// ----------------------------------------------------------------------------
bool Viewport::splineInterrupt()
{
    if (m_state == SPLINE && m_active_cmd)
    {
        m_active_cmd->undo();
        delete m_active_cmd;
        m_active_cmd = 0;
        setState(SELECT);
        return true;
    }
    return false;
} // splineInterrupt

// ----------------------------------------------------------------------------
void Viewport::loseFocus()
{
    if (m_new_entity) m_new_entity->setVisible(false);
}

// ----------------------------------------------------------------------------
void Viewport::gainFocus()
{
    if (m_new_entity) m_new_entity->setVisible(true);
}

// ----------------------------------------------------------------------------
bool Viewport::setTrack(Track* t)
{
    if (t->isValid())
    {
        m_track = t;
        m_terrain = m_track->getTerrain();
        m_aztec_cam->init(m_terrain->getSizeX(), m_terrain->getSizeZ());
        
        for (int i = 0; t->getRoadByID(i); i++)
            setActiveRoad(i);

        setActiveRoad((u32)0);
        return true;
    }
    return false;
} // setTrack

// ----------------------------------------------------------------------------
void Viewport::build()
{
    m_track->build();
} // build

void Viewport::clear()
{
    m_selection_handler->clearSelection();
    m_command_handler.clear();
    setState(SELECT);
    m_last_entity_ID = MAGIC_NUMBER;
    if (m_track)
    {
        m_track->quit();
        delete m_track;
        m_track = 0;
    }

} // clear

// ----------------------------------------------------------------------------
void Viewport::draw()
{
    m_clh.draw();
} // draw

// ----------------------------------------------------------------------------
void Viewport::restoreCam()
{
    if (m_state != FREECAM)
        m_aztec_cam->restore();
} // restoreCam

// ----------------------------------------------------------------------------
Indicator*  Viewport::getIndicator()
{
    return m_aztec_cam->getIndicator();
} // Indicator

// ----------------------------------------------------------------------------
void Viewport::showSky()
{
    if (m_sky && m_state == FREECAM) 
        m_sky->show();
} // showSky

// ----------------------------------------------------------------------------
void Viewport::setSky(Sky* s)
{
    m_sky = s;
    TerrPanel::getTerrPanel()->refreshSkyButtons(s);
} // setSky

// ----------------------------------------------------------------------------
Viewport::~Viewport()
{
    if (m_active_cmd) delete m_active_cmd;
}
