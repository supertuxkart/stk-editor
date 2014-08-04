#ifndef ROAD_HPP
#define ROAD_HPP

#include "mesh/iroad.hpp"
#include "gui/isubscriber.hpp"

class Road :public IRoad, public ISubscriber
{
private:

    CMeshBuffer<S3DVertex2TCoords>* m_mesh_buff;
    SMesh                           m_smesh;
    ITriangleSelector*              m_tri;
    
    array<vector2df>                m_cross_section;

    virtual void    textureExport(FILE* fp);
    virtual void    textureImport(FILE* fp);

    void    calcVertexRow(vector3df p, vector3df n, vector3df w, int offset, 
                                                       float wx, float t);
    void    createIndexList(int nj, int ni);

// ----------------------------------------------------------------------------
public:
    Road(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n);
    Road(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp);

    void    refresh();
    void    render();

    void    notify(ITexture* t);
    void    setWireFrame(bool b);

    void    attachToDriveLine(IRoad* dl);
    void    setCrossSection(array<vector2df> cs);

    static array<vector2df> genStandardCrossSection(u32 wvn);
    

    array<vector2df> getCrossSectionArray()          { return m_cross_section; }
    ITriangleSelector*               getTriangleSel(){ return m_tri;           }
    CMeshBuffer<S3DVertex2TCoords>*  getMeshBuffer() { return m_mesh_buff;     }
                                                                               
    virtual const aabbox3d<f32>& getBoundingBox() const                        
                                    { return m_mesh_buff->getBoundingBox();    }
};
#endif
