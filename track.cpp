#include "track.hpp"
#include "editor.hpp"

#include <iostream>

Track* Track::m_track = 0;

// ----------------------------------------------------------------------------
Track::MouseData::MouseData()
{
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
}

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


    if (m_mouse_data.wheel != 0)
    {
        pos = m_normal_camera->getPosition();
        pos.Y += -dt * m_mouse_data.wheel;
        if (pos.Y < 10) pos.Y = 10;
        m_normal_camera->setPosition(pos);
        m_mouse_data.wheel = 0;
    }

} // animateCamera

// ----------------------------------------------------------------------------
void Track::animateEditing()
{

    if (m_active_cmd)
    {
        if (m_mouse_data.left_btn_down)
        {
            m_active_cmd->undo();
            m_active_cmd->update(-m_mouse_data.dx(), 0, m_mouse_data.dy());
            m_active_cmd->redo();
        }
        if (m_mouse_data.right_btn_down)
        {
            m_active_cmd->undo();
            m_active_cmd->update(0, -m_mouse_data.dy(), 0);
            m_active_cmd->redo();
        }
        m_mouse_data.setStorePoint();

        if ((m_mouse_data.rightPressed() && m_mouse_data.left_btn_down) ||
            (m_mouse_data.leftPressed() && m_mouse_data.right_btn_down))
        {
            // cancel operation
            m_active_cmd->undo();
            delete m_active_cmd;
            m_active_cmd = 0;
            return;
        }
    } 

    if (m_mouse_data.leftReleased() || m_mouse_data.rightReleased())
    {
        // operation finished - if there was any
        if (m_active_cmd != 0) m_command_handler.add(m_active_cmd);
        m_active_cmd = 0;
    }

    if (m_mouse_data.leftPressed() || m_mouse_data.rightPressed())
    {
        // new operation start
        if (m_state == MOVE) m_active_cmd = new MoveCmd(m_entity_manager.getSelection());
        else m_active_cmd = new RotateCmd(m_entity_manager.getSelection());
        m_mouse_data.setStorePoint();
    }

}

// ----------------------------------------------------------------------------
void Track::animateSelection()
{
    if (m_mouse_data.leftPressed())
    {
        if (!m_key_state[CTRL_PRESSED]) m_entity_manager.clearSelection();
        
        m_entity_manager.selectNode(
            Editor::getEditor()->getSceneManager()->getSceneCollisionManager()
                ->getSceneNodeFromScreenCoordinatesBB(
                    vector2d<s32>(m_mouse_data.x, m_mouse_data.y
            )));
    }
}

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
    m_grid_on = true;
    for (int i = 0; i < m_key_num; i++) m_key_state[i] = false;

    ISceneNode* node;

    for (int i = 1; i < 10; i++)
    {
        node = Editor::getEditor()->getSceneManager()->addCubeSceneNode();

        node->setID(i);
        m_entity_manager.add(node);
    }

} // init

// ----------------------------------------------------------------------------
void Track::setState(State state)
{
    ISceneManager* scene_manager = Editor::getEditor()->getSceneManager();
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

    m_state = state;

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
    switch (e.MouseInput.Event)
    {
    case EMIE_MOUSE_WHEEL:
        m_mouse_data.wheel = e.MouseInput.Wheel;
        break;
    case EMIE_LMOUSE_PRESSED_DOWN:
        m_mouse_data.left_btn_down  = true;
        m_mouse_data.left_pressed   = true;
        m_mouse_data.left_released  = false;
        break;
    case EMIE_LMOUSE_LEFT_UP:
        m_mouse_data.left_btn_down  = false;
        m_mouse_data.left_pressed   = false;
        m_mouse_data.left_released  = true;
        break;
    case EMIE_RMOUSE_PRESSED_DOWN:
        m_mouse_data.right_btn_down = true;
        m_mouse_data.right_pressed  = true;
        m_mouse_data.right_released = false;
        break;
    case EMIE_RMOUSE_LEFT_UP:
        m_mouse_data.right_btn_down = false;
        m_mouse_data.right_pressed  = false;
        m_mouse_data.right_released = true;
        break;
    case EMIE_MOUSE_MOVED:
        m_mouse_data.x              = e.MouseInput.X;
        m_mouse_data.y              = e.MouseInput.Y;
        break;
    default:
        break;
    }

} // mouseEvent

// ----------------------------------------------------------------------------
void Track::animate(long dt)
{
    if (m_state != FREECAM)
    {
        animateNormalCamera(dt);
        if (m_state == MOVE || m_state == ROTATE)
        {
            // holding ctrl down will let you select elements in move/rotate state
            if (m_key_state[CTRL_PRESSED]) animateSelection();
            else animateEditing();
        }
        else if (m_state == SELECT)
        {
            animateSelection();
        }
    }


} // animate
