#ifndef TRACK_HPP
#define TRACK_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;

class Track
{
public:
    enum State
    {
        SELECT,
        PLACE,
        MOVE,
        ROTATE,
        FREECAM
    };
    enum Key
    {
        W_PRESSED,
        A_PRESSED,
        S_PRESSED,
        D_PRESSED
    };
private:
    static Track*       m_track;
    static const int    m_key_num = 4;

    State               m_state;

    // camera used for editing
    // position has to be uploaded manually
    ICameraSceneNode*   m_normal_camera;

    bool                m_key_state[m_key_num];
    bool                m_grid_on;
    int                 m_mouse_wheel_state;

    Track() {};
    void                animateNormalCamera(long dt);

public:
    static Track* getTrack();
    void          init();
    void          setState(State state);
    void          setGrid(bool grid_on);
    void          changeGridDensity(int dir);
    void          keyEvent(EKEY_CODE code, bool pressed);
    void          animate(long dt);

    void          setNormalCamera(ICameraSceneNode* cam) { m_normal_camera     = cam; }
    void          setMouseWheelState(int ms)             { m_mouse_wheel_state = ms;  }

    bool          isGridOn()        { return m_grid_on; }
};

#endif
