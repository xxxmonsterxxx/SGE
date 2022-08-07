##########################################################################

project(SGE_test)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -ObjC++")
set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(glfw3 3.3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exampleApplication)

get_filename_component(SGE_FOLDER ../ ABSOLUTE)

file(GLOB SOURCES test.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

set(GLM_INC_DIR ~/Programming/Library/glm)
find_library(SGR_LIB SGR)

find_library(SGE_LIB SGE ${SGE_FOLDER}/build/debug/shared/ NO_DEFAULT_PATH)

include_directories(${SGE_FOLDER}/include ${GLM_INC_DIR})
target_link_libraries (${PROJECT_NAME} glfw ${SGE_LIB})

file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

###########################################################################