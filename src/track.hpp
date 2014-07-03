#ifndef TRACK_HPP
#define TRACK_HPP

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace io;

class Terrain;

class Track
{
private:
    stringw     m_name;
    stringw     m_author;
    Terrain*    m_terrain;
public:
    Track()     { m_terrain = 0; }
    ~Track();

    void        create(u32 tx, u32 tz);
    void        load(path file);
    void        save(path file);
    void        quit();

    void        setName(stringw n)      { m_name   = n;     }
    void        setAuthor(stringw a)    { m_author = a;     }
    Terrain*    getTerrain()            { return m_terrain; }

};


#endif
