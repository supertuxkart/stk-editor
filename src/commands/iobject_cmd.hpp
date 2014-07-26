#ifndef IOCOMMAND_HPP
#define IOCOMMAND_HPP

#include "commands/icmd.hpp"

#include <irrlicht.h>
#include <list>

using namespace irr;
using namespace scene;
using namespace core;

// ----------------------------------------------------------------------------
class IObjectCmd :public ICmd
{
protected:
    list<ISceneNode*> m_elements;

    virtual void redo(ISceneNode* e) = 0;
    virtual void undo(ISceneNode* e) = 0;

public:
    IObjectCmd(list<ISceneNode*> elements) { m_elements = elements; }
    virtual ~IObjectCmd() {};

    void            redo();
    void            undo();
    void            calcCenter(s32* x, s32* y);
    virtual void    update(s32 x, s32 y) {};
};

// ----------------------------------------------------------------------------
class DelCmd : public IObjectCmd
{
private:
    bool m_rdy;
public:
    DelCmd(list<ISceneNode*> e) :IObjectCmd(e)           { m_rdy = false; }

    void redo(ISceneNode* e)      { e->setVisible(false); m_rdy =  true; }
    void undo(ISceneNode* e)      { e->setVisible( true); m_rdy = false; }

    ~DelCmd();
};

// ----------------------------------------------------------------------------
class CreateCmd : public IObjectCmd
{
private:
    bool m_rdy;
public:
    CreateCmd(list<ISceneNode*> e) :IObjectCmd(e)    { m_rdy = true; }

    void redo(ISceneNode* e)      { e->setVisible(true);  m_rdy = true;  }
    void undo(ISceneNode* e)      { e->setVisible(false); m_rdy = false; }

    ~CreateCmd();
};

// ----------------------------------------------------------------------------
class MoveCmd : public IObjectCmd
{
private:
    line3df m_start_ray;
    line3df m_curr_ray;
    vector3df m_tx, m_ty;
public:
    MoveCmd(list<ISceneNode*> e, s32 sx, s32 sy);

    void update(s32 x, s32 y);

    void redo(ISceneNode* e);
    void undo(ISceneNode* e);
}; // MoveCmd

// ----------------------------------------------------------------------------
class RotateCmd : public IObjectCmd
{
private:
    bool z_mode;
    s32 m_sx, m_sy;
    s32 m_x,  m_y;
    s32 m_cx, m_cy;
    vector3df m_tx;
    vector3df m_ty;
    vector3df m_tz;
private:
    vector3df calcRot(f32 angle, vector3df axis);
    void      calcAllRot(vector3df* x, vector3df* y, vector3df* z);
public:
    RotateCmd(list<ISceneNode*> e, s32 x, s32 y, vector3df i, vector3df j, vector3df k);

    void setZMode(bool zmode, s32 x, s32 y);
    void redo(ISceneNode* e);
    void undo(ISceneNode* e);

    void update(s32 x, s32 y)       { m_x = x; m_y = y; }
}; // RotateCmd

// ----------------------------------------------------------------------------
class ScaleCmd : public IObjectCmd
{
private:
    s32 m_sx, m_sy;
    s32 m_cx, m_cy;
    s32 m_x,  m_y ;
public:
    ScaleCmd(list<ISceneNode*> e, s32 x, s32 y);

    void redo(ISceneNode* e);
    void undo(ISceneNode* e);

    void update(s32 x, s32 y)       { m_x = x; m_y = y; }
}; // ScaleCmd

#endif
