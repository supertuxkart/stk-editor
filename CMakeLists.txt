project(stk-editor)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(STKTE_SOURCE_DIR "src/")

file(GLOB_RECURSE STKTE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${STKTE_SOURCE_DIR}*.hpp")
file(GLOB_RECURSE STKTE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${STKTE_SOURCE_DIR}*.cpp")

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PhysFS REQUIRED)

if(WIN32)
    message(WARNING "Link IRRLICHT manually: cmake -DIRRLICHT_INCLUDE_DIRS=<path> -DIRRLICHT_LIBRARIES=<library path>")
else()
    find_package(Irrlicht)
    list(APPEND LINK_LIBRARIES)
endif()

if(NOT IRRLICHT_LIBRARIES OR NOT IRRLICHT_INCLUDE_DIR)
    message(FATAL_ERROR "IRRLICHT include not found, install irrlicht or give the path yourself: -DIRRLICHT_INCLUDE_DIRS=<path> -DIRRLICHT_LIBRARIES=<path>")
endif()
list(APPEND LINK_LIBRARIES ${IRRLICHT_LIBRARIES})

if(UNIX)
    add_definitions(-Wall -Wextra -g)
    
    find_package(X11 REQUIRED)
    list(APPEND LINK_LIBRARIES ${X11_LIBRARIES} "-lXxf86vm" "-Wl,-O1")
endif()

# Includes
include_directories(${STKTE_SOURCE_DIR} ${IRRLICHT_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${STKTE_SOURCES} ${STKTE_HEADERS})

# Link options
target_link_libraries(
    ${PROJECT_NAME}
    ${LINK_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${PHYSFS_LIBRARY}
)
