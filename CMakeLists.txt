cmake_minimum_required(VERSION 3.10)
project(MySuperbuild)

# Add ExternalProject module
include(ExternalProject)

SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install) #set library install path
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/install) #make libraries available as they are built

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

ExternalProject_Add(
    sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.10
    CMAKE_ARGS 
        -DSDL_SHARED=OFF 
        -DSDL_STATIC=ON
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    UPDATE_DISCONNECTED TRUE
)


ExternalProject_Add(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
    DEPENDS sdl
    UPDATE_DISCONNECTED TRUE
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    CMAKE_ARGS 
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}  # Set the install prefix
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}  # Set the prefix path to SDL
    CONFIGURE_COMMAND ""  # imgui has no CMakeLists
    BUILD_BYPRODUCTS ${CMAKE_INSTALL_PREFIX}/lib/libimgui.a
    BUILD_COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} 
                  -I<SOURCE_DIR> 
                  -I${CMAKE_INSTALL_PREFIX}/include  
                  -c 
                  <SOURCE_DIR>/imgui.cpp
                  <SOURCE_DIR>/imgui_demo.cpp
                  <SOURCE_DIR>/imgui_draw.cpp
                  <SOURCE_DIR>/imgui_tables.cpp
                  <SOURCE_DIR>/imgui_widgets.cpp
                  <SOURCE_DIR>/backends/imgui_impl_sdl3.cpp
                  <SOURCE_DIR>/backends/imgui_impl_sdlrenderer3.cpp
                  COMMAND sh -c "${CMAKE_AR} rcs <BINARY_DIR>/libimgui.a <BINARY_DIR>/*.o"
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/include/imgui 
        COMMAND ${CMAKE_COMMAND} -E copy <BINARY_DIR>/libimgui.a ${CMAKE_INSTALL_PREFIX}/lib/
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/imgui.h ${CMAKE_INSTALL_PREFIX}/include/imgui/
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/imgui_internal.h ${CMAKE_INSTALL_PREFIX}/include/imgui/
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/imconfig.h ${CMAKE_INSTALL_PREFIX}/include/imgui/
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/backends/imgui_impl_sdl3.h ${CMAKE_INSTALL_PREFIX}/include/imgui
        COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/backends/imgui_impl_sdlrenderer3.h ${CMAKE_INSTALL_PREFIX}/include/imgui
)



ExternalProject_Add(
    graphviz
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    #WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/graphviz
    URL https://gitlab.com/api/v4/projects/4207231/packages/generic/graphviz-releases/12.2.1/graphviz-12.2.1.tar.gz
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND sh -c "./configure --prefix=${CMAKE_INSTALL_PREFIX} --with-qt=no --with-gtk-no --with-x=no --with-gtkgl=no --with-glut=no --enable-php=no --enable-r=no --disable-shared --enable-static"
    #BUILD_COMMAND ${MAKE_EXECUTABLE}
    #INSTALL_COMMAND sh -c "${MAKE_EXECUTABLE} install"
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)


ExternalProject_Add(
    test
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    DEPENDS sdl imgui graphviz
    BUILD_ALWAYS TRUE
)

