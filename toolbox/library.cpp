#include "toolbox/library.hpp"
#include "toolbox/element.hpp"

#include "editor.hpp"
#include <iostream>

using namespace io;

// ----------------------------------------------------------------------------
Library::Library(stringw name, unsigned int buffer_size)
{
    m_buffer_size = buffer_size;
    m_name        = name;
    IFileSystem* file_system = Editor::getEditor()->getDevice()->getFileSystem();
    path wd = file_system->getWorkingDirectory();

    IFileArchive* dir = 0;
    stringw dir_path = "/libraries/";
    dir_path += name;
    dir_path += "/xml";

    if (!file_system->addFileArchive(wd + dir_path, true, false, EFAT_FOLDER, "", &dir))
    {
        std::cerr << "The data library could not be found. \
                      Later it will be included in STK media repo, \
                      but for now you need to download it separately. \
                      Did you read the README ?";
        exit(-1);
    }

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
void Library::selectElements(stringw s, stringw categ)
{
    m_selected_elements.clear();
    if (categ == "All")
    {
        std::map<stringw, std::list<Element*>*>::iterator it;
        for (it = m_element_table.begin(); it != m_element_table.end(); ++it)
        {
            std::list<Element*>::iterator iit;
            for (iit = (*it).second->begin(); iit != (*it).second->end(); ++iit)
                if ((*iit)->tagBeginsWith(s)) m_selected_elements.push_back(*iit);
        } // category iteration
    } // categ == ""
    else
    {
        std::map<stringw, std::list<Element*>*>::iterator it = m_element_table.find(categ);
        if (it != m_element_table.end())
        {
            std::list<Element*>::iterator iit;
            for (iit = (*it).second->begin(); iit != (*it).second->end(); ++iit)
                if ((*iit)->tagBeginsWith(s)) m_selected_elements.push_back(*iit);
        }
    } // categ != ""

} // selectElements

// ----------------------------------------------------------------------------
std::list<Element*> Library::getElements(unsigned int ix)
{
    std::list<Element*> elements;
    std::list<Element*>::iterator it = m_selected_elements.begin();

    for (unsigned int i = 0; i < ix * m_buffer_size; i++, it++);

    for (unsigned int i = 0; i < m_buffer_size && it != m_selected_elements.end(); i++)
    {
        elements.push_back(*it);
        it++;
    }

    return elements;
} // getElements

// ----------------------------------------------------------------------------
unsigned int Library::getSelectionPageNum()
{
    if (m_selected_elements.empty()) return 1;
    int pn = m_selected_elements.size() / m_buffer_size;
    if (m_selected_elements.size() % m_buffer_size != 0) pn++;
    return pn;
} // getSelectionPageNum()


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
