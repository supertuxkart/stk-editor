#ifndef TRACK_HPP
#define TRACK_HPP

#include "commands/commandhandler.hpp"
#include "commands/iocommand.hpp"
#include "commands/itcommand.hpp"

#include "entities/entitymanager.hpp"
#include "entities/terrain.hpp"
#include "road/bezier.hpp"
#include "road/tcr.hpp"
#include "road/road.hpp"
#include "road/driveline.hpp"

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
        W_PRESSED,
        A_PRESSED,
        S_PRESSED,
        D_PRESSED,
        SPACE_PRESSED,
        CTRL_PRESSED,
        SHIFT_PRESSED //it has to be the last and the first has to have id 0
    };

    struct MouseData
    {
        bool                    in_view;
        f32                     wheel;
        bool                    left_btn_down;
        bool                    right_btn_down;

        bool                    left_pressed;
        bool                    right_pressed;

        bool                    left_released;
        bool                    right_released;

        s32                     x;
        s32                     y;
        s32                     prev_x;
        s32                     prev_y;

        MouseData();

        void setStorePoint()    { prev_x = x; prev_y = y;                                       }
        s32 dx()                { return x - prev_x; }
        s32 dy()                { return y - prev_y; }

        // return value is true if left mouse button was pressed down after the last call
        bool leftPressed()        { bool b = left_pressed;    left_pressed = false; return b;   }

        // return value is true if right mouse button was pressed down after the last call
        bool rightPressed()       { bool b = right_pressed;   right_pressed = false; return b;  }

        // return value is true if left mouse button was released after the last call
        bool leftReleased()       { bool b = left_released;  left_released = false; return b;  }

        // return value is true if right mouse button was released after the last call
        bool rightReleased()      { bool b = right_released; right_released = false; return b; }

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

    // command not yet finished
    IOCommand*              m_active_obj_cmd;
    ITCommand*              m_active_terr_cmd;

    // camera used for editing
    // position has to be uploaded manually
    ICameraSceneNode*   m_normal_camera;
    float               m_normal_cd;

    ICameraSceneNode*   m_free_camera;

    bool                m_key_state[m_key_num];

    static const long   TERRAIN_WAIT_TIME = 50;

    Track() {};
    void                animateNormalCamera(float dt);

    void                animateEditing();
    void                animateSelection();
    void                animatePlacing();
    void                animateSplineEditing();
    void                animateTerrainMod(long dt);

    void                leaveState();

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

    void          setFreeCamera(ICameraSceneNode* cam)   { m_free_camera       = cam; }
    void          undo()                                 { m_command_handler.undo();  }
    void          redo()                                 { m_command_handler.redo();  }
    
    bool          getSplineMode()                        { return m_spline_mode;      }
    
    ~Track();

};

#endif
