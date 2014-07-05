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
    stringw     m_track_name;
    stringw     m_designer;
    stringc     m_file_name;
    Terrain*    m_terrain;
    DriveLine*  m_driveline;
public:

    Track(u32 tx, u32 tz);
    Track(path file);

    ~Track();

    void       save();
    void       quit();
    void       build();
               
    void       setTrackName(stringw n)      { m_track_name   = n; }
    void       setFileName(stringc n)       { m_file_name = n;    }
    void       setDesigner(stringw d)       { m_designer     = d; }
    
    void       setDriveLine(DriveLine* dl)  { m_driveline = dl; }
    Terrain*   getTerrain()                 { return m_terrain; }    

};


#endif
