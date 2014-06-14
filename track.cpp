#include "track.hpp"
#include "editor.hpp"

#include "toolbox/terrpanel.hpp"

#include <iostream>

Track* Track::m_track          = 0;
int    Track::m_last_entity_ID = MAGIC_NUMBER;

// ----------------------------------------------------------------------------
Track::MouseData::MouseData()
{
    in_view = false;

    wheel = 0;
    left_btn_down = false;
    right_btn_down = false;

    left_pressed = false;
    right_pressed = false;

    left_released = false;
    right_released = false;

    x = 0;
    y = 0;
    prev_x = 0;
    prev_y = 0;
} // MouseData

// ----------------------------------------------------------------------------
void Track::animateNormalCamera(long dt)
{
    vector3df pos;
    if (m_key_state[W_PRESSED] ^ m_key_state[S_PRESSED])
    {
        float sgn = (m_key_state[S_PRESSED]) ? 1.0f : -1.0f;
        pos = m_normal_camera->getPosition();
        pos.Z += sgn * dt / 25.0f;
        m_normal_camera->setPosition(pos);

        pos = m_normal_camera->getTarget();
        pos.Z += sgn * dt / 25.0f;
        m_normal_camera->setTarget(pos);
    };

    if (m_key_state[A_PRESSED] ^ m_key_state[D_PRESSED])
    {
        float sgn = (m_key_state[A_PRESSED]) ? 1.0f : -1.0f;
        pos = m_normal_camera->getPosition();
        pos.X += sgn * dt / 25.0f;
        m_normal_camera->setPosition(pos);

        pos = m_normal_camera->getTarget();
        pos.X += sgn * dt / 25.0f;
        m_normal_camera->setTarget(pos);
    };


    if (m_mouse.wheel != 0)
    {
        pos = m_normal_camera->getPosition();
        pos.Y += -dt * m_mouse.wheel;
        if (pos.Y < 10) pos.Y = 10;
        m_normal_camera->setPosition(pos);
        m_mouse.wheel = 0;
    }

} // animateCamera

// ----------------------------------------------------------------------------
void Track::animateEditing()
{

    if (m_active_cmd)
    {
        if (m_mouse.left_btn_down)
        {
            m_active_cmd->undo();
            m_active_cmd->update((float)-m_mouse.dx(), 0.0f, (float)m_mouse.dy());
            m_active_cmd->redo();
        }
        if (m_mouse.right_btn_down)
        {
            m_active_cmd->undo();
            m_active_cmd->update(0.0f, (float) -m_mouse.dy(), 0.0f);
            m_active_cmd->redo();
        }
        m_mouse.setStorePoint();

        if ((m_mouse.rightPressed() && m_mouse.left_btn_down) ||
            (m_mouse.leftPressed() && m_mouse.right_btn_down))
        {
            // cancel operation
            m_active_cmd->undo();
            delete m_active_cmd;
            m_active_cmd = 0;
            return;
        }
    }

    if (m_mouse.leftReleased() || m_mouse.rightReleased())
    {
        // operation finished - if there was any
        if (m_active_cmd != 0) m_command_handler.add(m_active_cmd);
        m_active_cmd = 0;
    }

    if (m_mouse.leftPressed() || m_mouse.rightPressed())
    {
        // new operation start
        switch (m_state)
        {
        case MOVE:
            m_active_cmd = new MoveCmd(m_entity_manager.getSelection(),
                                       m_key_state[SHIFT_PRESSED]);
            break;
        case ROTATE:
            m_active_cmd = new RotateCmd(m_entity_manager.getSelection(),
                                         m_key_state[SHIFT_PRESSED]);
            break;
        case SCALE:
            m_active_cmd = new ScaleCmd(m_entity_manager.getSelection(),
                                        m_key_state[SHIFT_PRESSED]);
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
        if (!m_key_state[CTRL_PRESSED]) m_entity_manager.clearSelection();

        ISceneNode* node;
        node = Editor::getEditor()->getSceneManager()->getSceneCollisionManager()
            ->getSceneNodeFromScreenCoordinatesBB(
                vector2d<s32>(m_mouse.x, m_mouse.y), MAGIC_NUMBER);
        

        if (node)
            m_entity_manager.selectNode(node);
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


        if (m_mouse.leftPressed())
        {
            m_last_entity_ID++;
            m_new_entity->setID(m_last_entity_ID);
            m_entity_manager.add(m_new_entity);
            std::list<ISceneNode*> list;
            list.push_back(m_new_entity);
            Command* cmd = new CreateCmd(list);
            m_command_handler.add(cmd);
            m_new_entity = m_new_entity->clone();
        }

    }
} // animatePlacing

// ----------------------------------------------------------------------------
void Track::animateTerrainMod(long dt)
{
    if (!m_mouse.in_view) return;

    ISceneCollisionManager* cm;
    cm = Editor::getEditor()->getSceneManager()->getSceneCollisionManager();
    line3d<float> ray;
    ray = cm->getRayFromScreenCoordinates(vector2d<s32>(m_mouse.x, m_mouse.y));
    Terrain::TerrainMod* tm = TerrPanel::getTerrPanel()->getTerrainModData();

    m_terrain->highlight(ray, tm->radius);

    if (m_state == TERRAIN_CUT)
    {
        if (m_mouse.left_btn_down) m_terrain->cut(ray, *tm);
    }

    if (m_state == TERRAIN_MOD)
    {
        if (m_mouse.leftReleased() || m_mouse.rightReleased())
            tm->countdown = -1;

        tm->countdown -= dt;
        if (tm->countdown > 0) return;

        if (m_mouse.leftPressed() || m_mouse.rightPressed())
            tm->ID++;
        if (m_mouse.left_btn_down || m_mouse.right_btn_down)
        {
            tm->countdown = TERRAIN_WAIT_TIME;
            if (m_mouse.right_btn_down) tm->dh *= -1;
            m_terrain->modify(ray, *tm);
            if (m_mouse.right_btn_down) tm->dh *= -1;
        }
    }
} // animateTerrainMod

// ----------------------------------------------------------------------------
Track* Track::getTrack()
{
    if (m_track != 0) return m_track;

    m_track = new Track();
    m_track->init();
    return m_track;
} // getTrack


// ----------------------------------------------------------------------------
void Track::init()
{
    m_active_cmd = 0;
    m_state = SELECT;
    m_new_entity = 0;
    m_grid_on = true;
    for (int i = 0; i < m_key_num; i++) m_key_state[i] = false;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();

    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1);
    m_terrain->setSize(50, 50, 50, 50);

} // init

// ----------------------------------------------------------------------------
void Track::setState(State state)
{
    ISceneManager* scene_manager = Editor::getEditor()->getSceneManager();

    if (m_state == PLACE && state != PLACE)
    {
        m_new_entity->remove();
        m_new_entity = 0;
    }

    if (m_state == FREECAM && state != FREECAM)
    {
        m_state = SELECT;
        m_free_camera->setInputReceiverEnabled(false);
        scene_manager->setActiveCamera(m_normal_camera);
    }
    else if (m_state != FREECAM && state == FREECAM)
    {
        m_free_camera->setInputReceiverEnabled(true);
        scene_manager->setActiveCamera(m_free_camera);
    }

    if (m_state == FREECAM && state == FREECAM)
    {
        m_state = SELECT;
        m_free_camera->setInputReceiverEnabled(false);
        scene_manager->setActiveCamera(m_normal_camera);
    } else m_state = state;

} // setState

// ----------------------------------------------------------------------------
void Track::setGrid(bool grid_on)
{
    m_grid_on = grid_on;
} // setGrid

// ----------------------------------------------------------------------------
void Track::changeGridDensity(int dir)
{

} // changeGridDensity

// ----------------------------------------------------------------------------
void Track::keyEvent(EKEY_CODE code, bool pressed)
{
    switch (code)
    {
    case KEY_KEY_W:
        m_key_state[W_PRESSED] = pressed;
        break;
    case KEY_KEY_A:
        m_key_state[A_PRESSED] = pressed;
        break;
    case KEY_KEY_S:
        m_key_state[S_PRESSED] = pressed;
        break;
    case KEY_KEY_D:
        m_key_state[D_PRESSED] = pressed;
        break;
    case KEY_SHIFT:
    case KEY_LSHIFT:
        m_key_state[SHIFT_PRESSED] = pressed;
        break;
    case KEY_CONTROL:
    case KEY_LCONTROL:
        m_key_state[CTRL_PRESSED] = pressed;
        break;
    default:
        break;
    }
} // keyEvent

// ----------------------------------------------------------------------------
void Track::mouseEvent(const SEvent& e)
{
    dimension2du ss = Editor::getEditor()->getScreenSize();

    // check if mouse is outside of the viewport's domain
    if (e.MouseInput.Y < 50 || e.MouseInput.X > ss.Width - 250)
    {
        if (m_new_entity && m_new_entity->isVisible())
            m_new_entity->setVisible(false);
        m_mouse.in_view = false;
        return;
    }

    m_mouse.in_view = true;
    if (m_new_entity && !m_new_entity->isVisible())
        m_new_entity->setVisible(true);

    switch (e.MouseInput.Event)
    {
    case EMIE_MOUSE_WHEEL:
        m_mouse.wheel = e.MouseInput.Wheel;
        break;
    case EMIE_LMOUSE_PRESSED_DOWN:
        m_mouse.left_btn_down = true;
        m_mouse.left_pressed = true;
        m_mouse.left_released = false;
        break;
    case EMIE_LMOUSE_LEFT_UP:
        m_mouse.left_btn_down = false;
        m_mouse.left_pressed = false;
        m_mouse.left_released = true;
        break;
    case EMIE_RMOUSE_PRESSED_DOWN:
        m_mouse.right_btn_down = true;
        m_mouse.right_pressed = true;
        m_mouse.right_released = false;
        break;
    case EMIE_RMOUSE_LEFT_UP:
        m_mouse.right_btn_down = false;
        m_mouse.right_pressed = false;
        m_mouse.right_released = true;
        break;
    case EMIE_MOUSE_MOVED:
        m_mouse.x = e.MouseInput.X;
        m_mouse.y = e.MouseInput.Y;
        break;
    default:
        break;
    }

} // mouseEvent

// ----------------------------------------------------------------------------
void Track::deleteCmd()
{
    Command* dcmd = new DelCmd(m_entity_manager.getSelection());

    m_entity_manager.clearSelection();
    dcmd->redo();
    m_command_handler.add(dcmd);

} //deleteCmd

// ----------------------------------------------------------------------------
void Track::setNewEntity(const stringw path)
{
    if (m_state != PLACE)
        m_state = PLACE;
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
        animateNormalCamera(dt);
        if (m_state == MOVE || m_state == ROTATE || m_state == SCALE)
        {
            // holding ctrl down will let you select elements in move/rotate state
            if (m_key_state[CTRL_PRESSED]) animateSelection();
            else animateEditing();
        }
        else if (m_state == SELECT)
            animateSelection();
        else if (m_state == PLACE)
            animatePlacing();
        else if (m_state == TERRAIN_MOD || m_state == TERRAIN_CUT)
            animateTerrainMod(dt);
    }

} // animate


// ----------------------------------------------------------------------------
Track::~Track()
{
    if (m_active_cmd) delete m_active_cmd;
    if (m_terrain)    delete m_terrain;
}
