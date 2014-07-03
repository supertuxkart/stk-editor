#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "mesh/mesh_structs.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;
using namespace io;

class Terrain :public ISceneNode, public IShaderConstantSetCallBack
{
private:
// ----------------------------------------------------------------------------
    // private variables - geometry
    Mesh                m_mesh;
    Mesh                m_highlight_mesh;

    // terrain size
    f32                 m_x, m_z;

    aabbox3d<f32>       m_bounding_box;

    bool                m_highlight_visible;

    void (Terrain::*m_fp_h) (const TerrainMod&, int, int, int, int);

// ----------------------------------------------------------------------------
    // private variables - material
    SMaterial           m_material;
    SMaterial           m_highlight_material;

    IImage*             m_splattingImg;

    u16                 m_tile_num_x;
    u16                 m_tile_num_z;

    void (Terrain::*m_fp_b) (u8*,int,SColor);

// ----------------------------------------------------------------------------
public:
    // vertex count in a single row/column
    const int                 m_nx, m_nz;
    static const u32          SPIMG_X;
    static const u32          SPIMG_Y;

// ----------------------------------------------------------------------------
    // private functions - geometry
    void    callOnVertices(TerrainMod* tmod, bool call_outside = false,
                                             bool call_in_square = false);

    void    vertexHighlight(const TerrainMod& tm, int ix, int iz, int i, int j);
    void    vertexHeight   (const TerrainMod& tm, int ix, int iz, int i, int j);


    bool    intersectionPoint(const line3df& ray, float r, vector2df* cpos,
                              int* ix, int* iz);

    void    createIndexList(u16* indices, int x, int z);
    void    recalculateNormals();
    void    refreshBB();

// ----------------------------------------------------------------------------
    // private functions - material

    void        initMaterials();
    ITexture*   createSplattingImg();
    void        draw(const TerrainMod& tm);

    void        pixelHardBrush(u8* img, int ix, SColor col, bool erase);
    void        pixelSoftBrush(u8* img, int ix, SColor col, double e);
    void        pixelBrigBrush(u8* img, int ix, double e);

// ----------------------------------------------------------------------------
public:
    Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id,
                  float x, float z, int nx, int nz);

    ~Terrain();

    void         modify   (TerrainMod* tm);
    void         cut      (TerrainMod* tm);
    void         highlight(TerrainMod* tm);

    vector3df    placeBBtoGround(const aabbox3d<f32>& box, line3d<float> ray);

    void        setHighlightVisibility(bool b) { m_highlight_visible = b; }

    void        exprt();

// ----------------------------------------------------------------------------
    // inherited functions - ISceneNode
    void                 OnRegisterSceneNode();
    void                 render();

    const aabbox3d<f32>& getBoundingBox()   const { return m_bounding_box; }
    u32                  getMaterialCount() const { return 1; }

// ----------------------------------------------------------------------------
    // inherited functions - IShaderConstantSetCallBack
    SMaterial&           getMaterial(u32 i)       { return m_material; }

    void OnSetConstants(video::IMaterialRendererServices* services, s32 userData);
};

#endif
