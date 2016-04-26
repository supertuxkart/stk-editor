project(supertuxkart-editor)
cmake_minimum_required(VERSION 2.8)

if(NOT (CMAKE_MAJOR_VERSION VERSION_LESS 3))
  cmake_policy(SET CMP0043 OLD)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

option(STATIC_PHYSFS "Force build with static physfs library" OFF)
option(STATIC_ZLIB "Force build with static zlib library" OFF)

# These variables enable MSVC to find libraries located in "dependencies"
if(WIN32)
    set(ENV{PATH} "$ENV{PATH};${PROJECT_SOURCE_DIR}/dependencies/include")
    set(ENV{LIB} ${PROJECT_SOURCE_DIR}/dependencies/lib)
    set(ENV{OPENALDIR} ${PROJECT_SOURCE_DIR}/dependencies)
endif()

set(STKTE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
include_directories(${STKTE_SOURCE_DIR})

if(WIN32)
    add_definitions(-D_IRR_STATIC_LIB_)
    configure_file("${PROJECT_SOURCE_DIR}/src/icon_rc.template" "${PROJECT_BINARY_DIR}/tmp/icon.rc")
endif()

file(GLOB_RECURSE STKTE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${STKTE_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE STKTE_RESOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PROJECT_BINARY_DIR}/tmp/*.rc")

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found")
endif()

if(WIN32 OR APPLE OR STATIC_ZLIB)
    add_subdirectory("${PROJECT_SOURCE_DIR}/lib/zlib")
    include_directories("${PROJECT_SOURCE_DIR}/lib/zlib")
    set(ZLIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/zlib" "${PROJECT_BINARY_DIR}/lib/zlib/")
    set(ZLIB_LIBRARY zlibstatic)
else()
    find_package(ZLIB REQUIRED)
endif()

if(WIN32 OR APPLE OR STATIC_PHYSFS)
    add_definitions(-DPHYSFS_BUILD_STATIC)
    add_subdirectory("${PROJECT_SOURCE_DIR}/lib/physfs")
    include_directories("${PROJECT_SOURCE_DIR}/lib/physfs")
    set(PHYSFS_LIBRARY physfs-static)
else()
    find_package(PhysFS REQUIRED)
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/lib/irrlicht")
include_directories("${PROJECT_SOURCE_DIR}/lib/irrlicht/include")

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    find_library(IRRLICHT_XF86VM_LIBRARY Xxf86vm)
endif()

if(UNIX OR MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    add_definitions(-DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG DEBUG)
else()
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-DDEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    else()
        add_definitions(-DNDEBUG)
    endif()
endif()

add_executable(${PROJECT_NAME} ${STKTE_SOURCES} ${STKTE_RESOURCES})

# Link options
target_link_libraries(${PROJECT_NAME}
                      irrlicht
                      ${PHYSFS_LIBRARY}
                      ${OPENGL_LIBRARIES}
)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} 
                          ${X11_LIBRARIES}
                          ${IRRLICHT_XF86VM_LIBRARY}
    )
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} winmm)
endif()
