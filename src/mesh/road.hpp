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

    ITriangleSelector*               getTriangleSel(){ return m_tri;       }
    CMeshBuffer<S3DVertex2TCoords>*  getMeshBuffer() { return m_mesh_buff; }
};
#endif
