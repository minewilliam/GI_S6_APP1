cmake_minimum_required(VERSION 3.1)

project(AssetConv VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_BUILD_TYPE            Debug) # Change to RelWithDebInfo or Release

add_library(nanosvg     STATIC  src/nanosvg.c)
add_library(stb_image   STATIC  src/stb_image.c)

add_executable(asset_conv src/asset_conv.cpp)
target_link_libraries(asset_conv nanosvg stb_image pthread)
