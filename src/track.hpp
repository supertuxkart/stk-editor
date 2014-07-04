#ifndef TRACK_HPP
#define TRACK_HPP

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace io;

class Terrain;
class DriveLine;

class Track
{
private:
    stringw     m_name;
    stringw     m_author;
    Terrain*    m_terrain;
    DriveLine*  m_driveline;
public:
    Track()     { m_terrain = 0; }
    ~Track();

    void       create(u32 tx, u32 tz);
    void       load(path file);
    void       save(path file);
    void       quit();
    void       build();
               
    void       setName(stringw n)           { m_name   = n;     }
    void       setAuthor(stringw a)         { m_author = a;     }
    void       setDriveLine(DriveLine* dl)  { m_driveline = dl; }
    Terrain*   getTerrain()                 { return m_terrain; }    

};


#endif
