#include "toolbar.hpp"

#include "editor.hpp"
#include <iostream>

ToolBar* ToolBar::m_toolbar = 0;

// ----------------------------------------------------------------------------
void ToolBar::init()
{
    m_bar = Editor::getEditor()->getGUIEnv()->addToolBar();
    m_bar->setMinSize(dimension2du(0,50));

    buttonInit(0,  TBI_NEW,         "new.png",      L"New");
    buttonInit(1,  TBI_OPEN,        "open.png",     L"Open");
    buttonInit(2,  TBI_SAVE,        "save.png",     L"Save");
    buttonInit(3,  TBI_SAVE_AS,     "save_as.png",  L"Save as");
    buttonInit(4,  TBI_UNDO,        "undo.png",     L"Undo");
    buttonInit(5,  TBI_REDO,        "redo.png",     L"Redo");
    buttonInit(6,  TBI_CAM,         "cam1.png",     L"Free Camera Mode");
    buttonInit(7,  TBI_SELECT,      "select.png",   L"Select");
    buttonInit(8,  TBI_MOVE,        "move.png",     L"Move");
    buttonInit(9, TBI_ROTATE,      "rotate.png",   L"Rotate");
    buttonInit(10, TBI_DELETE,      "delete.png",   L"Delete");
    buttonInit(11, TBI_GRID_ON_OFF, "grid.png",     L"Grid On / Off");
    buttonInit(12, TBI_GRID_INC,    "gridinc.png",  L"Increase grid density");
    buttonInit(13, TBI_GRID_DEC,    "griddec.png",  L"Decrease grid density");
    buttonInit(14, TBI_TRY,         "try.png",      L"Try your track");
    buttonInit(15, TBI_SETTINGS,    "settings.png", L"Settings");
    buttonInit(16, TBI_EXIT,        "exit.png",     L"Quit");

    reallocate();

} // init

// ----------------------------------------------------------------------------
ITexture* ToolBar::loadImg(const char* file_path)
{
    IVideoDriver* driver = Editor::getEditor()->getVideoDriver();
    ITexture* img = driver->getTexture(file_path);
    if (img == 0)
    {
        std::cerr << "Missing image: " << file_path << std::endl;
    }
    return img;
} // loadImg

void ToolBar::buttonInit(unsigned int ix, ToolboxButtonIndex TBI, 
                    const char* file_path, const wchar_t* text)
{
    ITexture* img;
    img = loadImg(file_path);
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

    // first 4 icon - new, open, save, save as
    int block_begin = 5;
    for (int i = 0; i < 4; i++)
        m_buttons[i]->setRelativePosition(position2di(block_begin + i * 50, 5));

    // second block: 2 icon - undo, redo
    block_begin += min((screen_x - 18 * 50) / 6, max_dist_between_blocks);
    for (int i = 4; i < 6; i++)
        m_buttons[i]->setRelativePosition(position2di(block_begin + i * 50, 5));

    // fourth block: 5 icon - free camera, select, move, rotate, delete
    block_begin += min((screen_x - 18 * 50) / 6, max_dist_between_blocks);
    for (int i = 6; i < 11; i++)
        m_buttons[i]->setRelativePosition(position2di(block_begin + i * 50, 5));

    // fifth block: 3 icon - grid, grid+, grid-
    block_begin += min((screen_x - 18 * 50) / 6, max_dist_between_blocks);
    for (int i = 11; i < 14; i++)
        m_buttons[i]->setRelativePosition(position2di(block_begin + i * 50, 5));

    // test
    block_begin += min((screen_x - 18 * 50) / 6, max_dist_between_blocks);
    m_buttons[14]->setRelativePosition(position2di(block_begin + 15 * 50, 5));

    // last block: 2 icon - settings, quit
    m_buttons[15]->setRelativePosition(position2di(screen_x - 100, 5));
    m_buttons[16]->setRelativePosition(position2di(screen_x - 50, 5));
} // reallocate
