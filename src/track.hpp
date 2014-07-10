#ifndef TRACK_HPP
#define TRACK_HPP

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace io;

class Terrain;
class IRoad;
class ISpline;
class DriveLine;

class Track
{
private:
    stringw     m_track_name;
    stringw     m_designer;
    stringc     m_file_name;
    Terrain*    m_terrain;
    DriveLine*  m_driveline;

    map<unsigned int, IRoad*>    m_roads;

public:

    Track(f32 tx, f32 tz);
    Track(path file);

    ~Track();

    void       save();
    void       quit();
    void       build();

    void       insertRoad(IRoad* road);
    void       removeLastRoad();
    void       createRoad(stringw type, stringw name);

    IRoad*     getRoadByID(u32 id)           { return m_roads[id]; }

    void       setTrackName(stringw n)       { m_track_name   = n; }
    void       setFileName(stringc n)        { m_file_name    = n; }
    void       setDesigner(stringw d)        { m_designer     = d; }

    Terrain*   getTerrain()                  { return m_terrain;   }

    map<unsigned int, IRoad*>* getRoadList() { return &m_roads;    }

};


#endif
