#include "viewport/checkline_handler.hpp"

#include "editor.hpp"

// ----------------------------------------------------------------------------
ISceneNode* CheckLineHandler::startPlacingNew()
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end() && it->active) ++it;
    m_check_lines.erase(it, m_check_lines.end());

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_clip.n1 = sm->addSphereSceneNode(0.2f, 16, 0, ANOTHER_MAGIC_NUMBER);

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
    m_clip.n2 = sm->addSphereSceneNode(0.2f,16,0,ANOTHER_MAGIC_NUMBER);
    return m_clip.n2;
} // place

// ----------------------------------------------------------------------------
void CheckLineHandler::undo()
{
    if (m_check_lines.size() == 0) return;

    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end() && it->active) ++it;
    if (it != m_check_lines.begin()) --it;

    it->active = false;
    it->n1->setVisible(false);
    it->n2->setVisible(false);
} // undo

// ----------------------------------------------------------------------------
void CheckLineHandler::redo()
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end() && it->active) ++it;
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
        ++it;
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
        ++it;
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
        ++it;
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
                ++it;
            }
            else
            {
                p1 = it->n1->getPosition();
                p2 = it->n2->getPosition();
                (*scene) << "    <check-line kind=\"activate\" other-ids=\"" << j + 2;
                (*scene) << "\" p1=\"" << p1.X << " " << p1.Y << " " << p1.Z << "\" p2=\"" << p2.X;
                (*scene) << " " << p2.Y << " " << p2.Z << "\" same-group=\"" << j + 1 << "\"/>\n";
                ++it;
                j++;
            }
        }

        std::list<CheckLine>::iterator it2 = m_check_lines.begin();
        for (it2 = m_check_lines.begin(); it2 != m_check_lines.end(); ++it2)
            if (!it->removed && it->active) it = it2;

        p1 = it->n1->getPosition();
        p2 = it->n2->getPosition();
        (*scene) << "    <check-line kind=\"activate\" other-ids=\"" << 0;
        (*scene) << "\" p1=\"" << p1.X << " " << p1.Y << " " << p1.Z << "\" p2=\"" << p2.X;
        (*scene) << " " << p2.Y << " " << p2.Z << "\" same-group=\"" << j+1 << "\"/>\n";
    }
    else (*scene) << "/>\n";

    (*scene) << "  </checks>\n";

} // build

// ----------------------------------------------------------------------------
void CheckLineHandler::save(FILE* fp)
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    u32 size = 0;
    while (it != m_check_lines.end() && it->active)
    {
        if (!it->removed) size++;
        ++it;
    }
    fwrite(&size, sizeof(u32), 1, fp);
    
    it = m_check_lines.begin();
    while (it != m_check_lines.end() && it->active)
    {
        if (!it->removed)
        {
            fwrite(&it->n1->getPosition(), sizeof(vector3df), 1, fp);
            fwrite(&it->n2->getPosition(), sizeof(vector3df), 1, fp);
        }
        ++it;
    }
} // save

// ----------------------------------------------------------------------------
void CheckLineHandler::reload(FILE* fp)
{
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    while (it != m_check_lines.end())
    {
        it->n1->remove();
        it->n1 = 0;
        it->n2->remove();
        it->n2 = 0;
        ++it;
    }
    m_check_lines.clear();
    
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    
    u32 size;
    fread(&size, sizeof(u32), 1, fp);
    for (u32 i = 0; i < size; i++)
    {
        vector3df n1, n2;
        fread(&n1, sizeof(vector3df), 1, fp);
        fread(&n2, sizeof(vector3df), 1, fp);
        CheckLine cl;
        cl.n1 = sm->addSphereSceneNode(0.2f, 16, 0, ANOTHER_MAGIC_NUMBER);
        cl.n2 = sm->addSphereSceneNode(0.2f, 16, 0, ANOTHER_MAGIC_NUMBER);
        cl.n1->setPosition(n1);
        cl.n2->setPosition(n2);
        cl.active = true;
        m_check_lines.push_back(cl);
        cl.n1 = 0;
        cl.n2 = 0;
    }
} // reload

// ----------------------------------------------------------------------------
void CheckLineHandler::draw()
{
    IVideoDriver* vd = Editor::getEditor()->getVideoDriver();
    std::list<CheckLine>::iterator it = m_check_lines.begin();
    SMaterial m;
    vd->setMaterial(m);
    vd->setTransform(video::ETS_WORLD, core::IdentityMatrix);

    while (it != m_check_lines.end() && it->active)
    {
        if (!it->removed)
        {
            vd->draw3DLine(it->n1->getPosition(), it->n2->getPosition(),
                                                SColor(255, 255, 0, 0));
        }
        ++it;
    }
} // draw
