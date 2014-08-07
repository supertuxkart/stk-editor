#include "track.hpp"
#include "editor.hpp"

#include "viewport/viewport.hpp"

#include "gui/msg_wndw.hpp"

#include "mesh/terrain.hpp"
#include "mesh/driveline.hpp"
#include "mesh/road.hpp"

#include "spline/bezier.hpp"
#include "spline/tcr.hpp"

#include "b3d/B3DMeshWriter.h"

#include "editor.hpp"

#include <physfs.h>
#include <iostream>
#include <stdio.h>
#include <assert.h>

#define TOP_SECRET_SIGNATURE_NUMBER 3293525168
#define MAX_ROAD_NUM 1000

// ----------------------------------------------------------------------------
ISceneNode* Track::loadItem(stringc name)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISceneNode* node = 0;
    if (name == "banana")
    {
        node = sm->addAnimatedMeshSceneNode(sm->getMesh("models/banana.b3d"));
        node->setName("banana");
    }
    if (name == "item")
    {
        node = sm->addAnimatedMeshSceneNode(sm->getMesh("models/gift-box.b3d"));
        node->setName("item");
    }
    if (name == "small-nitro")
    {
        node = sm->addAnimatedMeshSceneNode(sm->getMesh("models/nitrotank-small.b3d"));
        node->setName("small-nitro");
    }
    if (name == "big-nitro")
    {
        node = sm->addAnimatedMeshSceneNode(sm->getMesh("models/nitrotank-big.b3d"));
        node->setName("big-nitro");
    }
    return node;
} // loadItem

// ----------------------------------------------------------------------------
void Track::exportElements(std::ofstream& stream, bool obj)
{
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISceneNode* node;
    stringc name;
    int i = 1;
    while ((node = sm->getSceneNodeFromId(MAGIC_NUMBER + i)))
    {
        name = node->getName();
        vector3df pos, rot, sca;
        if (node->isVisible() && name != "banana" && name != "item"
            && name != "small-nitro" && name != "big-nitro"
            && (name.equalsn("obj/", 4) == obj))
        {
            pos = node->getPosition();
            rot = node->getRotation();
            sca = node->getScale();
            if (name.equalsn("obj/", 4))
            {
                stream << "    <static-object model=\"" << Editor::toRelative(name).c_str();
                copyObj(name);
                
                ITexture* tex;
                for (int j = 0; tex = node->getMaterial(0).getTexture(j); j++)
                    copyObj(stringc("obj/") + Editor::toRelative(tex->getName()));                
            } // export as static-object
            else
            {
                stream << "  <library name=\"" << Editor::getLib(node->getName()).c_str();
            } // export as library
            stream << "\" xyz=\"";
            stream << pos.X << " " << pos.Y << " " << pos.Z << "\" hpr=\"";
            stream << rot.X << " " << rot.Y << " " << rot.Z << "\" scale=\"";
            stream << sca.X << " " << sca.Y << " " << sca.Z << "\"/>\n";
        }
        i++;
    }
} // exportElements

// ----------------------------------------------------------------------------
void Track::copyObj(stringc name)
{
    stringc p = Editor::getEditor()->getMapsPath();
    p += "/../";
    std::ifstream  src((p+name).c_str(), std::ios::binary);    
    name = Editor::toRelative(name);
    stringw dst_s = Editor::getEditor()->getTrackDir() + m_file_name + "/";
    std::ofstream  dst((dst_s + name).c_str(), std::ios::binary);
    try
    {
        dst << src.rdbuf();
    }
    catch (const std::exception& ex) 
    {
        std::cerr << ex.what();
    }
    catch (const std::string& ex) {
        std::cerr << ex.c_str();
    }
    catch (...) 
    {
        std::cerr << "Something terrible happend :(";
    }

    src.close();
    dst.close();
} // cpyFile

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
    m_valid = true;
} // Track

// ----------------------------------------------------------------------------
Track::Track(path file)
{
    m_valid = true;
    FILE* pFile;
    pFile = fopen(file.c_str(), "rb");

    if (!pFile)
    {
        m_valid = false;
        stringw emsg = "Editor failed to open file:\n \"";
        emsg += file;
        emsg += "\"";
        
        MsgWndw::get()->showMsg(emsg);
        return;
    }

    // SIGN
    u64 sign;
    fread(&sign, sizeof(u64), 1, pFile);
    if (sign != TOP_SECRET_SIGNATURE_NUMBER)
    {
        MsgWndw::get()->showMsg("File can not be opened: signature failed.");
        m_valid = false;
        fclose(pFile);
        return;
    }

    // TRACK NAME
    u8 size;
    wchar_t* c;
    fread(&size, sizeof(u8), 1, pFile);
    if (!Editor::isValidSize(size)) { fclose(pFile); m_valid = false; return; }
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, pFile);
    m_track_name = c;
    delete[] c;

    // DESIGNER NAME
    fread(&size, sizeof(u8), 1, pFile);
    if (!Editor::isValidSize(size)) { fclose(pFile); m_valid = false; return; }
    c = new wchar_t[size];
    fread(c, sizeof(wchar_t), size, pFile);
    m_designer = c;
    delete[] c;

    // FILE NAME
    c8* cc;
    fread(&size, sizeof(u8), 1, pFile);
    if (!Editor::isValidSize(size)) { fclose(pFile); m_valid = false; return; }
    cc = new c8[size];
    fread(cc, sizeof(c8), size, pFile);
    m_file_name = cc;
    delete[] cc;

    // MUSIC
    fread(&size, sizeof(u8), 1, pFile);
    if (!Editor::isValidSize(size)) { fclose(pFile); m_valid = false; return; }
    cc = new c8[size];
    fread(cc, sizeof(c8), size, pFile);
    m_music = cc;
    delete[] cc;

    // TERRAIN
    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    m_terrain = new Terrain(sm->getRootSceneNode(), sm, 1, pFile);

    if (!m_terrain->isValid())
    {
        fclose(pFile);
        MsgWndw::get()->showMsg("Loading failed :invalid terrain!");
        m_valid = false;
        return;
    }

    // SKY

    Sky* s = Viewport::get()->getSky();
    delete s;

    s = new Sky(pFile);
    Viewport::get()->setSky(s);
    
    // GRAVITY ROAD FLAG
    fread(&m_gravity_road, sizeof(bool), 1, pFile);

    // ROADS

    IRoad* r;
    fread(&size, sizeof(u8), 1, pFile);
    if (size < 0 || size > MAX_ROAD_NUM)
    {
        MsgWndw::get()->showMsg("Loading failed :invalid terrain!");
    }
    else
    {
        if (size > 0)
        {
            m_driveline = new DriveLine(sm->getRootSceneNode(), sm, 0, pFile);
            if (!m_driveline->isValid())
            {
                std::cerr << "Warning: invalid driveline!\n";
                ISpline* spline = new TCR(sm->getRootSceneNode(), sm, 0);
                m_driveline = new DriveLine(sm->getRootSceneNode(), sm, 0, spline, L"DriveLine");
            }
            m_roads.push_back(m_driveline);
            m_driveline->refresh();
        } // driveline
        for (u8 i = 1; i < size; i++)
        {
            r = new Road(sm->getRootSceneNode(), sm, 0, pFile);
            if (r->isValid())
            {
                m_roads.push_back(r);
                r->refresh();
                r->setWireFrame(false);
                Viewport::get()->setSplineMode(false);
            }
            else std::cerr << "Warning: invalid road - skipped :(\n";
        } // roads
    } // valid roadnum

    // OBJECTS
    u32 num;
    fread(&num, sizeof(u32), 1, pFile);
    Viewport::setLastEntityID(num + MAGIC_NUMBER);

    for (u32 i = 0; i < num; i++)
    {
        ISceneNode* node = 0;
        vector3df pos, rot, sca;
        fread(&pos, sizeof(vector3df), 1, pFile);
        fread(&rot, sizeof(vector3df), 1, pFile);
        fread(&sca, sizeof(vector3df), 1, pFile);
        u8 size;
        fread(&size, sizeof(u8), 1, pFile);
        if (!Editor::isValidSize(size)) return;
        c8 *name = new c8[size];
        fread(name, sizeof(c8), size, pFile);
        path p = name;
        if (p == path("banana") || p == path("item")
            || p == path("small-nitro") || p == path("big-nitro"))
        {
            node = loadItem(name);
        } // item
        else
        {
            node = sm->addAnimatedMeshSceneNode(sm->getMesh(p));
            node->setName(name);
        } // object
        if (node)
        {
            node->setPosition(pos);
            node->setRotation(rot);
            node->setScale(sca);
            node->setID(MAGIC_NUMBER + i + 1);
        }  // valid node
        else
        {
            std::cerr << "Warning: couldn't load object < " << name << " >!\n";
            num -= 1;
            i -= 1;
        } // invalid node
        delete[] name;
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

  if (!pFile)
  {
      MsgWndw::get()->showMsg("Save failed: file could not be created!\n");
      return;
  }

  // SIGN
  u64 sign = TOP_SECRET_SIGNATURE_NUMBER;
  fwrite(&sign, sizeof(u64), 1, pFile);

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

  // GRAVITY ROAD FLAG
  fwrite(&m_gravity_road, sizeof(bool), 1, pFile);

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
      node = sm->getSceneNodeFromId(MAGIC_NUMBER + i + 1);
      assert(node);
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

  MsgWndw::get()->showMsg("Track saved!\n");

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

    for (u32 i = 1; i < m_roads.size(); i++)
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
    mat << "/>\n";

    if (m_gravity_road)
    {
        for (u32 i = 1; i < m_roads.size(); i++)
        {
            stringc tex = m_roads[i]->getTexName();
            if (tex.size()>0)
            {
                mat << "  <material name=\"";
                mat << tex.c_str();
                mat << "\" has-gravity=\"yes\" />\n";
            }
        } // roads
    } // gravity road mode

    mat <<"</materials>\n";
    mat.close();

    stringw track;
    track += "<track  name           = \"";
    track += m_track_name + L"\"\n";
    track += "        version        = \"5\"\n";
    track += "        groups         = \"made-by-STK-TE\"\n";
    track += "        designer       = \"";
    track += m_designer + "\"\n";
    track += "        music          = \"";
    track += m_music.c_str();
    track += "\"\n";
    track += "        screenshot     = \"screenshot.jpg\"\n";
    track += "        smooth-normals = \"true\"\n";
    track += "        reverse        = \"Y\"\n>\n";
    track += "</track>\n";

    PHYSFS_uint64 len = 4 * track.size();
    char*	      dst = new char[len];
#ifdef _WIN32
    PHYSFS_utf8FromUcs2((PHYSFS_uint16*)track.c_str(),dst,len);
#else
    PHYSFS_utf8FromUcs4((PHYSFS_uint32*)track.c_str(), dst, len);
#endif

    FILE* f;
    f = fopen((p + "/track.xml").c_str(), "wb");
    fwrite(dst, sizeof(char), strlen(dst), f);
    fclose(f);
    delete[] dst;


    std::ofstream scene;
    scene.open((p + "/scene.xml").c_str());
    scene << "<scene>\n";
    scene << "  <track model=\"track.b3d\" x=\"0\" y=\"0\" z=\"0\">\n";
    
    exportElements(scene, true);
    scene << "  </track>\n";
    exportElements(scene, false);


    ISceneManager* sm = Editor::getEditor()->getSceneManager();
    ISceneNode* node;
    int i = 1;
    stringw name;
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

    MsgWndw::get()->showMsg("Track exported!");

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
            for (u8 j = 0; j < m_roads.size(); j++)
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

// ----------------------------------------------------------------------------
u32 Track::getRoadID(IRoad* r)
{ 
    u32 i;
    for (i = 0; i < m_roads.size() && m_roads[i] != r; i++);
    return i;
} // getRoadID
