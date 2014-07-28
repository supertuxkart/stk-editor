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
bool CheckLineHandler::isCheckLine(ISceneNode* n)
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end())
    {
        if (&(*it->n1) == n || &(*it->n2) == n) return true;
        it++;
    }
    return false;
} // isCheckLine

// ----------------------------------------------------------------------------
void CheckLineHandler::remove(ISceneNode* n, bool remove)
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end())
    {
        if (it->n1 == n || it->n2 == n)
        {
            it->removed = remove;
            it->n1->setVisible(!remove);
            it->n2->setVisible(!remove);
            return;
        }
        it++;
    }
} // remove

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
void CheckLineHandler::build(std::ofstream* scene)
{
    (*scene) << "  <checks>\n";
    (*scene) << "    <check-lap kind=\"lap\" same-group=\"0\"";
    
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    u32 i = 0;
    while (it != m_check_lines.end() && it->active)
    {
        if (!it->removed) i++;
        it++;
    }

    if (i>0)
    {
       (*scene) << " other-ids = \"1\" />\n";
        u32 j = 0;
        vector3df p1, p2;
        for (it = m_check_lines.begin(); j < i - 1 && it != m_check_lines.end();)
        {
            if (it->removed)
            {
                it++;
            }
            else
            {
                p1 = it->n1->getPosition();
                p2 = it->n2->getPosition();
                (*scene) << "    <check-line kind=\"activate\" other-ids=\"" << j + 2;
                (*scene) << "\" p1=\"" << p1.X << " " << p1.Z << "\" p2=\"" << p2.X;
                (*scene) << " " << p2.Z << "\" same-group=\"" << j + 1 << "\"/>\n";
                it++;
                j++;
            }
        }

        p1 = it->n1->getPosition();
        p2 = it->n2->getPosition();
        f32 h = (p1.Y + p2.Y) / 2.0 - 1;
        (*scene) << "    <check-line kind=\"activate\" other-ids=\"" << 0;
        (*scene) << "\" p1=\"" << p1.X << " " << p1.Y << " " << p1.Z << "\" p2=\"" << p2.X;
        (*scene) << " " << p2.Y << " " << p2.Z << "\" same-group=\"" << j+1 << "\"/>\n";
    }
    else (*scene) << "/>\n";

    (*scene) << "  </checks>\n";

} // build

// ----------------------------------------------------------------------------
void CheckLineHandler::draw()
{
    IVideoDriver* vd = Editor::getEditor()->getVideoDriver();

    std::list<CheckLine>::iterator it = m_check_lines.begin();

    while (it != m_check_lines.end() && it->active)
    {
        if (!it->removed)
            vd->draw3DLine(it->n1->getPosition(), it->n2->getPosition(), SColor(255, 255, 0, 0));
        it++;
    }

} // draw
