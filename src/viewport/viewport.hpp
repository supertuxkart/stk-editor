#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "commands/command_handler.hpp"

#include <irrlicht.h>

class  Track;
class  Terrain;
class  IRoad;
class  SelectionHandler;
class  AztecCamera;
class  Indicator;
struct Mouse;
struct Keys;

using namespace irr;
using namespace scene;
using namespace core;

class Viewport
{
public:
    enum State
    {
        SELECT,
        MOVE,
        ROTATE,
        SCALE,
        PLACE,
        SPLINE,
        TERRAIN_MOD,
        FREECAM
    };

private:
    static Viewport*        m_self;

    State                   m_state;

    Track*                  m_track;

    Mouse*                  m_mouse;
    Keys*                   m_keys;

    SelectionHandler*       m_selection_handler;
    CommandHandler          m_command_handler;

    ISceneNode*             m_new_entity;
    static int              m_last_entity_ID;

    bool                    m_spline_mode;

    IRoad*                  m_active_road;
    ISceneNode*             m_junk_node;


    // command not yet finished
    ICmd*                   m_active_cmd;

    AztecCamera*            m_aztec_cam;
    ICameraSceneNode*       m_free_camera;

    Terrain*                m_terrain;
    static const long       TERRAIN_WAIT_TIME = 50;

    Viewport() {};

    void                    animateEditing();
    void                    animatePlacing();
    void                    animateSplineEditing();
    void                    animateTerrainMod(long dt);

    void                    leaveState();

public:
    static Viewport*    get(ICameraSceneNode* cam = 0, Mouse* m = 0, Keys* k = 0);
    void                init(ICameraSceneNode* cam, Mouse* m, Keys* k);
    void                setState(State state);
    void                deleteCmd();
    void                setNewEntity(const stringw path);
    void                animate(long dt);
    void                setSplineMode(bool b);
    void                setActiveRoad(IRoad* r);
    void                roadBorn(IRoad* road, stringw name);
    void                undo();
    void                redo();
    void                looseFocus();
    void                gainFocus();
    void                setTrack(Track* t);

    Indicator*          getIndicator();

    void                setFreeCamera(ICameraSceneNode* cam) { m_free_camera = cam;  }
    bool                getSplineMode()                      { return m_spline_mode; }

    ~Viewport();

};

#endif
