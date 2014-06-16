#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "structs.hpp"

#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace video;
using namespace core;

class Terrain :public ISceneNode, public IShaderConstantSetCallBack
{
private:
// ----------------------------------------------------------------------------
    // private variables - geometry
    Mesh                m_mesh;
    Mesh                m_highlight_mesh;

    // terrain size
    f32                 m_x, m_z;
    // vertex count in a single row/column
    int                 m_nx, m_nz;
    
    int                 m_last_mod_ID;
    float*              m_vertex_h_before;

    aabbox3d<f32>       m_bounding_box;

    void (Terrain::*m_fp) (const TerrainMod&, int, int, int, int);

// ----------------------------------------------------------------------------
    // private variables - material
    SMaterial           m_material;
    SMaterial           m_highlight_material;

    static const int    SPIMG_X;
    static const int    SPIMG_Y;
    IImage*             splattingImg;

// ----------------------------------------------------------------------------
    // private functions - geometry
    void    callOnVertices(TerrainMod* tmod, bool call_outside = false, 
                                             bool call_in_square = false);

    void    vertexHighlight(const TerrainMod& tm, int ix, int iz, int i, int j);
    void    vertexCut      (const TerrainMod& tm, int ix, int iz, int i, int j);
    void    modifyVertex   (const TerrainMod& tm, int ix, int iz, int i, int j);


    bool    intersectionPoint(const line3df& ray, float r, vector2df* cpos,
                              int* ix, int* iz);

    void    createIndexList(u16* indices, int x, int z);
    void    recalculateNormals();

// ----------------------------------------------------------------------------
    // private functions - material
    
    void      initMaterials();
    ITexture* createSplattingImg();
    
// ----------------------------------------------------------------------------
public:
    Terrain(ISceneNode* parent, ISceneManager* mgr, s32 id,
                  float x, float z, int nx, int nz);

    ~Terrain();

    void         modify   (TerrainMod* tm);
    void         cut      (TerrainMod* tm);
    void         highlight(TerrainMod* tm);

    vector3df    placeBBtoGround(const aabbox3d<f32>& box, line3d<float> ray);

    // material:

    void        draw(const TerrainMod& tm);


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
