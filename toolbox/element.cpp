#include "toolbox/element.hpp"

// ----------------------------------------------------------------------------
Element::Element(io::IXMLReader* xml)
{
    if (!xml)
    {
        m_valid = false;
        return;
    }

    const stringw img(L"img");
    const stringw model(L"model");
    const stringw tag(L"tag");

    while (xml->read())
    {
        if (xml->getNodeType() == EXN_ELEMENT)
        {
            if (img.equals_ignore_case(xml->getNodeName()))            
                m_img = xml->getAttributeValueSafe(L"path");
            else if (model.equals_ignore_case(xml->getNodeName()))
                m_model = xml->getAttributeValueSafe(L"path");
            else if (tag.equals_ignore_case(xml->getNodeName()))
                m_tags.push_back(xml->getAttributeValueSafe(L"name"));
        }
    }

    m_valid = true;
}

// ----------------------------------------------------------------------------
bool Element::tagBeginsWith(const stringw& s)
{
    std::list<stringw>::iterator it;
    for (it = m_tags.begin(); it != m_tags.end(); ++it)
    {
        if ((*it).equalsn(s, s.size())) return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
bool Element::nameBeginsWith(const stringw& s)
{
    return m_name.equalsn(s, s.size());
}

