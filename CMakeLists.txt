##########################################################################

cmake_minimum_required(VERSION 3.10)

project(SGE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17" )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -ObjC++")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

file(GLOB SOURCES *.cpp )

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
set(SGR_DIR ~/Programming/Library/SGR/0.1.0)

find_library(SGR sgr ${SGR_DIR}/lib/shared)

set(GLM_INC_DIR modules/glm/)
set(STB_INC_DIR modules/stb/)

include_directories(${PROJECT_NAME} ${SGR_DIR}/include ${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${STB_INC_DIR})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${SGR} glfw ${Vulkan_LIBRARIES})

###########################################################################