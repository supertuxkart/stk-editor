#ifndef ICMD_HPP
#define ICMD_HPP

// ----------------------------------------------------------------------------
class ICmd
{
public:
    ICmd() {};
    virtual ~ICmd() {};
    virtual void redo() = 0;
    virtual void undo() = 0;
};

#endif
