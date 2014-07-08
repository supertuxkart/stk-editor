#ifndef TEX_SEL_HPP
#define TEX_SEL_HPP

#include <irrlicht.h>

#include <map>

using namespace irr;
using namespace gui;
using namespace core;
using namespace video;

class TexSel
{
public:
    static const int FIRST_BTN_ID = 601;
    static const int SEARCH_BOX   = 600;
private:

    // private variables:

    static TexSel*  m_self;

    IGUIWindow*     m_wndw;

    IGUIEditBox*    m_search_field;

    u8              m_btn_num;
    list<ITexture*> m_tex_list;
    IGUIButton**    m_btn_table;
    s32             m_selected_page;
    u8              m_selected_ix;
    bool            m_clicked;

    IGUIButton*     m_next;
    IGUIButton*     m_prev;

    // private functions:

    void    init();

    TexSel() {};
    
    void bindTexturesToButton(u32 page);
    void loadTextures();
    void initButtons();

public:
    static TexSel*      getTexSel();

    void        btnClicked(u32 id);

    u32         getBtnNum() { return m_btn_num; }

    void        show()      { m_wndw->setVisible(true); }
    void        hide()      { m_wndw->setVisible(false); }
};

#endif
