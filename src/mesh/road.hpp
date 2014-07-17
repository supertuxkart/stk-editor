#ifndef ROAD_HPP
#define ROAD_HPP

#include "mesh/iroad.hpp"
#include "gui/isubscriber.hpp"

class Road :public IRoad, public ISubscriber
{
private:

    void    calcVertexRow(vector3df p, vector3df n, vector3df w, int offset, 
                                                       float wx, float t);
    void    createIndexList(int nj, int ni);

// ----------------------------------------------------------------------------
public:
    Road(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s, stringw n) 
            :IRoad(parent, mgr, id, s, n) {};

    Road(ISceneNode* parent, ISceneManager* mgr, s32 id, FILE* fp)
        :IRoad(parent, mgr, id, fp) {};

    void    refresh();
    void    render();

    void    notify(ITexture* t);
    void    setWireFrame(bool b);
};

#endif
