##########################################################################

project(SGE_test)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -ObjC++")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

find_package(glfw3 3.3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

get_filename_component(SGE_FOLDER ../ ABSOLUTE)

file(GLOB SOURCES ${SGE_FOLDER}/examplesData/test.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

set(GLM_INC_DIR ~/Programming/Library/glm)
set(SGR_FOLDER ~/Programming/Library/SGR/0.5.1)

find_library(SGE_LIB SGE ${SGE_FOLDER}/build/shared/)

include_directories(${SGE_FOLDER}/include ${SGR_FOLDER}/include ${GLM_INC_DIR})
target_link_libraries (${PROJECT_NAME} glfw ${SGE_LIB})

###########################################################################