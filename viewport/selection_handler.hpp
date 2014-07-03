#ifndef SELECTION_HANDLER_HPP
#define SELECTION_HANDLER_HPP


#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;

struct Mouse;
struct Keys;

class SelectionHandler
{
private:
    list<ISceneNode*>   m_elements;
    list<ISceneNode*>   m_selected_elements;
    Mouse*              m_mouse;
    Keys*               m_keys;

public:
    SelectionHandler(Mouse* m, Keys* k) :m_mouse(m), m_keys(k) {};

    void                clearSelection();
    void                selectNode(ISceneNode* node);
    void                animate(u32 id);

    void                add(ISceneNode* e)      { m_elements.push_back(e);    }
    list<ISceneNode*>   getSelection()          { return m_selected_elements; }
};

#endif
