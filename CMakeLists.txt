cmake_minimum_required(VERSION 3.10)

project(SGE DESCRIPTION "Simple Game Engine")

include_directories(include)

SET(RELEASE TRUE)
SET(VERSION 0.2.0)
SET(SGR_VERSION 0.6.0)

SET(SOURCE 	src/GameObject.cpp
			src/Mesh.cpp
			src/sge_utils.cpp
			src/SGE.cpp)

add_library(SGE_ST STATIC ${SOURCE})
add_library(SGE SHARED ${SOURCE})


# compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -ObjC++")

set(GLM_INC_DIR modules/glm) # include libraries

# find additional dependecies
find_package(glfw3 3.3 REQUIRED)
set(SGR_DIR ~/Programming/Library/SGR/${SGR_VERSION})

find_library(SGR_LIB SGR ${SGR_DIR}/lib/shared)

include_directories(${SGR_DIR}/include ${GLM_INC_DIR})
target_link_libraries(SGE ${SGR_LIB} glfw)
target_link_libraries(SGE_ST ${SGR_LIB} glfw)

if (RELEASE)
	file(COPY include DESTINATION build/${VERSION})
	set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY build/${VERSION}/lib/shared)
	set_target_properties(SGE_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY build/${VERSION}/lib/static)
else (RELEASE)
	set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY build/shared)
	set_target_properties(SGE_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY build/static)
endif (RELEASE)

include(ProcessorCount)
ProcessorCount(N)
message("Please build using: make -j"  ${N})
