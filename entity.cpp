#include "entity.hpp"

#include "editor.hpp"

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;

s32 Entity::ID = 0;

// ----------------------------------------------------------------------------
Entity::Entity(std::string model)
{
    ID++;
    m_model_name = model;
    // m_sc_node = ...

    m_sc_node = Editor::getEditor()->getSceneManager()->addCubeSceneNode();
    m_sc_node->setID(ID);

} // Entity


// ----------------------------------------------------------------------------
void Entity::kill()
{
    if (m_sc_node)
        m_sc_node->remove();
} // removeFromScene

// ----------------------------------------------------------------------------
void Entity::resurrect()
{
    // m_sc_node = ...

    m_sc_node = Editor::getEditor()->getSceneManager()->addCubeSceneNode();

} // resurrect

// ----------------------------------------------------------------------------
void Entity::move(float dx, float dy, float dz)
{
    if (!m_sc_node)
    {
        std::cerr << " move cmd failed: node doesn't exist...";
        std::cerr << std::endl;
        return;
    }
    m_sc_node->setPosition(m_sc_node->getPosition() + vector3df(dx, dy, dz));
} // move

// ----------------------------------------------------------------------------
void Entity::rotate(float dx, float dy, float dz)
{
    if (!m_sc_node)
    {
        std::cerr << " rotation failed: node doesn't exist...";
        std::cerr << std::endl;
        return;
    }
    m_sc_node->setRotation(m_sc_node->getRotation() + vector3df(dx, dy, dz));
} // rotate
