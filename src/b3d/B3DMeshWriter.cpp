// Copyright (C) 2014 Lauri Kasanen
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#include "editor.hpp"

#include "B3DMeshWriter.h"
//#include "os.h"
#include "IMesh.h"
#include "IMeshBuffer.h"
#include "IWriteFile.h"
#include "ITexture.h"
#include "irrMap.h"

namespace irr
{
namespace scene
{

using namespace core;
using namespace video;

B3DMeshWriter::B3DMeshWriter(io::IFileSystem *fs): FileSystem(fs)
{
    #ifdef _DEBUG
    setDebugName("CB3DMeshWriter");
    #endif
}


//! Returns the type of the mesh writer
EMESH_WRITER_TYPE B3DMeshWriter::getType() const
{
    return EMWT_OBJ;
}

//! writes a mesh
bool B3DMeshWriter::writeMesh(io::IWriteFile* file, IMesh* const mesh, s32 flags)
{
    if (!file || !mesh)
        return false;
#ifdef __BIG_ENDIAN__
    os::Printer::log("B3D export does not support big-endian systems.", ELL_ERROR);
    return false;
#endif

    size = 0;
    file->write("BB3D", 4);
    file->write(&size, sizeof(u32)); // Updated later once known.

    int version = 1;
    write(file, &version, sizeof(int));

    //

    const u32 mbs = mesh->getMeshBufferCount();
    u32 i, j;
    array<SB3dTexture> texs;
    map<ITexture *, u32> tex2id;
    u32 texsizes = 0;
    for (i = 0; i < mbs; i++) {
        const IMeshBuffer * const mb = mesh->getMeshBuffer(i);
        const SMaterial &mat = mb->getMaterial();

        for (j = 0; j < MATERIAL_MAX_TEXTURES; j++) {
            if (mat.getTexture(j)) {
                SB3dTexture t = {
                    Editor::toRelative(mat.getTexture(j)->getName().getPath()).c_str(),
                    j == 2 ? 65536 : 1, 2,
                    0, 0, 1, 1, 0 };
                texs.push_back(t);
                texsizes += 7*4 + t.TextureName.size() + 1;
                tex2id[mat.getTexture(j)] = texs.size() - 1;
            }
        }
    }

    write(file, "TEXS", 4);
    write(file, &texsizes, 4);
    
    u32 max = texs.size();
    for (i = 0; i < max; i++) {
        write(file, texs[i].TextureName.c_str(), texs[i].TextureName.size() + 1);
        write(file, &texs[i].Flags, 7*4);
    }

    //

    const u32 brushsize = (7 * 4 + 1) * mbs + mbs * 4 * MATERIAL_MAX_TEXTURES + 4;
    write(file, "BRUS", 4);
    write(file, &brushsize, 4);
    u32 brushcheck = size;
    const u32 usedtex = MATERIAL_MAX_TEXTURES;
    write(file, &usedtex, 4);

    for (i = 0; i < mbs; i++) {
        const IMeshBuffer * const mb = mesh->getMeshBuffer(i);
        const SMaterial &mat = mb->getMaterial();

        write(file, "", 1);

        float f = 1;
        write(file, &f, 4);
        write(file, &f, 4);
        write(file, &f, 4);
        write(file, &f, 4);

        f = 0;
        write(file, &f, 4);

        u32 tmp = 1;
        write(file, &tmp, 4);
        tmp = 0;
        write(file, &tmp, 4);

        for (j = 0; j < MATERIAL_MAX_TEXTURES; j++) {
            if (mat.getTexture(j)) {
                const u32 id = tex2id[mat.getTexture(j)];
                write(file, &id, 4);
            } else {
                const int id = -1;
                write(file, &id, 4);
            }
        }
    }

    // Check brushsize
    brushcheck = size - brushcheck;
    if (brushcheck != brushsize)
        printf("Failed in brush size calculation, size %u advanced %u\n",
            brushsize, brushcheck);
    //

    for (i = 0; i < mbs; i++) {
        const IMeshBuffer * const mb = mesh->getMeshBuffer(i);

        write(file, "NODE", 4);

        // Calculate node size
        u32 nodesize = 41 + 8 + 8 + 8 + 4 + 4;

        nodesize += 12;
        nodesize += mb->getVertexCount() * 10 * 4;
        u32 texcoords = 1;
        if (mb->getVertexType() == EVT_2TCOORDS)
        texcoords = 2;
        nodesize += mb->getVertexCount() * texcoords * 2 * 4;

        nodesize += mb->getIndexCount() * 4;

        write(file, &nodesize, 4);
        u32 nodecheck = size;

        // Node
        write(file, "", 1);
        float f = 0;
        write(file, &f, 4);
        write(file, &f, 4);
        write(file, &f, 4);

        f = 1;
        write(file, &f, 4);
        write(file, &f, 4);
        write(file, &f, 4);

        write(file, &f, 4);
        f = 0;
        write(file, &f, 4);
        write(file, &f, 4);
        write(file, &f, 4);

        // Mesh
        write(file, "MESH", 4);
        const u32 meshsize = nodesize - 41 - 8;
        write(file, &meshsize, 4);

        write(file, &i, 4);

        // Verts
        write(file, "VRTS", 4);
        const u32 vertsize = 12 + mb->getVertexCount() * 10 * 4 +
        mb->getVertexCount() * texcoords * 2 * 4;
        write(file, &vertsize, 4);
        u32 vertcheck = size;

        int flags = 3;
        write(file, &flags, 4);
        write(file, &texcoords, 4);
        flags = 2;
        write(file, &flags, 4);

        max = mb->getVertexCount();
        for (j = 0; j < max; j++) {
            const vector3df &pos = mb->getPosition(j);
            write(file, &pos.X, 4);
            write(file, &pos.Y, 4);
            write(file, &pos.Z, 4);

            const vector3df &n = mb->getNormal(j);
            write(file, &n.X, 4);
            write(file, &n.Y, 4);
            write(file, &n.Z, 4);

            switch (mb->getVertexType()) {
                case EVT_STANDARD:
                {
                    S3DVertex *v = (S3DVertex *) mb->getVertices();
                    const SColorf col(v[j].Color);
                    write(file, &col.r, 4);
                    write(file, &col.g, 4);
                    write(file, &col.b, 4);
                    write(file, &col.a, 4);

                    write(file, &v[j].TCoords.X, 4);
                    write(file, &v[j].TCoords.Y, 4);
                }
                break;
                case EVT_2TCOORDS:
                {
                    S3DVertex2TCoords *v = (S3DVertex2TCoords *) mb->getVertices();
                    const SColorf col(v[j].Color);
                    write(file, &col.r, 4);
                    write(file, &col.g, 4);
                    write(file, &col.b, 4);
                    write(file, &col.a, 4);

                    write(file, &v[j].TCoords.X, 4);
                    write(file, &v[j].TCoords.Y, 4);
                    write(file, &v[j].TCoords2.X, 4);
                    write(file, &v[j].TCoords2.Y, 4);
                }
                break;
                case EVT_TANGENTS:
                {
                    S3DVertexTangents *v = (S3DVertexTangents *) mb->getVertices();
                    const SColorf col(v[j].Color);
                    write(file, &col.r, 4);
                    write(file, &col.g, 4);
                    write(file, &col.b, 4);
                    write(file, &col.a, 4);

                    write(file, &v[j].TCoords.X, 4);
                    write(file, &v[j].TCoords.Y, 4);
                }
                break;
            }
        }

        // Check vertsize
        vertcheck = size - vertcheck;
        if (vertcheck != vertsize)
            printf("Failed in vertex size calculation, size %u advanced %u\n",
                vertsize, vertcheck);

        // Tris
        write(file, "TRIS", 4);
        const u32 trisize = 4 + mb->getIndexCount() * 4;
        write(file, &trisize, 4);

        write(file, &i, 4);

        max = mb->getIndexCount();
        const u16 * const idx = (u16 *) mb->getIndices();
        for (j = 0; j < max; j += 3) {
            u32 tmp = idx[j];
            write(file, &tmp, sizeof(u32));

            tmp = idx[j + 1];
            write(file, &tmp, sizeof(u32));

            tmp = idx[j + 2];
            write(file, &tmp, sizeof(u32));
        }

        // Check that node calculation was ok
        nodecheck = size - nodecheck;
        if (nodecheck != nodesize)
            printf("Failed in node size calculation, size %u advanced %u\n",
                nodesize, nodecheck);
    }

    file->seek(4);
    file->write(&size, 4);

    return true;
}

void B3DMeshWriter::write(io::IWriteFile* file, const void *ptr, const u32 bytes)
{
    file->write(ptr, bytes);
    size += bytes;
}

} // end namespace
} // end namespace

