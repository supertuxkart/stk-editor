#ifndef ISUBSCRIBER_HPP
#define ISUBSCRIBER_HPP

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace video;

class ISubscriber
{
protected:
    u32 m_id;
public:
    virtual void notify(ITexture* t) = 0;
    void setSubIx(u32 ix)   { m_id = ix; }
};

#endif
