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
    stringw name;
    std::map<stringw, std::list<Element*>*> m_element_table;
public:
    Library(stringw name);
    std::list<stringw>   getCategoryList();
    std::list<Element*>  getElements(stringw s, stringw categ = "");
    ~Library();
};

#endif
