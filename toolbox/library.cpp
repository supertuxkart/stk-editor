#include "toolbox/library.hpp"
#include "toolbox/element.hpp"

#include "editor.hpp"

using namespace io;

// ----------------------------------------------------------------------------
Library::Library(stringw name)
{
    IFileSystem* file_system = Editor::getEditor()->getDevice()->getFileSystem();
    path wd = file_system->getWorkingDirectory();

    IFileArchive* dir;
    stringw dir_path = "/libraries/";
    dir_path += name;
    dir_path += "/xml";

    file_system->addFileArchive(wd+dir_path,true,false,EFAT_FOLDER,"",&dir);

    const IFileList* file_list = dir->getFileList();

    for (unsigned int i = 0; i < file_list->getFileCount(); i++)
    {
        IXMLReader* xml_reader = file_system->createXMLReader(
            file_list->getFullFileName(i));

        Element* element = new Element(xml_reader);
        if (element->isValid())
        {
            stringw categ = element->getCategory();
            std::map<stringw, std::list<Element*>*>::iterator it;
            it = m_element_table.find(categ);
            if (it == m_element_table.end())
            {
                // category doesn't exist -> create
                std::list<Element*>* list = new std::list<Element*>();
                list->push_back(element);
                std::pair<stringw, std::list<Element*>*> p(categ, list);
                m_element_table.insert(p);
            }
            else
            {
                (*it).second->push_back(element);
            }
        }
    }

} // Library


// ----------------------------------------------------------------------------
std::list<stringw> Library::getCategoryList()
{
    std::list<stringw> list;
    std::map<stringw, std::list<Element*>*>::iterator it;
    for (it = m_element_table.begin(); it != m_element_table.end(); ++it)
        list.push_back((*it).first);
    return list;
} // getCategoryList

// ----------------------------------------------------------------------------
std::list<Element*> Library::getElements(stringw s, stringw categ)
{
    std::list<Element*> elements;

    if (categ == "")
    {
        std::map<stringw, std::list<Element*>*>::iterator it;
        for (it = m_element_table.begin(); it != m_element_table.end(); ++it)
        {
            std::list<Element*>::iterator iit;
            for (iit = (*it).second->begin(); iit != (*it).second->end(); ++iit)
                if ((*iit)->tagBeginsWith(s)) elements.push_back(*iit);
        } // category iteration
    } // categ == ""
    else
    {
        std::map<stringw, std::list<Element*>*>::iterator it = m_element_table.find(categ);
        if (it != m_element_table.end())
        {
            std::list<Element*>::iterator iit;
            for (iit = (*it).second->begin(); iit != (*it).second->end(); ++iit)
                if ((*iit)->tagBeginsWith(s)) elements.push_back(*iit);
        }
    } // categ != ""

    return elements;

} // getElements


// ----------------------------------------------------------------------------
Library::~Library()
{
    std::map<stringw, std::list<Element*>*>::iterator it;
    for (it = m_element_table.begin(); it != m_element_table.end(); ++it)
    {
        std::list<Element*> list = *(*it).second;
        std::list<Element*>::iterator iit;
        for (iit = list.begin(); iit != list.end(); iit++)
            delete (*iit);
        delete (*it).second;
    }

} // ~Library
