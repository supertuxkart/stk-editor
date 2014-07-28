#ifndef CL_CMD_HPP
#define CL_CMD_HPP

#include "commands/icmd.hpp"
#include "viewport/checkline_handler.hpp"

// ----------------------------------------------------------------------------
class CLCmd :public ICmd
{
private:
    CheckLineHandler* m_clh;
public:
    CLCmd(CheckLineHandler* clh) :m_clh(clh) {};
    ~CLCmd() {};

    void    redo()     { m_clh->redo(); }
    void    undo()     { m_clh->undo(); }
};

// ----------------------------------------------------------------------------
class CLRCmd :public ICmd
{
private:
    CheckLineHandler*   m_clh;
    list<ISceneNode*>   m_nodes;
public:
    CLRCmd(CheckLineHandler* clh, list<ISceneNode*> nl) :m_clh(clh), m_nodes(nl)
    { 
        doit(true);
    }
    ~CLRCmd() {};

    void    redo()     { doit(true);  }
    void    undo()     { doit(false); }

    void    doit(bool b)
    {
        list<ISceneNode*>::Iterator it;
        for (it = m_nodes.begin(); it != m_nodes.end(); it++)
            m_clh->remove(*it, b);
    }

};

#endif
