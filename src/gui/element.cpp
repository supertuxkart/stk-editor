#include "gui/element.hpp"

// ----------------------------------------------------------------------------
Element::Element(io::IXMLReader* xml)
{
    if (!xml)
    {
        m_valid = false;
        return;
    }

    const stringw library(L"library");
    const stringw object(L"object");
    const stringw img(L"img");
    const stringw model(L"model");
    const stringw tag(L"tag");
    bool lib = false;

    while (xml->read())
    {
        if (xml->getNodeType() == EXN_ELEMENT)
        {
            if (library.equals_ignore_case(xml->getNodeName()))
            {
                m_name     = xml->getAttributeValueSafe(L"name");
                m_category = xml->getAttributeValueSafe(L"category");
                lib = true;
            }
            else if (object.equals_ignore_case(xml->getNodeName()))
            {
                m_name = xml->getAttributeValueSafe(L"name");
                m_category = xml->getAttributeValueSafe(L"category");
                lib = false;
            }
            else if (img.equals_ignore_case(xml->getNodeName()))            
                m_img = xml->getAttributeValueSafe(L"path");
            else if (model.equals_ignore_case(xml->getNodeName()))
                m_model = xml->getAttributeValueSafe(L"path");
            else if (tag.equals_ignore_case(xml->getNodeName()))
                m_tags.push_back(xml->getAttributeValueSafe(L"name"));
        }
    }
    if (lib)
        m_model = stringc("library/")  + m_model;
    else m_model = stringc("obj/") + m_model;
    m_valid = true;
} // Element

// ----------------------------------------------------------------------------
bool Element::tagBeginsWith(const stringw& s)
{
    if (s == "") return true;
    std::list<stringw>::iterator it;
    for (it = m_tags.begin(); it != m_tags.end(); ++it)
    {
        if ((*it).equalsn(s, s.size())) return true;
    }
    return m_name.equalsn(s, s.size());
}
