#ifndef ROAD_HPP
#define ROAD_HPP

#include "road/iroad.hpp"

class Road :public IRoad
{
private:

    void    calcVertexRow(vector3df p, vector3df n, vector3df w, 
                                           int offset, float wx);
    void    createIndexList(int nj, int ni);

// ----------------------------------------------------------------------------
public:
    Road(ISceneNode* parent, ISceneManager* mgr, s32 id, ISpline* s) 
            :IRoad(parent, mgr, id, s) {};

    void    refresh();
    
    void    render();
};

#endif
