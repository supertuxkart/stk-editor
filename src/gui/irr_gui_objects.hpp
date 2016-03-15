#ifndef SIMPLEGUIOBJECTS_HPP
#define SIMPLEGUIOBJECTS_HPP

#include <irrlicht.h>

using namespace irr;
using namespace gui;
using namespace irr::core;

// gcc - suppress reorder warning
#pragma GCC diagnostic ignored "-Wreorder"

class SimpleGuiComponent {

public:
    SimpleGuiComponent(){}
    SimpleGuiComponent(IGUIWindow* setWindow, IGUIElement* setIrrComponent, rect<s32> setRootPos)       
    : m_wndw(setWindow), m_irr_component(setIrrComponent), m_root_pos(setRootPos)
    {
    }
    ~SimpleGuiComponent(){}
    
    IGUIElement*    getIrrComponent() { return m_irr_component; }
    rect<s32>       getRootPosition() { return m_root_pos;  }
private:
    IGUIWindow*     m_wndw;     // Parent window
    IGUIElement* m_irr_component;
    rect<s32> m_root_pos;

};

class SimpleGuiComponents {
private:
    list<SimpleGuiComponent*> SimpleGuiComponentList;
    
public:
    SimpleGuiComponents()
    {
    }
// ----------------------------------------------------------------------------    
    ~SimpleGuiComponents()
    {
        for(list<SimpleGuiComponent*>::Iterator itr = SimpleGuiComponentList.begin(); itr != SimpleGuiComponentList.end(); itr++)
        {
            SimpleGuiComponent* gui_component = ((SimpleGuiComponent*)*itr);
            
            delete gui_component;
        }
        SimpleGuiComponentList.clear();
    }
// ----------------------------------------------------------------------------
    void addComponent(IGUIWindow* setWindow, IGUIElement* setIrrComponent, rect<s32> setRootPos)
    { 
        SimpleGuiComponentList.push_front(new SimpleGuiComponent(setWindow, setIrrComponent, setRootPos));
    }   
// ----------------------------------------------------------------------------
    void updateComponentPositions(int locationAsPixel)
    {
        for(list<SimpleGuiComponent*>::Iterator itr = SimpleGuiComponentList.begin(); itr != SimpleGuiComponentList.end(); itr++)
        {
            SimpleGuiComponent* gui_component = ((SimpleGuiComponent*)*itr);
            
            gui_component->getIrrComponent()->setRelativePosition(rect<s32>(
                            gui_component->getRootPosition().UpperLeftCorner.X,
                            gui_component->getRootPosition().UpperLeftCorner.Y - locationAsPixel,
                            gui_component->getRootPosition().LowerRightCorner.X,
                            gui_component->getRootPosition().LowerRightCorner.Y - locationAsPixel));
        }
    }
};

#endif // SIMPLEGUIOBJECTS
    