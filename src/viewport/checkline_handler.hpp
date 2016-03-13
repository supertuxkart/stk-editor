#ifndef CHECKLINE_HANDLER_HPP
#define CHECKLINE_HANDLER_HPP


#include <irrlicht.h>
#include <iostream>
#include <fstream>
#include <list>

using namespace irr;
using namespace scene;
using namespace core;

struct CheckLine
{
    ISceneNode* n1;
    ISceneNode* n2;
    bool active;
    bool removed;

    CheckLine()
    {
        n1 = 0;
        n2 = 0;
        active  = false;
        removed = false;
    }

    ~CheckLine()
    {
        if (n1) n1->remove();
        if (n2) n2->remove();
    }
};

class CheckLineHandler
{
private:
    std::list<CheckLine> m_check_lines;
    // check_line in progress
    CheckLine        m_clip;
public:
    CheckLineHandler() {};

    ISceneNode* startPlacingNew();
    ISceneNode* place();

    void undo();
    void redo();
    
    bool isCheckLine(ISceneNode* n);
    void remove(ISceneNode* n, bool remove);

    void cancelActive();

    void build(std::ofstream* scene);
    void save(FILE* fp);
    void reload(FILE* fp);

    void clearCheckLines();
    void draw();

    ~CheckLineHandler(){};
};

#endif
