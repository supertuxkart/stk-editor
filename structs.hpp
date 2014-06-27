#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;

class ITCommand;

enum
{
    HEIGHT_MOD,
    HARD_BRUSH,
    SOFT_BRUSH,
    BRIGHTNESS_MOD
};

struct TerrainMod
{
    bool        left_click;
    line3df     ray;
    bool        in_terrain;
    int         edge_type;
    float       radius;
    float       dh;
    long        countdown;
    bool        max;
    float       max_v;
    bool        min;
    float       min_v;
    int         index;
    float       dist;
    SColor      col_mask;
    int         type;
    ITCommand*  cmd;
};

struct TerrainChange
{
    float              dh;
    u8                 old_v[4];
    bool               vchanged[4];
    u32                x;
    u32                z;
    S3DVertex2TCoords* v;
    u8*                img;
};

struct Mesh
{
    S3DVertex2TCoords*  vertices;
    u16*                indices;
    unsigned int        vertex_count;
    unsigned int        quad_count;
};

struct ControlPoint
{
    vector3df        vel;
    vector3df        pos;
    vector3df        normal;
    float            width;
    float            t;
    ISceneNode*      node;
    ISceneNode*      normal_node;
    ISceneNode*      width_node;
};

#endif
