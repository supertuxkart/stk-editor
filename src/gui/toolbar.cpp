#include "gui/toolbar.hpp"

#include "editor.hpp"

ToolBar* ToolBar::m_toolbar = 0;

// ----------------------------------------------------------------------------
void ToolBar::init()
{
    Editor* editor = Editor::getEditor();
    path icons = editor->getIconsLoc();
    m_bar      = editor->getGUIEnv()->addToolBar();
    m_bar->setMinSize(dimension2du(0,50));

    buttonInit(0,  TBI_NEW,         (icons + "new.png").c_str(),      _(L"New (ctrl+n)"));
    buttonInit(1,  TBI_OPEN,        (icons + "open.png").c_str(),     _(L"Open (ctrl+o)"));
    buttonInit(2,  TBI_SAVE,        (icons + "save.png").c_str(),     _(L"Save (ctrl+s)"));
    buttonInit(3,  TBI_EXPORT ,     (icons + "save_as.png").c_str(),  _(L"Export (ctrl+shift+s)"));
    buttonInit(4,  TBI_UNDO,        (icons + "undo.png").c_str(),     _(L"Undo (ctrl+z)"));
    buttonInit(5,  TBI_REDO,        (icons + "redo.png").c_str(),     _(L"Redo (ctrl+y)"));
    buttonInit(6,  TBI_SELECT,      (icons + "select.png").c_str(),   _(L"Select (shift+a)"));
    buttonInit(7,  TBI_MOVE,        (icons + "move.png").c_str(),     _(L"Move (shift+g)"));
    buttonInit(8,  TBI_ROTATE,      (icons + "rotate.png").c_str(),   _(L"Rotate (shift+r)"));
    buttonInit(9,  TBI_SCALE,       (icons + "scale.png").c_str(),    _(L"Scale (shift+s)"));
    buttonInit(10, TBI_DELETE,      (icons + "delete.png").c_str(),   _(L"Delete (del)"));
    buttonInit(11, TBI_CAM,         (icons + "cam1.png").c_str(),     _(L"Toggle camera mode (c)"));
    buttonInit(12, TBI_RECAM,       (icons + "cam2.png").c_str(),     _(L"Restore camera state (NUM1)"));
    buttonInit(13, TBI_DRIVELINE,   (icons + "spline.png").c_str(),   _(L"Select DriveLine (r)"));  
    buttonInit(14, TBI_HIDE_TERRAIN,(icons + "ht.png").c_str(),       _(L"Hide terrain (t)"));
    buttonInit(15, TBI_MUSIC,       (icons + "music.png").c_str(),    _(L"Music"));
    buttonInit(16, TBI_TRY,         (icons + "try.png").c_str(),      _(L"Try your track"));
    buttonInit(17, TBI_EXIT,        (icons + "exit.png").c_str(),     _(L"Quit (esc)"));

    reallocate();

} // init

void ToolBar::buttonInit(unsigned int ix, ToolboxButtonIndex TBI, 
                    const char* file_path, const wchar_t* text)
{
    ITexture* img;
    img = Editor::loadImg(file_path);
    m_buttons[ix] = m_bar->addButton(TBI, 0, text, img, 0, false, true);
    m_buttons[ix]->setMaxSize(dimension2du(40, 40));
    m_buttons[ix]->setMinSize(dimension2du(40, 40));
} // buttonInit

// ----------------------------------------------------------------------------
ToolBar* ToolBar::getToolBar()
{
    if (m_toolbar != 0) return m_toolbar;

    m_toolbar = new ToolBar();
    m_toolbar->init();
    return m_toolbar;
} // getToolBar

// ----------------------------------------------------------------------------
int min(int x, int y)
{
    if (x < y) return x;
    else return y;
} // min

// ----------------------------------------------------------------------------
void ToolBar::reallocate()
{
    unsigned int screen_x = Editor::getEditor()->getScreenSize().Width;
    int max_dist_between_blocks = 50;

    int space_between_blocks = min((screen_x - 18 * 50) / 6, max_dist_between_blocks);

    // first 4 icon - new, open, save, save as
    int block_begin = 5;
    for (int i = 0; i < 4; i++)
        m_buttons[i]->setRelativePosition(position2di(block_begin + i * 50, 5));

    // second block: 2 icon - undo, redo
    block_begin += space_between_blocks;
    for (int i = 4; i < 6; i++)
        m_buttons[i]->setRelativePosition(position2di(block_begin + i * 50, 5));

    // fourth block: 5 icon - select, move, rotate, scale, delete
    block_begin += space_between_blocks;
    for (int i = 6; i < 11; i++)
        m_buttons[i]->setRelativePosition(position2di(block_begin + i * 50, 5));

    // fifth block: switch camera, restore def camera pos,rot
    block_begin += space_between_blocks;
    m_buttons[11]->setRelativePosition(position2di(block_begin + 11 * 50, 5));
    m_buttons[12]->setRelativePosition(position2di(block_begin + 12 * 50, 5));

    // Sixth block: driveline,hide terrain, music
    block_begin += space_between_blocks;
    m_buttons[13]->setRelativePosition(position2di(block_begin + 13 * 50, 5));
    m_buttons[14]->setRelativePosition(position2di(block_begin + 14 * 50, 5));
    m_buttons[15]->setRelativePosition(position2di(block_begin + 15 * 50, 5));

    // test
    block_begin += space_between_blocks;
    m_buttons[16]->setRelativePosition(position2di(block_begin + 16 * 50, 5));

    // last block - quit
    m_buttons[17]->setRelativePosition(position2di(screen_x - 50, 5));
} // reallocate
