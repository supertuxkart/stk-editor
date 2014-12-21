#include "gui/new_dialog_wndw.hpp"

#include "editor.hpp"


NewDialogWndw* NewDialogWndw::m_self = 0;

// ----------------------------------------------------------------------------
void NewDialogWndw::init()
{
    Editor* editor = Editor::getEditor();
    IGUIEnvironment* gui_env = editor->getGUIEnv();
    IGUIFont* font = editor->getFont();
    dimension2du ss = editor->getScreenSize();

    rect<s32> frame = rect<s32>((s32)(ss.Width  / 2.0f - 150), 
                                (s32)(ss.Height / 2.0f - 100),
                                (s32)(ss.Width  / 2.0f + 150),
                                (s32)(ss.Height / 2.0f + 130));
    m_wndw = gui_env->addWindow(frame, false, _(L"New Track"), 0);

    gui_env->addStaticText(_(L"Track name:"), rect<s32>(10, 30, 130, 50),
        false, false, m_wndw, -1, false)->setOverrideFont(font);
    gui_env->addStaticText(_(L"Designer:"), rect<s32>(10, 60, 130, 80),
        false, false, m_wndw, -1, false)->setOverrideFont(font);
    gui_env->addStaticText(_(L"File name:"), rect<s32>(10, 90, 130, 110),
        false, false, m_wndw, -1, false)->setOverrideFont(font);
    gui_env->addStaticText(_(L"Size:"), rect<s32>(10, 120, 130, 140),
        false, false, m_wndw, -1, false)->setOverrideFont(font);

    frame        =  rect<s32>(150, 30, 280, 50);
    m_track_name =  gui_env->addEditBox(L"", frame, true, m_wndw);
    m_track_name -> setText(_(L"RenameMePls!"));
    frame        =  rect<s32>(150, 60, 280, 80);
    m_designer   =  gui_env->addEditBox(L"", frame, true, m_wndw);
    m_designer   -> setText(_(L"TheGreatTux"));
    frame        =  rect<s32>(150, 90, 280, 110);
    m_file_name  =  gui_env->addEditBox(L"", frame, true, m_wndw);
    m_file_name  -> setText(_(L"file_name"));

    frame = rect<s32>(150, 120, 280, 140);
    m_size_cb = gui_env->addComboBox(frame, m_wndw);

    m_size_cb->addItem(_(L"Small"      ),  50);
    m_size_cb->addItem(_(L"Medium"     ),  125);
    m_size_cb->addItem(_(L"Large"      ),  250);
    m_size_cb->addItem(_(L"Extra Large"),  350);

    m_grav_road_check = gui_env->addCheckBox(false, rect<s32>(80, 160, 220, 180), 
                                                    m_wndw, -1, L"  Gravity Roads");
    
    frame = rect<s32>(180, 195, 250, 215);
    gui_env->addButton(frame, m_wndw, BTN_ID,     _(L"Create"), _(L"Create"));

    frame = rect<s32>(40, 195, 110, 215);
    gui_env->addButton(frame, m_wndw, BTN_ID + 1, _(L"Cancel"), _(L"Cancel"));

    m_wndw->setDrawTitlebar(false);
    m_wndw->getCloseButton()->setVisible(false);

} // init

// ----------------------------------------------------------------------------
NewDialogWndw* NewDialogWndw::get()
{
    if (m_self != 0) return m_self;

    m_self = new NewDialogWndw();
    m_self->init();
    return m_self;
} // get

// ----------------------------------------------------------------------------
u32 NewDialogWndw::getSize()
{
    return m_size_cb->getItemData(m_size_cb->getSelected());
} // getSize

// ----------------------------------------------------------------------------
void NewDialogWndw::reallocate(dimension2du ss)
{
    m_wndw->setRelativePosition(position2di(ss.Width / 2 - 150,
                                            ss.Height / 2 - 100));
} // reallocate

