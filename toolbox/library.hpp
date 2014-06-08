#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include <irrlicht.h>

#include <list>
#include <map>

using namespace irr;
using namespace core;

class Element;

class Library
{
private:
    stringw      m_name;
    unsigned int m_buffer_size;
    
    std::map<stringw, std::list<Element*>*> m_element_table;

    std::list<Element*> m_selected_elements;

public:
    Library(stringw name, unsigned int buffer_size = 12);

    std::list<stringw>   getCategoryList();
    void                 selectElements(stringw s, stringw categ = "");
    std::list<Element*>  getElements(unsigned int ix = 0);

    unsigned int         getSelectionPageNum()  
                { return m_selected_elements.size() / m_buffer_size + 1; }

    ~Library();
};

#endif
