#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;

struct TerrainMod
{
    line3df ray;
    bool    in_terrain;
    int     edge_type;
    int     ID;
    float   radius;
    float   dh;
    long    countdown;
    bool    max;
    float   max_v;
    bool    min;
    float   min_v;
    bool    max_cut;
    float   cut_v;
    int     index;
    float   dist;
    SColor  col_mask;
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
    float            t;
    ISceneNode*      node;
    ISceneNode*      normal_node;
};

#endif
