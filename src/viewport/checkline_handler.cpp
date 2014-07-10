#include "viewport/checkline_handler.hpp"

#include "editor.hpp"

// ----------------------------------------------------------------------------
ISceneNode* CheckLineHandler::startPlacingNew()
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end() && it->active) it++;
    m_check_lines.erase(it, m_check_lines.end());

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_clip.n1 = sm->addSphereSceneNode(0.2, 16, 0, ANOTHER_MAGIC_NUMBER);
    
    return m_clip.n1;

} // startPlacingNew

// ----------------------------------------------------------------------------
ISceneNode* CheckLineHandler::place()
{
    if (m_clip.n2)
    {
        m_clip.active = true;
        m_check_lines.push_back(m_clip);
        m_clip.n1 = 0;
        m_clip.n2 = 0;
        m_clip.active = true;
        return 0;
    }
    
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_clip.n2 = sm->addSphereSceneNode(0.2,16,0,ANOTHER_MAGIC_NUMBER);
    return m_clip.n2;
} // place

// ----------------------------------------------------------------------------
void CheckLineHandler::undo()
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end() && it->active) it++;
    if (it != m_check_lines.begin()) it--;

    it->active = false;
    it->n1->setVisible(false);
    it->n2->setVisible(false);
} // undo

// ----------------------------------------------------------------------------
void CheckLineHandler::redo()
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end() && it->active) it++;
    if (it != m_check_lines.end())
    {
        it->active = true;
        it->n1->setVisible(true);
        it->n2->setVisible(true);
    }
} // redo

// ----------------------------------------------------------------------------
void CheckLineHandler::cancelActive()
{
    if (m_clip.n1)
    {
        m_clip.n1->remove();
        m_clip.n1 = 0;
    }
    if (m_clip.n2)
    {
        m_clip.n2->remove();
        m_clip.n2 = 0;
    }
} // cancelActive

// ----------------------------------------------------------------------------
void CheckLineHandler::draw()
{
    IVideoDriver* vd = Editor::getEditor()->getVideoDriver();

    std::list<CheckLine>::iterator it = m_check_lines.begin();

    while (it != m_check_lines.end() && it->active)
    {
        vd->draw3DLine(it->n1->getPosition(), it->n2->getPosition(), SColor(255, 255, 0, 0));
        it++;
    }

} // draw
