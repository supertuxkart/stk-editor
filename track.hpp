#ifndef TRACK_HPP
#define TRACK_HPP

#include "commands/commandhandler.hpp"

#include <irrlicht.h>

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

class Track
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
    static Track*           m_track;
    
    static int              m_last_entity_ID;

    State                   m_state;

    Mouse*                  m_mouse;
    Keys*                   m_keys;

    SelectionHandler*       m_selection_handler;

    ISceneNode*             m_new_entity;

    CommandHandler          m_command_handler;

    Terrain*                m_terrain;

    bool                    m_spline_mode;

    IRoad*                  m_active_road;
    ISceneNode*             m_junk_node;


    // command not yet finished
    ICommand*               m_active_cmd;

    Indicator*              m_indicator;

    AztecCamera*            m_aztec_cam;

    ICameraSceneNode*       m_free_camera;

    static const long       TERRAIN_WAIT_TIME = 50;

    Track() {};

    void                    animateEditing();
    void                    animatePlacing();
    void                    animateSplineEditing();
    void                    animateTerrainMod(long dt);

    void                    leaveState();

public:
    static Track* getTrack(ICameraSceneNode* cam = 0);
    void          init(ICameraSceneNode* cam);
    void          setState(State state);
    void          keyEvent(EKEY_CODE code, bool pressed);
    void          mouseEvent(const SEvent& event);
    void          deleteCmd();
    void          setNewEntity(const stringw path);
    void          animate(long dt);
    void          setSplineMode(bool b);
    void          setActiveRoad(IRoad* r);
    void          roadBorn(IRoad* road, stringw name);
    void          undo();
    void          redo();  

    void          setFreeCamera(ICameraSceneNode* cam)   { m_free_camera       = cam; }
    bool          getSplineMode()                        { return m_spline_mode;      }

    Indicator*    getIndicator()                         { return m_indicator;        }
    
    ~Track();

};

#endif
