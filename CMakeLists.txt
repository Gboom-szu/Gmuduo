cmake_minimum_required(VERSION 3.23)

project(Gmuduo CXX)


set(CXX_FLAGS 
-std=c++17)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)


add_subdirectory(gmuduo)
add_subdirectory(thirdparty/spdlog)