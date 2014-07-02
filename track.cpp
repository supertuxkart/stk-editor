#include "track.hpp"
#include "editor.hpp"

#include "toolbox/terrpanel.hpp"
#include "toolbox/roadpanel.hpp"

#include "commands/heightmodcmd.hpp"
#include "commands/texmodcmd.hpp"
#include "commands/roadcommand.hpp"
#include "commands/rccommand.hpp"

#include <assert.h>
#include <iostream>

Track* Track::m_track          = 0;
int    Track::m_last_entity_ID = MAGIC_NUMBER;

// ----------------------------------------------------------------------------
void Track::animateNormalCamera(f32 dt)
{    
    if (m_keys.state(W_PRESSED) ^ m_keys.state(S_PRESSED))
    {
        float sgn = m_keys.state(S_PRESSED) ? 1.0f : -1.0f;
        vector3df pos = m_normal_camera->getPosition();
        vector3df tar = m_normal_camera->getTarget();
        vector3df transformed_z_dir = getTransformedZdir();

        pos += transformed_z_dir * sgn * dt / 20.0f;
        m_normal_camera->setPosition(pos);
        
        tar += transformed_z_dir * sgn * dt / 20.0f;
        m_normal_camera->setTarget(tar);
        
        m_indicator->updatePos(pos, tar);
    };

    if (m_keys.state(A_PRESSED) ^ m_keys.state(D_PRESSED))
    {
        float sgn = m_keys.state(D_PRESSED) ? 1.0f : -1.0f;
        vector3df pos = m_normal_camera->getPosition();
        vector3df tar = m_normal_camera->getTarget();
        vector3df transformed_x_dir = getTransformedXdir();

        pos += transformed_x_dir * sgn * dt / 20.0f;
        m_normal_camera->setPosition(pos);

        tar += transformed_x_dir * sgn * dt / 20.0f;
        m_normal_camera->setTarget(tar);

        m_indicator->updatePos(pos, tar);
    };
    
    if (m_mouse.wheel != 0)
    {
        dimension2du ss = Editor::getEditor()->getScreenSize();
        m_normal_cd -= m_mouse.wheel * 10.0f;
        if (m_normal_cd < 8) m_normal_cd = 8;
        matrix4 mat;

        f32 nv   = m_normal_camera->getNearValue();
        f32 fv   = m_normal_camera->getFarValue();
        f32 hVol = m_normal_cd * ss.Height / ss.Width;

        mat.buildProjectionMatrixOrthoLH(m_normal_cd, hVol, nv, fv);
        m_normal_camera->setProjectionMatrix(mat, true);        
        m_mouse.wheel = 0;

        m_indicator->setProjMat(m_normal_cd, hVol, nv, fv);
    }

    if (m_keys.state(SPACE_PRESSED) && m_active_cmd == 0)
    {
        if (m_mouse.left_btn_down)
        {
            vector3df tar = m_normal_camera->getTarget();
            tar.rotateXZBy(m_mouse.dx() / 5.0f, m_normal_camera->getPosition());
            m_normal_camera->setTarget(tar);
            m_indicator->updateTar(tar);
        }

        if (m_mouse.right_btn_down)
        {
            vector3df pos = m_normal_camera->getPosition();
            vector3df tar = m_normal_camera->getTarget();
            vector3df transformed_z_dir = vector3df(pos.X - tar.X, 0, pos.Z - tar.Z);
            transformed_z_dir.normalize();
            tar += transformed_z_dir * (f32) m_mouse.dy();
            m_normal_camera->setTarget(tar);
            m_indicator->updateTar(tar);
        }

        m_mouse.setStorePoint();
    }

} // animateCamera

// ----------------------------------------------------------------------------
void Track::animateEditing()
{

    if (m_spline_mode && m_selection_handler.getSelection().size() == 0)
        m_selection_handler.selectNode(m_active_road->getSpline());

    if (m_active_cmd)
    {
        IOCommand* m_active_obj_cmd = dynamic_cast<IOCommand*>(m_active_cmd);

        if (m_mouse.left_btn_down)
        {
            m_active_obj_cmd->undo();
            vector3df v = getTransformedXdir() * m_mouse.dx() + 
                          getTransformedZdir() * m_mouse.dy();
            m_active_obj_cmd->update(v.X,v.Y,v.Z);
            m_active_obj_cmd->redo();
            if (m_spline_mode)
            {
                m_active_road->getSpline()->updatePosition();
                m_active_road->refresh();
            }

        }
        if (m_mouse.right_btn_down)
        {
            m_active_obj_cmd->undo();
            m_active_obj_cmd->update(0.0f, (float)-m_mouse.dy(), 0.0f);
            m_active_obj_cmd->redo();
            if (m_spline_mode)
            {
                m_active_road->getSpline()->updatePosition();
                m_active_road->refresh();
            }
        }
        m_mouse.setStorePoint();

        if ((m_mouse.rightPressed() && m_mouse.left_btn_down) ||
            (m_mouse.leftPressed() && m_mouse.right_btn_down))
        {
            // cancel operation
            m_active_obj_cmd->undo();
            delete m_active_obj_cmd;
            m_active_cmd = 0;
            m_active_road->getSpline()->updatePosition();
            m_active_road->refresh();
            return;
        }
    }

    if (m_mouse.leftReleased() || m_mouse.rightReleased())
    {
        // operation finished - if there was any
        if (m_active_cmd != 0) m_command_handler.add(m_active_cmd);
        m_active_cmd = 0;
    }

    if ((m_mouse.leftPressed() || m_mouse.rightPressed()) && !m_keys.state(SPACE_PRESSED))
    {
        // new operation start
        switch (m_state)
        {
        case MOVE:
            m_active_cmd = new MoveCmd(m_selection_handler.getSelection(),
                                       m_keys.state(SHIFT_PRESSED));
            break;
        case ROTATE:
            m_active_cmd = new RotateCmd(m_selection_handler.getSelection(),
                                         m_keys.state(SHIFT_PRESSED));
            break;
        case SCALE:
            m_active_cmd = new ScaleCmd(m_selection_handler.getSelection(),
                                        m_keys.state(SHIFT_PRESSED));
            break;
        default:
            break;
        }
        m_mouse.setStorePoint();
    }

} // animateEditing

// ----------------------------------------------------------------------------
void Track::animateSelection()
{
    if (m_mouse.leftPressed())
    {
        if (!m_keys.state(CTRL_PRESSED)) m_selection_handler.clearSelection();
        int id = (m_spline_mode) ? ANOTHER_MAGIC_NUMBER : MAGIC_NUMBER;

        ISceneNode* node;
        node = Editor::getEditor()->getSceneManager()->getSceneCollisionManager()
            ->getSceneNodeFromScreenCoordinatesBB(
                vector2d<s32>(m_mouse.x, m_mouse.y), id);
        if (node)
            m_selection_handler.selectNode(node);
    }
} // animateSelection

// ----------------------------------------------------------------------------
void Track::animatePlacing()
{
    if (m_new_entity)
    {
        ISceneCollisionManager* cm = Editor::getEditor()->getSceneManager()
            ->getSceneCollisionManager();
        line3d<f32> r = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse.x, m_mouse.y));

        m_new_entity->setPosition(vector3df(0, 0, 0));
        m_new_entity->updateAbsolutePosition();
        m_new_entity->setPosition(m_terrain->placeBBtoGround(m_new_entity->getTransformedBoundingBox(), r));

        if (m_mouse.leftPressed() && !m_keys.state(SPACE_PRESSED))
        {
            m_last_entity_ID++;
            m_new_entity->setID(m_last_entity_ID);
            m_selection_handler.add(m_new_entity);
            list<ISceneNode*> list;
            list.push_back(m_new_entity);
            IOCommand* cmd = new CreateCmd(list);
            m_command_handler.add((ICommand*)cmd);
            m_new_entity = m_new_entity->clone();
        } // m_mouse.leftPressed()
    }
} // animatePlacing

// ----------------------------------------------------------------------------
void Track::animateSplineEditing()
{

    ISceneCollisionManager* cm = Editor::getEditor()->getSceneManager()
        ->getSceneCollisionManager();
    line3d<f32> r = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse.x, m_mouse.y));

    m_junk_node->setPosition(vector3df(0, 0, 0));
    m_junk_node->updateAbsolutePosition();
    vector3df p = m_terrain->placeBBtoGround(m_junk_node->getTransformedBoundingBox(), r);
    p.Y += 1.0;

    if (m_active_cmd)
    {
        dynamic_cast<RoadCommand*>(m_active_cmd)->updatePos(p);
    }
    

    if (!m_active_cmd||(m_mouse.leftPressed() && !m_keys.state(SPACE_PRESSED)))
    {
        if (m_active_cmd)
            m_command_handler.add(m_active_cmd);
        RoadCommand* rc = new RoadCommand(m_active_road, RoadPanel::getRoadPanel()->isInsertMode());
        rc->updatePos(p);
        m_active_cmd = rc;
    } // m_mouse.leftPressed()

    if (m_mouse.rightPressed())
    {
        if (m_active_cmd)
        {
            m_active_cmd->undo();
            delete m_active_cmd;
            m_active_cmd = 0;
        }
        setState(SELECT);
    }


} // animateSplineEditing

// ----------------------------------------------------------------------------
void Track::animateTerrainMod(long dt)
{
    if (!m_mouse.in_view) return;

    ISceneCollisionManager* cm;
    cm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
    line3d<float> ray;
    ray = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse.x, m_mouse.y));
    TerrainMod* tm = TerrPanel::getTerrPanel()->getTerrainModData();
    tm->ray = ray;

    m_terrain->highlight(tm);


    if (!m_keys.state(SPACE_PRESSED) &&
        (m_mouse.leftPressed() || m_mouse.rightPressed()))
    {
        // new operation start
        switch (tm->type)
        {
        case HEIGHT_MOD:
            tm->cmd = new HeightModCmd(m_terrain,m_terrain->m_nx, m_terrain->m_nz);
            m_active_cmd = tm->cmd;
            break;
        default:
            tm->cmd = new TexModCmd(m_terrain);
            m_active_cmd = tm->cmd;
            break;
        }
    }

    if (m_mouse.leftReleased() || m_mouse.rightReleased())
    {
        tm->countdown = -1;
        // operation finished - if there was any
        if (m_active_cmd != 0)
        {
            (dynamic_cast<ITCommand*>(m_active_cmd))->finish();
            m_command_handler.add(m_active_cmd);
        }
        m_active_cmd = 0;
    }

    tm->countdown -= dt;
    if (tm->countdown > 0) return;

    if (m_active_cmd && (m_mouse.left_btn_down || m_mouse.right_btn_down))
    {
        tm->countdown = TERRAIN_WAIT_TIME;
        if (m_mouse.right_btn_down) tm->left_click = false;
        else tm->left_click = true;
        m_terrain->modify(tm);
    }

    return;

} // animateTerrainMod

// ----------------------------------------------------------------------------
void Track::leaveState()
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    switch (m_state)
    {
    case PLACE:
        m_new_entity->remove();
        m_new_entity = 0;
        return;
    case SPLINE:
        return;
    case FREECAM:
        m_free_camera->setInputReceiverEnabled(false);
        sm->setActiveCamera(m_normal_camera);
        return;
    case TERRAIN_MOD:
        m_terrain->setHighlightVisibility(false);
        return;
    default:
        break;
    }


} // leaveState

// ----------------------------------------------------------------------------
vector3df Track::getTransformedXdir()
{
    vector3df transformed_z_dir = getTransformedZdir();
    transformed_z_dir.rotateXZBy(90);

    return transformed_z_dir;

} // getTransformedXdir

// ----------------------------------------------------------------------------
vector3df Track::getTransformedZdir()
{
    vector3df pos = m_normal_camera->getPosition();
    vector3df tar = m_normal_camera->getTarget();
    vector3df transformed_z_dir = vector3df(pos.X - tar.X, 0, pos.Z - tar.Z);
    transformed_z_dir.normalize();
    return transformed_z_dir;
} // getTransformedZdir

// ----------------------------------------------------------------------------
Track* Track::getTrack(ICameraSceneNode* cam)
{
    if (m_track != 0) return m_track;

    m_track = new Track();
    m_track->init(cam);
    return m_track;
} // getTrack


// ----------------------------------------------------------------------------
void Track::init(ICameraSceneNode* cam)
{
    m_normal_camera   = cam;
    m_active_cmd  = 0;
    m_active_road     = 0;
    m_state           = SELECT;
    m_spline_mode     = false;
    m_new_entity      = 0;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();

    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, 50, 50, 100, 100);
    
    m_junk_node = sm->addSphereSceneNode(3);
    m_junk_node->setVisible(false);

    dimension2du ss = Editor::getEditor()->getScreenSize();
    matrix4 mat;
    m_normal_cd = 50.0f;

    f32 nv = m_normal_camera->getNearValue();
    f32 fv = m_normal_camera->getFarValue();
    f32 hVol = m_normal_cd * ss.Height / ss.Width;

    mat.buildProjectionMatrixOrthoLH(m_normal_cd, hVol,nv,fv);

    m_normal_camera->setProjectionMatrix(mat,true);

    m_indicator = new Indicator(m_normal_camera->getPosition(), 
                                m_normal_camera->getTarget(),
                                m_normal_cd, hVol, nv, fv);

} // init

// ----------------------------------------------------------------------------
void Track::setState(State state)
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
    }

    m_state = state;

} // setState

// ----------------------------------------------------------------------------
void Track::keyEvent(EKEY_CODE code, bool pressed)
{
    m_keys.keyEvent(code, pressed);
} // keyEvent

// ----------------------------------------------------------------------------
void Track::mouseEvent(const SEvent& e)
{
    dimension2du ss = Editor::getEditor()->getScreenSize();

    // check if mouse is outside of the viewport's domain
    if (e.MouseInput.Y < 50 || e.MouseInput.X > (s32) ss.Width - 250)
    {
        if (m_new_entity && m_new_entity->isVisible())
            m_new_entity->setVisible(false);
        m_mouse.in_view = false;
        return;
    }

    m_mouse.in_view = true;
    if (m_new_entity && !m_new_entity->isVisible())
        m_new_entity->setVisible(true);

    m_mouse.refresh(e);

} // mouseEvent

// ----------------------------------------------------------------------------
void Track::deleteCmd()
{
    IOCommand* dcmd = new DelCmd(m_selection_handler.getSelection());

    m_selection_handler.clearSelection();
    dcmd->redo();
    m_command_handler.add((ICommand*)dcmd);

} //deleteCmd

// ----------------------------------------------------------------------------
void Track::setNewEntity(const stringw path)
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

} // setNewEntity

// ----------------------------------------------------------------------------
void Track::animate(long dt)
{
    if (m_state != FREECAM)
    {
        animateNormalCamera((f32)dt);
        switch (m_state)
        {
        case MOVE:
        case ROTATE:
        case SCALE:
            // holding ctrl down will let you select elements in move/rotate state
            if (m_keys.state(CTRL_PRESSED)) animateSelection();
            else animateEditing();
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
        case TERRAIN_MOD:
            animateTerrainMod(dt);
            return;
        default:
            return;
        }
    }

} // animate

// ----------------------------------------------------------------------------
void Track::setSplineMode(bool b)
{
    if (b != m_spline_mode)
        m_selection_handler.clearSelection();
    m_spline_mode = b;
    m_active_road->getSpline()->setNodeVisibility(m_spline_mode);
} // setSplineMode


void Track::setActiveRoad(IRoad* r)
{
    if (m_active_road)
    {
        m_active_road->getSpline()->setNodeVisibility(false);
    }
    m_active_road = r;
    r->getSpline()->setNodeVisibility(true);
    if (!m_spline_mode)
        setSplineMode(true);
} // setActiveRoad

// ----------------------------------------------------------------------------
void Track::roadBorn(IRoad* road, stringw name)
{
    if (m_active_cmd)
    {
        m_active_cmd->undo();
        delete m_active_cmd;
        m_active_cmd = 0;
    }
    m_command_handler.add(new RCCommand(road, name));
    m_active_road = road;
    setState(SPLINE);
} // roadBorn

// ----------------------------------------------------------------------------
void Track::undo()
{
    m_command_handler.undo();
    if (m_spline_mode)
    {
        m_active_road->getSpline()->updatePosition();
        m_active_road->refresh();
    }
} // undo

// ----------------------------------------------------------------------------

void Track::redo()
{
    m_command_handler.redo();
    if (m_spline_mode)
    {
        m_active_road->getSpline()->updatePosition();
        m_active_road->refresh();
    }
} // redo

// ----------------------------------------------------------------------------
Track::~Track()
{
    if (m_active_cmd) delete m_active_cmd;
    if (m_terrain)    delete m_terrain;
}

