#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>

#include <irrlicht.h>

using namespace irr;
using namespace scene;

class Entity
{
private:
    static s32   ID;
    ISceneNode*  m_sc_node;
    std::string  m_model_name;
public:
    Entity(std::string model);
    
    void kill();
    void resurrect();

    void move  (float dx, float dy, float dz);
    void rotate(float dx, float dy, float dz);

    bool hasSceneNode(ISceneNode* node)     { return (node == m_sc_node); }
    
    void drawBoundingBox()                   { m_sc_node->setDebugDataVisible(scene::EDS_BBOX); }
    void clearBoundingBox()                   { m_sc_node->setDebugDataVisible(0);    }

};

#endif
