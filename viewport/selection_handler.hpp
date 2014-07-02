#ifndef SELECTION_HANDLER_HPP
#define SELECTION_HANDLER_HPP


#include <irrlicht.h>

using namespace irr;
using namespace scene;
using namespace core;

class SelectionHandler
{
private:
    list<ISceneNode*>   m_elements;
    list<ISceneNode*>   m_selected_elements;
public:
    void                add(ISceneNode* e)      { m_elements.push_back(e); }
    list<ISceneNode*>   getSelection()          { return m_selected_elements; }

    void                clearSelection();
    void                selectNode(ISceneNode* node);
};

#endif
