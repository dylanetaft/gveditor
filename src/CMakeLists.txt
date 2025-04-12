cmake_minimum_required(VERSION 3.17)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)

# Set the C++ Version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gvtest
    VERSION 0.1
    LANGUAGES CXX)




link_directories(${CMAKE_INSTALL_PREFIX}/lib)



# Collect our header files
file(GLOB GVTEST_HEADERS_LIST CONFIGURE_DEPENDS ${MyProject_SOURCE_DIR}/include/*.h)

# Set our source files
set(GVTEST_SRC_LIST
    src/test.cpp)

# Set the include directory
#target_include_directories(gvtest PUBLIC include)

find_package(SDL3 REQUIRED)

# This is our only executable currently
add_executable(gvtest test.cpp)

target_link_libraries(gvtest PUBLIC SDL3::SDL3-static libimgui.a)

install(TARGETS gvtest
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)