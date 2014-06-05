#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "entity.hpp"

#include <list>

// ----------------------------------------------------------------------------
class Command
{
protected:
    std::list<Entity*> m_elements;

    virtual void redo(Entity* e) = 0;
    virtual void undo(Entity* e) = 0;

public:
    Command(std::list<Entity*> elements) { m_elements = elements; }
    virtual ~Command() {};
    void redo();
    void undo();
    virtual void update(float a, float b, float c) {};
};

// ----------------------------------------------------------------------------
class DelCmd : public Command
{
public:
    DelCmd(std::list<Entity*> e) :Command(e) {                 };
    void redo(Entity* e)                      { e->kill();      }
    void undo(Entity* e)                      { e->resurrect(); }
};

// ----------------------------------------------------------------------------
class MoveCmd : public Command
{
private:
    float m_dx, m_dy, m_dz;
public:
    MoveCmd(std::list<Entity*> e) :Command(e) { m_dx = 0; m_dy = 0; m_dz = 0;    }
    void redo(Entity* e)                      { e->move( m_dx,  m_dy,  m_dz);    }
    void undo(Entity* e)                      { e->move(-m_dx, -m_dy, -m_dz);    }
    void update(float a, float b, float c)    { m_dx += a; m_dy += b; m_dz += c; }
};

// ----------------------------------------------------------------------------
class RotateCmd : public Command
{
private:
    float m_dx, m_dy, m_dz;
public:
    RotateCmd(std::list<Entity*> e) :Command(e)  { m_dx = 0; m_dy = 0; m_dz = 0;     }
    void redo(Entity* e)                         { e->rotate(m_dx, m_dy, m_dz);      }
    void undo(Entity* e)                         { e->rotate(-m_dx, -m_dy, -m_dz);   }
    void update(float a, float b, float c)       { m_dx += a; m_dy += b; m_dz += c;  }
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
