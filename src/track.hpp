#ifndef TRACK_HPP
#define TRACK_HPP

#include <irrlicht.h>
#include <fstream>

using namespace irr;
using namespace scene;
using namespace core;
using namespace io;

class Terrain;
class IRoad;
class ISpline;
class DriveLine;

class Track
{
private:
    bool            m_valid;

    bool            m_gravity_road;

    stringw         m_track_name;
    stringw         m_designer;
    stringc         m_file_name;
    Terrain*        m_terrain;
    DriveLine*      m_driveline;
    stringc         m_music;

    // m_roads[0] is the driveline
    array<IRoad*>   m_roads;

    ISceneNode*    loadItem(stringc name);

    void           exportElements(std::ofstream& stream, bool obj);
    void           copyObj(stringc name);
public:

    Track(f32 tx, f32 tz);
    Track(path file);

    ~Track();

    void       save();
    void       quit();
    void       build();

    void       insertRoad(IRoad* road);
    void       removeRoad(IRoad* road);
    void       createRoad(stringw type, stringw name);
    u32        getRoadID(IRoad* r);

    IRoad*     getRoadByID(u32 id)           { IRoad* r = 0; return (id < m_roads.size()) ? m_roads[id] : r; }
    stringc    getFileName()                 { return m_file_name; }

    void       setTrackName(stringw n)       { m_track_name   = n; }
    void       setFileName(stringc n)        { m_file_name    = n; }
    void       setDesigner(stringw d)        { m_designer     = d; }
    void       setMusic(stringc m)           { m_music        = m; }
    void       setGravityRoad(bool b)        { m_gravity_road = b; }

    Terrain*   getTerrain()                  { return m_terrain;   }

    array<IRoad*>* getRoadList()             { return &m_roads;    }

    bool       isValid()                     { return m_valid;     }
};
#endif
