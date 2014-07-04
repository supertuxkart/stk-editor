#include "track.hpp"
#include "editor.hpp"

#include "mesh/terrain.hpp"
#include "mesh/driveline.hpp"

#include <fstream>
#include <iostream>
#include <stdio.h>

// ----------------------------------------------------------------------------
Track::~Track()
{
    if (m_terrain)
        delete m_terrain;
}

// ----------------------------------------------------------------------------
void Track::create(u32 tx, u32 tz)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, tx, tz, 2 * tx, 2 * tz);
}

// ----------------------------------------------------------------------------
void Track::load(path file)
{
    delete m_terrain;

    FILE* pFile;
    pFile = fopen(file.c_str(), "rb");

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, pFile);
    fclose(pFile);

} // load

// ----------------------------------------------------------------------------
void Track::save(path file)
{
  FILE* pFile;

  char buffer[] = { 'x' , 'y' , 'z' };
  pFile = fopen(file.c_str(), "wb");
  
  m_terrain->save(pFile);

  fclose(pFile);
} // save

// ----------------------------------------------------------------------------
void Track::quit()
{
    if (m_terrain)
        delete m_terrain;
}

// ----------------------------------------------------------------------------
void Track::build()
{
    m_terrain->build();
    m_driveline->build();

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    
    std::ofstream scene;
    scene.open("export/scene.xml");

    scene << "<scene>\n";
    scene << "  <track model=\"track.obj\" x=\"0\" y=\"0\" z=\"0\">\n";

    ISceneNode* node;
    int i = 1;
    while (node = sm->getSceneNodeFromId(MAGIC_NUMBER + i))
    {
        vector3df pos, rot, sca;
        if (node->isVisible())
        {
            pos   = node->getPosition();
            rot   = node->getRotation();
            sca = node->getScale();
            scene << "    <static-object model=\"" << node->getName() << "\" xyz=\"";
            scene << pos.X << " " << pos.Y << " " << pos.Z << "\" hpr=\"";
            scene << rot.X << " " << rot.Y << " " << rot.Z << "\" scale=\"";
            scene << sca.X << " " << sca.Y << " " << sca.Z << "\"/>\n";
        }
        i++;
    }

    scene << "  </track>\n";
    
    scene << "  <default - start karts - per - row = \"3\"\n";
    scene << "                   forwards-distance =\"1.50\"\n";
    scene << "                   sidewards-distance=\"3.00\"\n";
    scene << "                   upwards-distance  =\"0.10\"/>\n";
    
    scene << "</scene>\n";
    scene.close();
}
