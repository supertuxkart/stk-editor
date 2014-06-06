#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <irrlicht.h>
#include <list>

using namespace irr;
using namespace scene;
using namespace core;

// ----------------------------------------------------------------------------
class Command
{
protected:
    std::list<ISceneNode*> m_elements;

    virtual void redo(ISceneNode* e) = 0;
    virtual void undo(ISceneNode* e) = 0;

public:
    Command(std::list<ISceneNode*> elements) { m_elements = elements; }
    virtual ~Command() {};
    void redo();
    void undo();
    virtual void update(float a, float b, float c) {};
};

// ----------------------------------------------------------------------------
class DelCmd : public Command
{
private:
    bool m_rdy;
public:
    DelCmd(std::list<ISceneNode*> e) :Command(e)        { m_rdy = false; }

    void redo(ISceneNode* e)      { e->setVisible(false); m_rdy =  true; }
    void undo(ISceneNode* e)      { e->setVisible( true); m_rdy = false; }

    DelCmd::~DelCmd();
};

// ----------------------------------------------------------------------------
class MoveCmd : public Command
{
private:
    float m_dx, m_dy, m_dz;
public:
    MoveCmd(std::list<ISceneNode*> e) :Command(e) { m_dx = 0; m_dy = 0; m_dz = 0;    }
    void update(float a, float b, float c)        { m_dx += a; m_dy += b; m_dz += c; }

    void redo(ISceneNode* e);
    void undo(ISceneNode* e);
};

// ----------------------------------------------------------------------------
class RotateCmd : public Command
{
private:
    float m_dx, m_dy, m_dz;
public:
    RotateCmd(std::list<ISceneNode*> e) :Command(e) { m_dx = 0; m_dy = 0; m_dz = 0;    }
    void update(float a, float b, float c)          { m_dx += a; m_dy += b; m_dz += c; }
    void redo(ISceneNode* e);
    void undo(ISceneNode* e);
};

// ----------------------------------------------------------------------------
class CommandHandler
{
private:
    std::list<Command*>              m_cmd_stack;
    std::list<Command*>::iterator    m_it;
public:
    CommandHandler();
    void add(Command* cmd);
    void redo();
    void undo();
};

#endif
