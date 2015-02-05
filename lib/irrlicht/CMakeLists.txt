# CMakeLists.txt for Irrlicht in STK-Editor

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/Irrlicht/jpeglib"
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/Irrlicht/libpng"
                    "${ZLIB_INCLUDE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}/../zlib/")

if(APPLE)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source/Irrlicht/MacOSX" "${CMAKE_CURRENT_SOURCE_DIR}/source/Irrlicht")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch i386")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch i386 -F/Library/Frameworks")
endif()

add_definitions(-DNDEBUG=1 -DIRRLICHT_EXPORTS=1 -DPNG_THREAD_UNSAFE_OK -DPNG_NO_MMX_CODE -DPNG_NO_MNG_FEATURES)
if(WIN32)
  add_definitions(-D_IRR_STATIC_LIB_)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS) # Shut up about unsafe stuff
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -pipe -O3  -fno-exceptions  -fstrict-aliasing -fexpensive-optimizations -I/usr/X11R6/include")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -pipe -O3  -fno-exceptions  -fstrict-aliasing -fexpensive-optimizations -I/usr/X11R6/include")
endif()

add_definitions(-DNO_IRR_COMPILE_WITH_ZIP_ENCRYPTION_ -DNO_IRR_COMPILE_WITH_BZIP2_ -DNO_IRR_USE_NON_SYSTEM_BZLIB_ -DNO_IRR_COMPILE_WITH_LZMA_ -DDNO_IRR_COMPILE_WITH_DIRECTINPUT_JOYSTICK_ -DNO_IRR_COMPILE_WITH_DIRECT3D_9_)


set(IRRLICHT_SOURCES

source/Irrlicht/C3DSMeshFileLoader.cpp
source/Irrlicht/CAnimatedMeshHalfLife.cpp
source/Irrlicht/CAnimatedMeshMD2.cpp
source/Irrlicht/CAnimatedMeshMD3.cpp
source/Irrlicht/CAnimatedMeshSceneNode.cpp
source/Irrlicht/CAttributes.cpp
source/Irrlicht/CB3DMeshFileLoader.cpp
source/Irrlicht/CBillboardSceneNode.cpp
source/Irrlicht/CBoneSceneNode.cpp
source/Irrlicht/CBSPMeshFileLoader.cpp
source/Irrlicht/CBurningShader_Raster_Reference.cpp
source/Irrlicht/CCameraSceneNode.cpp
source/Irrlicht/CCgMaterialRenderer.cpp
source/Irrlicht/CColladaFileLoader.cpp
source/Irrlicht/CColladaMeshWriter.cpp
source/Irrlicht/CColorConverter.cpp
source/Irrlicht/CCSMLoader.cpp
source/Irrlicht/CCubeSceneNode.cpp
source/Irrlicht/CD3D8Driver.cpp
source/Irrlicht/CD3D8NormalMapRenderer.cpp
source/Irrlicht/CD3D8ParallaxMapRenderer.cpp
source/Irrlicht/CD3D8ShaderMaterialRenderer.cpp
source/Irrlicht/CD3D8Texture.cpp
source/Irrlicht/CD3D9CgMaterialRenderer.cpp
source/Irrlicht/CD3D9Driver.cpp
source/Irrlicht/CD3D9HLSLMaterialRenderer.cpp
source/Irrlicht/CD3D9NormalMapRenderer.cpp
source/Irrlicht/CD3D9ParallaxMapRenderer.cpp
source/Irrlicht/CD3D9ShaderMaterialRenderer.cpp
source/Irrlicht/CD3D9Texture.cpp
source/Irrlicht/CDefaultGUIElementFactory.cpp
source/Irrlicht/CDefaultSceneNodeAnimatorFactory.cpp
source/Irrlicht/CDefaultSceneNodeFactory.cpp
source/Irrlicht/CDepthBuffer.cpp
source/Irrlicht/CDMFLoader.cpp
source/Irrlicht/CDummyTransformationSceneNode.cpp
source/Irrlicht/CEmptySceneNode.cpp
source/Irrlicht/CFileList.cpp
source/Irrlicht/CFileSystem.cpp
source/Irrlicht/CFPSCounter.cpp
source/Irrlicht/CGeometryCreator.cpp
source/Irrlicht/CGUIButton.cpp
source/Irrlicht/CGUICheckBox.cpp
source/Irrlicht/CGUIColorSelectDialog.cpp
source/Irrlicht/CGUIComboBox.cpp
source/Irrlicht/CGUIContextMenu.cpp
source/Irrlicht/CGUIEditBox.cpp
source/Irrlicht/CGUIEnvironment.cpp
source/Irrlicht/CGUIFileOpenDialog.cpp
source/Irrlicht/CGUIFont.cpp
source/Irrlicht/CGUIImage.cpp
source/Irrlicht/CGUIImageList.cpp
source/Irrlicht/CGUIInOutFader.cpp
source/Irrlicht/CGUIListBox.cpp
source/Irrlicht/CGUIMenu.cpp
source/Irrlicht/CGUIMeshViewer.cpp
source/Irrlicht/CGUIMessageBox.cpp
source/Irrlicht/CGUIModalScreen.cpp
source/Irrlicht/CGUIScrollBar.cpp
source/Irrlicht/CGUISkin.cpp
source/Irrlicht/CGUISpinBox.cpp
source/Irrlicht/CGUISpriteBank.cpp
source/Irrlicht/CGUIStaticText.cpp
source/Irrlicht/CGUITabControl.cpp
source/Irrlicht/CGUITable.cpp
source/Irrlicht/CGUIToolBar.cpp
source/Irrlicht/CGUITreeView.cpp
source/Irrlicht/CGUIWindow.cpp
source/Irrlicht/CImage.cpp
source/Irrlicht/CImageLoaderBMP.cpp
source/Irrlicht/CImageLoaderDDS.cpp
source/Irrlicht/CImageLoaderJPG.cpp
source/Irrlicht/CImageLoaderPCX.cpp
source/Irrlicht/CImageLoaderPNG.cpp
source/Irrlicht/CImageLoaderPPM.cpp
source/Irrlicht/CImageLoaderPSD.cpp
source/Irrlicht/CImageLoaderRGB.cpp
source/Irrlicht/CImageLoaderTGA.cpp
source/Irrlicht/CImageLoaderWAL.cpp
source/Irrlicht/CImageWriterBMP.cpp
source/Irrlicht/CImageWriterJPG.cpp
source/Irrlicht/CImageWriterPCX.cpp
source/Irrlicht/CImageWriterPNG.cpp
source/Irrlicht/CImageWriterPPM.cpp
source/Irrlicht/CImageWriterPSD.cpp
source/Irrlicht/CImageWriterTGA.cpp
source/Irrlicht/CIrrDeviceConsole.cpp
source/Irrlicht/CIrrDeviceFB.cpp
source/Irrlicht/CIrrDeviceLinux.cpp
source/Irrlicht/CIrrDeviceSDL.cpp
source/Irrlicht/CIrrDeviceStub.cpp
source/Irrlicht/CIrrDeviceWin32.cpp
source/Irrlicht/CIrrDeviceWinCE.cpp
source/Irrlicht/CIrrMeshFileLoader.cpp
source/Irrlicht/CIrrMeshWriter.cpp
source/Irrlicht/CLightSceneNode.cpp
source/Irrlicht/CLimitReadFile.cpp
source/Irrlicht/CLMTSMeshFileLoader.cpp
source/Irrlicht/CLogger.cpp
source/Irrlicht/CLWOMeshFileLoader.cpp
source/Irrlicht/CMD2MeshFileLoader.cpp
source/Irrlicht/CMD3MeshFileLoader.cpp
source/Irrlicht/CMemoryFile.cpp
source/Irrlicht/CMeshCache.cpp
source/Irrlicht/CMeshManipulator.cpp
source/Irrlicht/CMeshSceneNode.cpp
source/Irrlicht/CMetaTriangleSelector.cpp
source/Irrlicht/CMountPointReader.cpp
source/Irrlicht/CMS3DMeshFileLoader.cpp
source/Irrlicht/CMY3DMeshFileLoader.cpp
source/Irrlicht/CNPKReader.cpp
source/Irrlicht/CNullDriver.cpp
source/Irrlicht/COBJMeshFileLoader.cpp
source/Irrlicht/COBJMeshWriter.cpp
source/Irrlicht/COCTLoader.cpp
source/Irrlicht/COctreeSceneNode.cpp
source/Irrlicht/COctreeTriangleSelector.cpp
source/Irrlicht/COgreMeshFileLoader.cpp
source/Irrlicht/COpenGLCgMaterialRenderer.cpp
source/Irrlicht/COpenGLDriver.cpp
source/Irrlicht/COpenGLExtensionHandler.cpp
source/Irrlicht/COpenGLNormalMapRenderer.cpp
source/Irrlicht/COpenGLParallaxMapRenderer.cpp
source/Irrlicht/COpenGLShaderMaterialRenderer.cpp
source/Irrlicht/COpenGLSLMaterialRenderer.cpp
source/Irrlicht/COpenGLTexture.cpp
source/Irrlicht/COSOperator.cpp
source/Irrlicht/CPakReader.cpp
source/Irrlicht/CParticleAnimatedMeshSceneNodeEmitter.cpp
source/Irrlicht/CParticleAttractionAffector.cpp
source/Irrlicht/CParticleBoxEmitter.cpp
source/Irrlicht/CParticleCylinderEmitter.cpp
source/Irrlicht/CParticleFadeOutAffector.cpp
source/Irrlicht/CParticleGravityAffector.cpp
source/Irrlicht/CParticleMeshEmitter.cpp
source/Irrlicht/CParticlePointEmitter.cpp
source/Irrlicht/CParticleRingEmitter.cpp
source/Irrlicht/CParticleRotationAffector.cpp
source/Irrlicht/CParticleScaleAffector.cpp
source/Irrlicht/CParticleSphereEmitter.cpp
source/Irrlicht/CParticleSystemSceneNode.cpp
source/Irrlicht/CPLYMeshFileLoader.cpp
source/Irrlicht/CPLYMeshWriter.cpp
source/Irrlicht/CQ3LevelMesh.cpp
source/Irrlicht/CQuake3ShaderSceneNode.cpp
source/Irrlicht/CReadFile.cpp
source/Irrlicht/CSceneCollisionManager.cpp
source/Irrlicht/CSceneLoaderIrr.cpp
source/Irrlicht/CSceneManager.cpp
source/Irrlicht/CSceneNodeAnimatorCameraFPS.cpp
source/Irrlicht/CSceneNodeAnimatorCameraMaya.cpp
source/Irrlicht/CSceneNodeAnimatorCollisionResponse.cpp
source/Irrlicht/CSceneNodeAnimatorDelete.cpp
source/Irrlicht/CSceneNodeAnimatorFlyCircle.cpp
source/Irrlicht/CSceneNodeAnimatorFlyStraight.cpp
source/Irrlicht/CSceneNodeAnimatorFollowSpline.cpp
source/Irrlicht/CSceneNodeAnimatorRotation.cpp
source/Irrlicht/CSceneNodeAnimatorTexture.cpp
source/Irrlicht/CShadowVolumeSceneNode.cpp
source/Irrlicht/CSkinnedMesh.cpp
source/Irrlicht/CSkyBoxSceneNode.cpp
source/Irrlicht/CSkyDomeSceneNode.cpp
source/Irrlicht/CSMFMeshFileLoader.cpp
source/Irrlicht/CSoftwareDriver2.cpp
source/Irrlicht/CSoftwareDriver.cpp
source/Irrlicht/CSoftwareTexture2.cpp
source/Irrlicht/CSoftwareTexture.cpp
source/Irrlicht/CSphereSceneNode.cpp
source/Irrlicht/CSTLMeshFileLoader.cpp
source/Irrlicht/CSTLMeshWriter.cpp
source/Irrlicht/CTarReader.cpp
source/Irrlicht/CTerrainSceneNode.cpp
source/Irrlicht/CTerrainTriangleSelector.cpp
source/Irrlicht/CTextSceneNode.cpp
source/Irrlicht/CTRFlat.cpp
source/Irrlicht/CTRFlatWire.cpp
source/Irrlicht/CTRGouraud2.cpp
source/Irrlicht/CTRGouraudAlpha2.cpp
source/Irrlicht/CTRGouraudAlphaNoZ2.cpp
source/Irrlicht/CTRGouraud.cpp
source/Irrlicht/CTRGouraudWire.cpp
source/Irrlicht/CTriangleBBSelector.cpp
source/Irrlicht/CTriangleSelector.cpp
source/Irrlicht/CTRNormalMap.cpp
source/Irrlicht/CTRStencilShadow.cpp
source/Irrlicht/CTRTextureBlend.cpp
source/Irrlicht/CTRTextureDetailMap2.cpp
source/Irrlicht/CTRTextureFlat.cpp
source/Irrlicht/CTRTextureFlatWire.cpp
source/Irrlicht/CTRTextureGouraud2.cpp
source/Irrlicht/CTRTextureGouraudAdd2.cpp
source/Irrlicht/CTRTextureGouraudAdd.cpp
source/Irrlicht/CTRTextureGouraudAddNoZ2.cpp
source/Irrlicht/CTRTextureGouraudAlpha.cpp
source/Irrlicht/CTRTextureGouraudAlphaNoZ.cpp
source/Irrlicht/CTRTextureGouraud.cpp
source/Irrlicht/CTRTextureGouraudNoZ2.cpp
source/Irrlicht/CTRTextureGouraudNoZ.cpp
source/Irrlicht/CTRTextureGouraudVertexAlpha2.cpp
source/Irrlicht/CTRTextureGouraudWire.cpp
source/Irrlicht/CTRTextureLightMap2_Add.cpp
source/Irrlicht/CTRTextureLightMap2_M1.cpp
source/Irrlicht/CTRTextureLightMap2_M2.cpp
source/Irrlicht/CTRTextureLightMap2_M4.cpp
source/Irrlicht/CTRTextureLightMapGouraud2_M4.cpp
source/Irrlicht/CTRTextureWire2.cpp
source/Irrlicht/CVideoModeList.cpp
source/Irrlicht/CVolumeLightSceneNode.cpp
source/Irrlicht/CWADReader.cpp
source/Irrlicht/CWaterSurfaceSceneNode.cpp
source/Irrlicht/CWriteFile.cpp
source/Irrlicht/CXMeshFileLoader.cpp
source/Irrlicht/CXMLReader.cpp
source/Irrlicht/CXMLWriter.cpp
source/Irrlicht/CZBuffer.cpp
source/Irrlicht/CZipReader.cpp
source/Irrlicht/IBurningShader.cpp
source/Irrlicht/Irrlicht.cpp
source/Irrlicht/irrXML.cpp
source/Irrlicht/os.cpp

source/Irrlicht/jpeglib/jaricom.c
source/Irrlicht/jpeglib/jcapimin.c
source/Irrlicht/jpeglib/jcapistd.c
source/Irrlicht/jpeglib/jcarith.c
source/Irrlicht/jpeglib/jccoefct.c
source/Irrlicht/jpeglib/jccolor.c
source/Irrlicht/jpeglib/jcdctmgr.c
source/Irrlicht/jpeglib/jchuff.c
source/Irrlicht/jpeglib/jcinit.c
source/Irrlicht/jpeglib/jcmainct.c
source/Irrlicht/jpeglib/jcmarker.c
source/Irrlicht/jpeglib/jcmaster.c
source/Irrlicht/jpeglib/jcomapi.c
source/Irrlicht/jpeglib/jcparam.c
source/Irrlicht/jpeglib/jcprepct.c
source/Irrlicht/jpeglib/jcsample.c
source/Irrlicht/jpeglib/jctrans.c
source/Irrlicht/jpeglib/jdapimin.c
source/Irrlicht/jpeglib/jdapistd.c
source/Irrlicht/jpeglib/jdarith.c
source/Irrlicht/jpeglib/jdatadst.c
source/Irrlicht/jpeglib/jdatasrc.c
source/Irrlicht/jpeglib/jdcoefct.c
source/Irrlicht/jpeglib/jdcolor.c
source/Irrlicht/jpeglib/jddctmgr.c
source/Irrlicht/jpeglib/jdhuff.c
source/Irrlicht/jpeglib/jdinput.c
source/Irrlicht/jpeglib/jdmainct.c
source/Irrlicht/jpeglib/jdmarker.c
source/Irrlicht/jpeglib/jdmaster.c
source/Irrlicht/jpeglib/jdmerge.c
source/Irrlicht/jpeglib/jdpostct.c
source/Irrlicht/jpeglib/jdsample.c
source/Irrlicht/jpeglib/jdtrans.c
source/Irrlicht/jpeglib/jerror.c
source/Irrlicht/jpeglib/jfdctflt.c
source/Irrlicht/jpeglib/jfdctfst.c
source/Irrlicht/jpeglib/jfdctint.c
source/Irrlicht/jpeglib/jidctflt.c
source/Irrlicht/jpeglib/jidctfst.c
source/Irrlicht/jpeglib/jidctint.c
source/Irrlicht/jpeglib/jmemmgr.c
source/Irrlicht/jpeglib/jmemnobs.c
source/Irrlicht/jpeglib/jquant1.c
source/Irrlicht/jpeglib/jquant2.c
source/Irrlicht/jpeglib/jutils.c

source/Irrlicht/libpng/pngwutil.c
source/Irrlicht/libpng/pngget.c
source/Irrlicht/libpng/pngrtran.c
source/Irrlicht/libpng/pngrutil.c
source/Irrlicht/libpng/pngerror.c
source/Irrlicht/libpng/pngset.c
source/Irrlicht/libpng/pngtest.c
source/Irrlicht/libpng/pngmem.c
source/Irrlicht/libpng/pngpread.c
source/Irrlicht/libpng/png.c
source/Irrlicht/libpng/pngread.c
source/Irrlicht/libpng/pngwio.c
source/Irrlicht/libpng/pngtrans.c
source/Irrlicht/libpng/pngwrite.c
source/Irrlicht/libpng/pngwtran.c
source/Irrlicht/libpng/example.c
source/Irrlicht/libpng/pngrio.c

)

if(APPLE)
    set(IRRLICHT_SOURCES
        ${IRRLICHT_SOURCES}
        source/Irrlicht/MacOSX/AppDelegate.mm
        source/Irrlicht/MacOSX/CIrrDeviceMacOSX.mm
        source/Irrlicht/MacOSX/OSXClipboard.mm)

    #list(APPEND CMAKE_C_SOURCE_FILE_EXTENSIONS mm)
    #set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS cpp)

    set_source_files_properties(source/Irrlicht/MacOSX/AppDelegate.mm PROPERTIES COMPILE_FLAGS "-x objective-c++ -O3  -fno-rtti")
    set_source_files_properties(source/Irrlicht/MacOSX/AppDelegate.mm PROPERTIES LANGUAGE C)

    set_source_files_properties(source/Irrlicht/MacOSX/CIrrDeviceMacOSX.mm PROPERTIES COMPILE_FLAGS "-x objective-c++ -O3  -fno-rtti")
    set_source_files_properties(source/Irrlicht/MacOSX/CIrrDeviceMacOSX.mm PROPERTIES LANGUAGE C)

    set_source_files_properties(source/Irrlicht/MacOSX/OSXClipboard.mm PROPERTIES COMPILE_FLAGS "-x objective-c++ -O3 -fno-rtti")
    set_source_files_properties(source/Irrlicht/MacOSX/OSXClipboard.mm PROPERTIES LANGUAGE C)
endif()

add_library(irrlicht ${IRRLICHT_SOURCES})
target_link_libraries(irrlicht ${ZLIB_LIBRARY})
