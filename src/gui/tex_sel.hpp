#ifndef TEX_SEL_HPP
#define TEX_SEL_HPP

#include <irrlicht.h>

#include <map>

using namespace irr;
using namespace gui;
using namespace core;
using namespace video;

class ISubscriber;

class TexSel
{
public:
    static const int FIRST_BTN_ID = 601;
    static const int FIRST_TEX_BTN_ID = 604;
    static const int SEARCH_BOX   = 600;
private:

    // private variables:

    static TexSel*                      m_self;

    IGUIWindow*                         m_wndw;

    IGUIEditBox*                        m_search_field;

    u8                                  m_btn_num;
    list<ITexture*>                     m_tex_list;
    std::pair<IGUIButton*, ITexture*>*  m_btn_table;
    u32                                 m_selected_page;

    IGUIButton*                         m_next;
    IGUIButton*                         m_prev;
    IGUIButton*                         m_cancel;

    list<ISubscriber*>                  m_subs;
    list<IGUIButton*>                   m_bsubs;

    // private functions:

    void    init();

    TexSel() {};
    
    bool    texContainsString(ITexture* tex, stringw str);
    void    bindTexturesToButton(u32 page);
    void    loadTextures();
    void    initButtons();
    u32     properTexNum();
public:
    static TexSel*      getTexSel();

    void        btnClicked(u32 id);
    void        notify(u32 id);
    void        searchFieldDirty();

    u32         getBtnNum()              { return m_btn_num; }

    void        show()                   { m_wndw->setVisible(true);          }
    void        hide()                   { m_wndw->setVisible(false); clear();}
    bool        isActive()               { return m_wndw->isVisible();        }
    void        clear()                  { m_bsubs.clear(); m_subs.clear();   }

    void        subscribe(IGUIButton* b) { m_bsubs.push_back(b); show();      }
    void        subscribe(ISubscriber* s){ m_subs.push_back(s);  show();      }
    
    void        reallocate();
};

#endif
