#ifndef ENVPANEL_HPP
#define ENVPANEL_HPP

#include <irrlicht.h>

#include<map>

class Library;

using namespace irr;
using namespace gui;
using namespace core;

class EnvPanel
{
public:
    static const int FIRST_BTN_ID = 302;
    static const int CB_ID = 301;
    static const int SF_ID = 300;
private:

    // private variables:

    static EnvPanel* m_env_panel;

    Library*        m_lib;
    IGUIComboBox*   m_cb;
    IGUIEditBox*    m_search_field;

    IGUIWindow*     m_wndw;
    unsigned int    m_index;
    IGUIButton*     m_next;
    IGUIButton*     m_prev;
    int             m_btn_num;

    std::pair<IGUIButton*, stringw> *m_btn_table;

    // private functions:

    void    init();
    void    initButtons();

    EnvPanel() {};

public:
    static EnvPanel*    getEnvPanel(IGUIWindow* wndw = 0);
    stringw             getModelPathFromBtnId(u32 ID);
    stringw             getModelNameFromBtnId(u32 ID);
    void                refreshBtnTable();
    void                reallocate(dimension2du ss);

    void                switchPage(int dir);
    int                 getBtnNum()                 { return m_btn_num; }
    void                resetIndex()                { m_index = 0;      }
};

#endif
