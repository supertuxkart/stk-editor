#include "track.hpp"
#include "editor.hpp"

#include "viewport/viewport.hpp"

#include "mesh/terrain.hpp"
#include "mesh/driveline.hpp"
#include "mesh/road.hpp"
#include "spline/bezier.hpp"
#include "spline/tcr.hpp"
#include "b3d/B3DMeshWriter.h"

#include <physfs.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <assert.h>

// ----------------------------------------------------------------------------
Track::Track(f32 tx, f32 tz)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, tx, tz,
                           (u32) (2 * tx), (u32) (2 * tz));

    ISpline* spline = new TCR(sm->getRootSceneNode(), sm, 0);
    m_driveline = new DriveLine(sm->getRootSceneNode(), sm, 0, spline, L"DriveLine");
    m_roads.push_back(m_driveline);
    m_music = "Origin.music";

} // Track

// ----------------------------------------------------------------------------
Track::Track(path file)
{
    FILE* pFile;
    pFile = fopen(file.c_str(), "rb");

    if (!pFile)
    {
        std::cerr << "File is removed or corrupted.\n Sorry! Terminating...\n";
        exit(-1);
    }

    // TRACK NAME
    u8 size;
    wchar_t* c;
    fread(&size, sizeof(u8), 1, pFile);
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, pFile);
    m_track_name = c;
    delete c;

    // DESIGNER NAME
    fread(&size, sizeof(u8), 1, pFile);
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, pFile);
    m_designer = c;
    delete c;

    // FILE NAME
    c8* cc;
    fread(&size, sizeof(u8), 1, pFile);
    cc = new c8[size];
    fread(cc, sizeof(c8), size, pFile);
    m_file_name = cc;
    delete cc;

    // MUSIC
    cc;
    fread(&size, sizeof(u8), 1, pFile);
    cc = new c8[size];
    fread(cc, sizeof(c8), size, pFile);
    m_music = cc;
    delete cc;

    // TERRAIN
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, pFile);

    // SKY

    Sky* s = Viewport::get()->getSky();
    delete s;
    
    s = new Sky(pFile);
    Viewport::get()->setSky(s);

    // ROADS

    IRoad* r;
    fread(&size, sizeof(u8), 1, pFile);
    if (size > 0)
    {
        m_driveline = new DriveLine(sm->getRootSceneNode(), sm, 0, pFile);
        m_roads.push_back(m_driveline);
        m_driveline->refresh();
    }
    for (u8 i = 1; i < size; i++)
    {
        r = new Road(sm->getRootSceneNode(), sm, 0, pFile);
        r->refresh();
        m_roads.push_back(r);
    }

    // OBJECTS

    u32 num;
    fread(&num, sizeof(u32), 1, pFile);
    Viewport::setLastEntityID(num + MAGIC_NUMBER);

    for (u32 i = 0; i < num; i++)
    {
        vector3df pos, rot, sca;
        fread(&pos, sizeof(vector3df), 1, pFile);
        fread(&rot, sizeof(vector3df), 1, pFile);
        fread(&sca, sizeof(vector3df), 1, pFile);
        u8 size;
        fread(&size, sizeof(u8), 1, pFile);
        c8 *name = new c8[size];
        fread(name, sizeof(c8), size, pFile);
        path p =name;
        ISceneNode* node =sm->addAnimatedMeshSceneNode(sm->getMesh(p));
        node->setPosition(pos);
        node->setRotation(rot);
        node->setScale(sca);
        node->setID(MAGIC_NUMBER + i);
        delete name;
    }

    fclose(pFile);

} // Track - from file

// ----------------------------------------------------------------------------
Track::~Track(){};


// ----------------------------------------------------------------------------
void Track::save()
{
  FILE* pFile = 0;

  path p = Editor::getEditor()->getMapsPath() + "/" + m_file_name.c_str();
  pFile = fopen(p.c_str(), "wb");

  assert(pFile);

  // TRACK NAME
  u8 size = m_track_name.size() + 1;
  fwrite(&size, sizeof(u8), 1, pFile);
  fwrite(m_track_name.c_str(), sizeof(wchar_t), size, pFile);

  // DESIGNER NAME
  size = m_designer.size() + 1;
  fwrite(&size, sizeof(u8), 1, pFile);
  fwrite(m_designer.c_str(), sizeof(wchar_t), size, pFile);

  // FILE NAME
  size = m_file_name.size() + 1;
  fwrite(&size, sizeof(u8), 1, pFile);
  fwrite(m_file_name.c_str(), sizeof(c8), size, pFile);

  // MUSIC
  size = m_music.size() + 1;
  fwrite(&size, sizeof(u8), 1, pFile);
  fwrite(m_music.c_str(), sizeof(c8), size, pFile);

  // TERRAIN
  m_terrain->save(pFile);

  // SKY
  Viewport::get()->getSky()->save(pFile);

  // ROADS
  size = m_roads.size();
  fwrite(&size, sizeof(u8), 1, pFile);
  IRoad* r;
  for (u8 i = 0; i < size; i++)
  {
      r = m_roads[i];
      r->save(pFile);
  }

  // OBJECTS
  ISceneManager* sm = Editor::getEditor()->getSceneManager();
  ISceneNode* node;
  u32 num = Viewport::getLastEntityID() - MAGIC_NUMBER;
  fwrite(&num, sizeof(u32), 1, pFile);

  for (u32 i = 0; i < num; i++)
  {
      assert(node = sm->getSceneNodeFromId(MAGIC_NUMBER + i + 1));
      if (node->isVisible())
      {
          fwrite(&node->getPosition(), sizeof(vector3df), 1, pFile);
          fwrite(&node->getRotation(), sizeof(vector3df), 1, pFile);
          fwrite(&node->getScale(),    sizeof(vector3df), 1, pFile);
          u8 size = strlen(node->getName()) + 1;
          fwrite(&size, sizeof(u8), 1, pFile);
          fwrite(node->getName(), sizeof(c8), size, pFile);
      }
  }
  fclose(pFile);

  Editor::getEditor()->addToRecentlyOpenedList(m_file_name);

} // save

// ----------------------------------------------------------------------------
void Track::quit()
{
    if (m_terrain) m_terrain->remove();
    m_terrain = 0;

    int i = 1;
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISceneNode* node;
    while ((node = sm->getSceneNodeFromId(MAGIC_NUMBER + i)))
    {
        node->remove();
        i++;
    }
    IRoad* r;
    for (u32 i = 0; i < m_roads.size(); i++)
    {
        r = m_roads[i];
        r->getSpline()->remove();
        r->remove();
    }

} // quit

// ----------------------------------------------------------------------------
void Track::build()
{
    IrrlichtDevice* device = Editor::getEditor()->getDevice();

    PHYSFS_setWriteDir(Editor::getEditor()->getTrackDir().c_str());
    PHYSFS_mkdir(m_file_name.c_str());

    path p = Editor::getEditor()->getTrackDir() + m_file_name;

    CMeshBuffer<S3DVertex2TCoords>* mb = m_terrain->build(p);
    SMesh smesh;
    smesh.addMeshBuffer(mb);

    for (int i = 1; i < m_roads.size(); i++)
    {
        IRoad* r = m_roads[i];
        if (r->getSpline()->getPointNum()>1)
            smesh.addMeshBuffer(((Road*)r)->getMeshBuffer());
    }

    B3DMeshWriter* writer = new B3DMeshWriter(device->getFileSystem());
    IWriteFile *file;
    file = device->getFileSystem()->createAndWriteFile((p + "/track.b3d").c_str());
    writer->writeMesh(file, &smesh);
    file->drop();
    delete writer;
    
    m_driveline->build(p);

    std::ofstream mat;
    mat.open((p + "/materials.xml").c_str());
    mat << "<materials>\n";
    mat << "  <material name=\"splatt.png\" graphical-effect=\"splatting\"";
    SMaterial m = m_terrain->getMaterial(0);
    for (int i = 1; i < 5; i++)
    {
        mat << " splatting-texture-" << i << "=\"";
        mat << Editor::toRelative(m.getTexture(i+1)->getName()).c_str();
        mat << "\"";
    }
    mat << "/>\n</materials>\n";

    mat.close();

    std::ofstream track;
    track.open((p + "/track.xml").c_str());
    track << "<track  name           = \"" << m_track_name.c_str() << "\"\n";
    track << "        version        = \"5\"\n";
    track << "        groups         = \"made-by-STK-TE\"\n";
    track << "        music          = \"" << m_music.c_str() << "\"\n";
    track << "        screenshot     = \"screenshot.jpg\"\n";
    track << "        smooth-normals = \"true\"\n";
    track << "        reverse        = \"Y\"\n>\n";
    track << "</track>\n";
    track.close();

    std::ofstream scene;
    scene.open((p + "/scene.xml").c_str());
    scene << "<scene>\n";
    scene << "  <track model=\"track.b3d\" x=\"0\" y=\"0\" z=\"0\">\n";

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISceneNode* node;
    stringc name;
    int i = 1;
    while ((node = sm->getSceneNodeFromId(MAGIC_NUMBER + i)))
    {
        name = node->getName();
        vector3df pos, rot, sca;
        if (node->isVisible() && name != "banana" && name != "item"
                              && name != "small-nitro" && name != "big-nitro")
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

    i = 1;
    while ((node = sm->getSceneNodeFromId(MAGIC_NUMBER + i)))
    {
        name = node->getName();
        vector3df pos;
        if (node->isVisible() && (name == "banana" || name == "item"
            || name == "small-nitro" || name == "big-nitro"))
        {
            pos = node->getPosition();
            scene << "  <" << name.c_str() << " x=\"" << pos.X << "\" y=\"" << pos.Y
                  << "\" z=\"" << pos.Z << "\" />\n";
        }
        i++;
    }

    scene << Viewport::get()->getSky()->getXmlString().c_str();

    Viewport::get()->printCheckLine(&scene);

    scene << "  <default-start   karts-per-row = \"3\"\n";
    scene << "                   forwards-distance =\"1.50\"\n";
    scene << "                   sidewards-distance=\"3.00\"\n";
    scene << "                   upwards-distance  =\"0.10\"/>\n";

    scene << "</scene>\n";
    scene.close();
} // build

// ----------------------------------------------------------------------------
void Track::insertRoad(IRoad* road)
{
    m_roads.push_back(road);
} // insertRoad

// ----------------------------------------------------------------------------
void Track::removeRoad(IRoad* road)
{
    IRoad* r;
    for (u8 i = 0; i < m_roads.size(); i++)
    {
        r = m_roads[i];
        if (r == road)
        {
            array<IRoad*> roads;
            for (int j = 0; j < m_roads.size(); j++)
                if (j!=i) roads.push_back(m_roads[j]);
            m_roads = roads;
            return;
        }
    }
} // removeRoad

// ----------------------------------------------------------------------------
void Track::createRoad(stringw type, stringw name)
{
    // Viewport::get()->setState(Viewport::SELECT);

    IRoad* rm;

    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISpline* spline;

    if (type == L"Bezier")
    {
        spline = new Bezier(sm->getRootSceneNode(), sm, 0);
    }
    else spline = new TCR(sm->getRootSceneNode(), sm, 0);

    rm = new Road(sm->getRootSceneNode(), sm, 0, spline, name);

    Viewport::get()->roadBorn(rm);

    m_roads.push_back(rm);
} // createRoad


