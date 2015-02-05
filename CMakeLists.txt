project(stk-editor)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# These variables enable MSVC to find libraries located in "dependencies"
if(WIN32)
    set(ENV{PATH} "$ENV{PATH};${PROJECT_SOURCE_DIR}/dependencies/include")
    set(ENV{LIB} ${PROJECT_SOURCE_DIR}/dependencies/lib)
    set(ENV{OPENALDIR} ${PROJECT_SOURCE_DIR}/dependencies)
endif()

set(STKTE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
include_directories(${STKTE_SOURCE_DIR})

file(GLOB_RECURSE STKTE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${STKTE_SOURCE_DIR}/*.cpp")

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found")
endif()

if(WIN32)
    add_subdirectory("${PROJECT_SOURCE_DIR}/lib/zlib")
    include_directories("${PROJECT_SOURCE_DIR}/lib/zlib")
    set(ZLIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/zlib" "${PROJECT_BINARY_DIR}/lib/zlib/")
    set(ZLIB_LIBRARY zlibstatic)

    add_definitions(-DPHYSFS_BUILD_STATIC)
    add_subdirectory("${PROJECT_SOURCE_DIR}/lib/physfs")
    include_directories("${PROJECT_SOURCE_DIR}/lib/physfs")
    set(PHYSFS_LIBRARY physfs-static)
else()
    find_package(ZLIB REQUIRED)
    find_package(PhysFS REQUIRED)
endif()

add_subdirectory("${PROJECT_SOURCE_DIR}/lib/irrlicht")
include_directories("${PROJECT_SOURCE_DIR}/lib/irrlicht/include")

if(WIN32)
    add_definitions(-D_IRR_STATIC_LIB_)
endif()

if(UNIX)
    find_package(X11 REQUIRED)
    list(APPEND LINK_LIBRARIES ${X11_LIBRARIES} "-lXxf86vm" -lX11 "-Wl,-O1")
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



add_executable(${PROJECT_NAME} ${STKTE_SOURCES})

# Link options
target_link_libraries(
    ${PROJECT_NAME}
    irrlicht
    ${LINK_LIBRARIES}
    ${PHYSFS_LIBRARY}
    ${OPENGL_LIBRARIES}
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} winmm)
endif()
