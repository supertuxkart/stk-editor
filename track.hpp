#ifndef TRACK_HPP
#define TRACK_HPP

#include "indicator.hpp"

#include "commands/commandhandler.hpp"
#include "commands/iocommand.hpp"
#include "commands/itcommand.hpp"

#include "entities/entitymanager.hpp"
#include "entities/terrain.hpp"

#include "road/bezier.hpp"
#include "road/tcr.hpp"
#include "road/road.hpp"
#include "road/driveline.hpp"

#include "mouse.hpp"

#include <irrlicht.h>

class Entity;

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
    enum Key
    {
        W_PRESSED, // the first has to have id 0
        A_PRESSED,
        S_PRESSED,
        D_PRESSED,
        SPACE_PRESSED,
        CTRL_PRESSED,
        SHIFT_PRESSED //it has to be the last the first has to have id 0
    };


private:
    static Track*           m_track;
    static const int        m_key_num = SHIFT_PRESSED + 1;
    
    static int              m_last_entity_ID;

    State                   m_state;

    MouseData               m_mouse;

    EntityManager           m_entity_manager;

    ISceneNode*             m_new_entity;

    CommandHandler          m_command_handler;

    Terrain*                m_terrain;

    bool                    m_spline_mode;

    IRoad*                  m_active_road;
    ISceneNode*             m_junk_node;


    // command not yet finished
    ICommand*               m_active_cmd;


    Indicator*              m_indicator;

    // camera used for editing
    // position has to be uploaded manually
    ICameraSceneNode*       m_normal_camera;

    float                   m_normal_cd;

    ICameraSceneNode*       m_free_camera;

    bool                    m_key_state[m_key_num];

    static const long       TERRAIN_WAIT_TIME = 50;

    Track() {};
    void                    animateNormalCamera(float dt);

    void                    animateEditing();
    void                    animateSelection();
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
