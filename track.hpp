#ifndef TRACK_HPP
#define TRACK_HPP

#include "command.hpp"
#include "entities/entitymanager.hpp"
#include "entities/terrain.hpp"

#include <irrlicht.h>
#include <list>

#define MAGIC_NUMBER      8192

class Entity;
class Command;

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
        TERRAIN_MOD,
        TERRAIN_CUT,
        TERRAIN_DRAW,
        FREECAM
    };
    enum Key
    {
        W_PRESSED,
        A_PRESSED,
        S_PRESSED,
        D_PRESSED,
        CTRL_PRESSED,
        SHIFT_PRESSED
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
    static const int        m_key_num = 6;
    
    static int              m_last_entity_ID;

    State                   m_state;

    MouseData               m_mouse;

    EntityManager           m_entity_manager;

    ISceneNode*             m_new_entity;

    CommandHandler          m_command_handler;

    Terrain*                m_terrain;

    // command not yet finished
    Command*            m_active_cmd;

    // camera used for editing
    // position has to be uploaded manually
    ICameraSceneNode*   m_normal_camera;

    ICameraSceneNode*   m_free_camera;

    bool                m_key_state[m_key_num];
    bool                m_grid_on;

    static const long   TERRAIN_WAIT_TIME = 200;

    Track() {};
    void                animateNormalCamera(long dt);

    void                animateEditing();
    void                animateSelection();
    void                animatePlacing();
    void                animateTerrainMod(long dt);

public:
    static Track* getTrack();
    void          init();
    void          setState(State state);
    void          setGrid(bool grid_on);
    void          changeGridDensity(int dir);
    void          keyEvent(EKEY_CODE code, bool pressed);
    void          mouseEvent(const SEvent& event);
    void          deleteCmd();
    void          setNewEntity(const stringw path);
    void          animate(long dt);

    void          setNormalCamera(ICameraSceneNode* cam) { m_normal_camera     = cam; }
    void          setFreeCamera(ICameraSceneNode* cam)   { m_free_camera       = cam; }
    void          undo()                                 { m_command_handler.undo();  }
    void          redo()                                 { m_command_handler.redo();  }

    bool          isGridOn()                             { return m_grid_on; }
    
    ~Track();

};

#endif
