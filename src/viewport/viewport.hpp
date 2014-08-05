#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "track.hpp"
#include "commands/command_handler.hpp"
#include "viewport/checkline_handler.hpp"
#include "viewport/aztec_camera.hpp"
#include "mesh/sky.hpp"

#include <irrlicht.h>

class  Terrain;
class  IRoad;
class  SelectionHandler;
class  Indicator;
class  DriveLine;
struct Mouse;
struct Keys;
class  Sky;

using namespace irr;
using namespace scene;
using namespace core;

class Viewport
{
public:
    enum State
    {
        SELECT,
        EDIT,
        PLACE,
        SPLINE,
        CHECK_LINE,
        TERRAIN_MOD,
        FREECAM
    };

    enum Editing
    {
        MOVE,
        ROTATE,
        SCALE
    };

private:
    static Viewport*        m_self;

    State                   m_state;
    Editing                 m_edit;

    Track*                  m_track;

    Mouse*                  m_mouse;
    Keys*                   m_keys;

    SelectionHandler*       m_selection_handler;
    CommandHandler          m_command_handler;
    CheckLineHandler        m_clh;

    ISceneNode*             m_new_entity;
    static int              m_last_entity_ID;

    bool                    m_spline_mode;
    bool                    m_rcs_mode;

    IRoad*                  m_active_road;
    ISceneNode*             m_junk_node;


    // command not yet finished
    ICmd*                   m_active_cmd;

    AztecCamera*            m_aztec_cam;
    ICameraSceneNode*       m_free_camera;

    Sky*                    m_sky;
    Terrain*                m_terrain;
    static const long       TERRAIN_WAIT_TIME = 50;

    Viewport() {};

    void                    animateSelection();
    void                    animateEditing(u32 dt);
    void                    animatePlacing();
    void                    animateSplineEditing();
    void                    animateCheckLine();
    void                    animateTerrainMod(long dt);

    void                    leaveState();

public:
    static Viewport*    get(ICameraSceneNode* cam = 0, Mouse* m = 0, Keys* k = 0);
    void                init(ICameraSceneNode* cam, Mouse* m, Keys* k);
    void                move();
    void                rotate();
    void                scale();
    void                switchRoadCrossSectionMode(bool apply_mod);
    void                setState(State state);
    void                deleteCmd();
    void                setNewEntity(const stringw path, const stringw name);
    void                setNewEntity(ISceneNode* node);
    void                animate(long dt);
    void                setSplineMode(bool b);
    void                setActiveRoad(IRoad* r);
    void                setActiveRoad(u32 id);
    void                roadBorn(IRoad* road);
    void                undo();
    void                redo();
    bool                splineInterrupt();
    void                loseFocus();
    void                gainFocus();
    bool                setTrack(Track* t);
    void                build();
    void                clear();
    void                clearSelection();
    void                draw();
    void                restoreCam();
    void                selectDriveLine();

    Indicator*          getIndicator();

    void showSky();
    void setSky(Sky* sky);

    void                setRCSMode(bool b)                   { m_rcs_mode       = b;     }
    void                setFreeCamera(ICameraSceneNode* cam) { m_free_camera    = cam;   }
    static void         setLastEntityID(u32 id)              { m_last_entity_ID = id;    }
    static u32          getLastEntityID()                    { return m_last_entity_ID;  }
    bool                getSplineMode()                      { return m_spline_mode;     }
    Track*              getTrack()                           { return m_track;           }
    Terrain*            getTerrain()                         { return m_terrain;         }
    Sky*                getSky()                             { return m_sky;             }
    void                hideSky()                            { if (m_sky) m_sky->hide(); }
    u32                 getState()                           { return m_state;           }
    vector3df           getAztecPos()                        { return m_aztec_cam->Pos();}
    void                printCheckLine(std::ofstream* f)     { m_clh.build(f);           }
    void                setEditMode(Editing e)               { m_edit = e;               }

    ~Viewport();
};

#endif
