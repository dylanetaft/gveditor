cmake_minimum_required(VERSION 3.17)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)
include(ExternalProject)

# Set the C++ Version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gvtest
    VERSION 0.1
    LANGUAGES CXX)



find_package(PkgConfig REQUIRED)

find_package(EXPAT REQUIRED)
find_package(ZLIB REQUIRED)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)
pkg_check_modules(libgvc REQUIRED IMPORTED_TARGET libgvc)
pkg_check_modules(libxdot REQUIRED IMPORTED_TARGET libxdot)

# Collect our header files
file(GLOB GVTEST_HEADERS_LIST CONFIGURE_DEPENDS ${MyProject_SOURCE_DIR}/include/*.h)

# Set our source files
set(GVTEST_SRC_LIST
    src/test.cpp)

# Set the include directory
#target_include_directories(gvtest PUBLIC include)

find_package(SDL3 REQUIRED)

add_executable(gvtest test.cpp)


find_library(IMGUI_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libimgui.a)

# Add libs manually in dependency order
target_link_libraries(gvtest PRIVATE
    SDL3::SDL3-static
    nanosvg
    nanosvgrast
    -limgui
    -lgvplugin_core
    -lgvplugin_dot_layout
    -lgvplugin_neato_layout
    -lxdot
    -lgvc
    -lpathplan
    -lcgraph
    -lcdt
    -lexpat
    -lz
)


install(TARGETS gvtest
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)