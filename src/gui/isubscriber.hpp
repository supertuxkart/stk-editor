#ifndef ISUBSCRIBER_HPP
#define ISUBSCRIBER_HPP

class irr::video::ITexture;

class ISubscriber
{
protected:
    u32 m_id;
public:
    virtual void notify(ITexture* t) = 0;
};

#endif
