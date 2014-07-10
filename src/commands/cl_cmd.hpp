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

#endif
