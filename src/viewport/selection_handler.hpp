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

    bool                firstIsCloser(vector3df v1, vector3df v2);

public:
    SelectionHandler(Mouse* m, Keys* k) :m_mouse(m), m_keys(k) {};

    void                clearSelection();
    void                selectNode(ISceneNode* node);
    void                animate(u32 id);

    list<ISceneNode*>   getSelectedSpherePoints();
    list<ISceneNode*>   getSelectedObjects();

    void                add(ISceneNode* e)      { m_elements.push_back(e);    }
    list<ISceneNode*>   getSelection()          { return m_selected_elements; }
};

#endif
