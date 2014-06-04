#include "track.hpp"
#include "editor.hpp"

Track* Track::m_track = 0;

// ----------------------------------------------------------------------------
void Track::animateNormalCamera(long dt)
{
    vector3df pos;
    if (m_key_state[W_PRESSED] ^ m_key_state[S_PRESSED])
    {
        float sgn = (m_key_state[S_PRESSED]) ? 1.0 : -1.0;
        pos = m_normal_camera->getPosition();
        pos.Z += sgn * dt / 25.0;
        m_normal_camera->setPosition(pos);

        pos = m_normal_camera->getTarget();
        pos.Z += sgn * dt / 25.0;
        m_normal_camera->setTarget(pos);
    };

    if (m_key_state[A_PRESSED] ^ m_key_state[D_PRESSED])
    {
        float sgn = (m_key_state[A_PRESSED]) ? 1.0 : -1.0;
        pos = m_normal_camera->getPosition();
        pos.X += sgn * dt / 25.0;
        m_normal_camera->setPosition(pos);

        pos = m_normal_camera->getTarget();
        pos.X += sgn * dt / 25.0;
        m_normal_camera->setTarget(pos);
    };


    if (m_mouse_wheel_state != 0)
    {
        pos = m_normal_camera->getPosition();
        pos.Y += - dt * m_mouse_wheel_state;
        if (pos.Y < 10) pos.Y = 10;
        m_normal_camera->setPosition(pos);
        m_mouse_wheel_state = 0;
    }


} // animateCamera

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
    m_state = SELECT;
    m_grid_on = true;
    for (int i = 0; i < m_key_num; i++) m_key_state[i] = false;
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
    default:
        break;
    }
} // keyEvent

// ----------------------------------------------------------------------------
void Track::animate(long dt)
{
    if ( m_state != FREECAM)
        animateNormalCamera(dt);
} // animate
