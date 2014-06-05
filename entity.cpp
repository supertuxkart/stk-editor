#include "entity.hpp"

#include "editor.hpp"

#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;


// ----------------------------------------------------------------------------
Entity::Entity(unsigned int ID, std::string model)
{
    m_ID = ID;
    m_model_name = model;
    // m_sc_node = ...

    m_sc_node = Editor::getEditor()->getSceneManager()->addCubeSceneNode();

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
        std::cerr << m_ID << " node can't move, because it doesn't exist...";
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
        std::cerr << m_ID << " node can't rotate, because it doesn't exist...";
        std::cerr << std::endl;
        return;
    }
    m_sc_node->setRotation(m_sc_node->getRotation() + vector3df(dx, dy, dz));
} // rotate
