#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>

#include <irrlicht.h>

using namespace irr;
using namespace scene;

class Entity
{
private:
    unsigned int m_ID;
    ISceneNode*  m_sc_node;
    std::string  m_model_name;
public:
    Entity(unsigned int ID, std::string model);
    
    void kill();
    void resurrect();

    void move  (float dx, float dy, float dz);
    void rotate(float dx, float dy, float dz);
};

#endif
