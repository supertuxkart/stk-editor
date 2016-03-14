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
    SimpleGuiComponent(IGUIWindow* setWindow, IGUIScrollBar* setScrollBar, rect<s32> setRootPos) 
    : m_wndw(setWindow), m_scrollbar(setScrollBar), m_image(nullptr), m_editbox(nullptr), m_button(nullptr), m_statictext(nullptr), m_checkbox(nullptr), rootPos(setRootPos), m_active(setScrollBar)    {}
    
    SimpleGuiComponent(IGUIWindow* setWindow, IGUIImage* setImage, rect<s32> setRootPos)         
    : m_wndw(setWindow), m_scrollbar(nullptr), m_image(setImage), m_editbox(nullptr), m_button(nullptr), m_statictext(nullptr), m_checkbox(nullptr), rootPos(setRootPos), m_active(setImage)            {}
    
    SimpleGuiComponent(IGUIWindow* setWindow, IGUIEditBox* setEditBox, rect<s32> setRootPos)     
    : m_wndw(setWindow), m_scrollbar(nullptr), m_image(nullptr), m_editbox(setEditBox), m_button(nullptr), m_statictext(nullptr), m_checkbox(nullptr), rootPos(setRootPos), m_active(setEditBox)        {}
    
    SimpleGuiComponent(IGUIWindow* setWindow, IGUIButton* setButton, rect<s32> setRootPos)       
    : m_wndw(setWindow), m_scrollbar(nullptr), m_image(nullptr), m_editbox(nullptr), m_button(setButton), m_statictext(nullptr), m_checkbox(nullptr), rootPos(setRootPos), m_active(setButton)          {}
    
    SimpleGuiComponent(IGUIWindow* setWindow, IGUIStaticText* setStaticText, rect<s32> setRootPos)       
    : m_wndw(setWindow), m_scrollbar(nullptr), m_image(nullptr), m_editbox(nullptr), m_button(nullptr), m_statictext(setStaticText), m_checkbox(nullptr), rootPos(setRootPos), m_active(setStaticText)  {}
    
    SimpleGuiComponent(IGUIWindow* setWindow, IGUICheckBox* setCheckBox, rect<s32> setRootPos)       
    : m_wndw(setWindow), m_scrollbar(nullptr), m_image(nullptr), m_editbox(nullptr), m_button(nullptr), m_statictext(nullptr), m_checkbox(setCheckBox), rootPos(setRootPos), m_active(setCheckBox)      {}
    
    ~SimpleGuiComponent(){}
    
    IGUIElement* getComponent() { return m_active; }
    rect<s32> getRootPosition() { return rootPos;  }
private:
    IGUIWindow*     m_wndw;     // Parent window
    IGUIScrollBar*  m_scrollbar;
    IGUIImage*      m_image;
    IGUIEditBox*    m_editbox;
    IGUIButton*     m_button;
    IGUIStaticText* m_statictext;
    IGUICheckBox*   m_checkbox;
    
    IGUIElement* m_active;
    
    rect<s32> rootPos;

};

class SimpleGuiComponents {
public:
    //SimpleGuiComponents(){}
    SimpleGuiComponents()
    {
    }
    
    ~SimpleGuiComponents(){}
    // TODO: unique_ptr<SimpleGuiComponent*>
    void addGuiComponent(IGUIWindow* setWindow, IGUIScrollBar* setScrollBar, rect<s32> setRootPos)    { SimpleGuiComponentList.push_front(new SimpleGuiComponent(setWindow, setScrollBar, setRootPos)); }
    void addGuiComponent(IGUIWindow* setWindow, IGUIImage* setImage, rect<s32> setRootPos)            { SimpleGuiComponentList.push_front(new SimpleGuiComponent(setWindow, setImage, setRootPos));     }
    void addGuiComponent(IGUIWindow* setWindow, IGUIEditBox* setEditBox, rect<s32> setRootPos)        { SimpleGuiComponentList.push_front(new SimpleGuiComponent(setWindow, setEditBox, setRootPos));   }
    void addGuiComponent(IGUIWindow* setWindow, IGUIButton* setbutton, rect<s32> setRootPos)          { SimpleGuiComponentList.push_front(new SimpleGuiComponent(setWindow, setbutton, setRootPos));    }
    void addGuiComponent(IGUIWindow* setWindow, IGUIStaticText* setStaticText, rect<s32> setRootPos)  { SimpleGuiComponentList.push_front(new SimpleGuiComponent(setWindow, setStaticText, setRootPos));}
    void addGuiComponent(IGUIWindow* setWindow, IGUICheckBox* setCheckBox, rect<s32> setRootPos)      { SimpleGuiComponentList.push_front(new SimpleGuiComponent(setWindow, setCheckBox, setRootPos));  }
    
    void updateComponentPositions(int locationAsPixel)
    {
        for(list<SimpleGuiComponent*>::Iterator itr = SimpleGuiComponentList.begin(); itr != SimpleGuiComponentList.end(); itr++)
            ((SimpleGuiComponent*)*itr)->getComponent()->setRelativePosition(rect<s32>(((SimpleGuiComponent*)*itr)->getRootPosition().UpperLeftCorner.X,
                                                                ((SimpleGuiComponent*)*itr)->getRootPosition().UpperLeftCorner.Y - locationAsPixel,
                                                                ((SimpleGuiComponent*)*itr)->getRootPosition().LowerRightCorner.X,
                                                                ((SimpleGuiComponent*)*itr)->getRootPosition().LowerRightCorner.Y - locationAsPixel));
    }
private:
    list<SimpleGuiComponent*> SimpleGuiComponentList;

};

#endif // SIMPLEGUIOBJECTS
    