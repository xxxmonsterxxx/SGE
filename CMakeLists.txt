##########################################################################

project(SGE DESCRIPTION "Simple Game Engine")

cmake_minimum_required(VERSION 3.10)

##########################################################################

file(STRINGS "./include/version.h" ver)

string(REGEX MATCH "SGE_VERSION_MAJOR ([0-9]*)" _ ${ver})
set(VMAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "SGE_VERSION_MINOR ([0-9]*)" _ ${ver})
set(VMINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "SGE_VERSION_PATCH ([0-9]*)" _ ${ver})
set(VPATCH ${CMAKE_MATCH_1})

set(VERSION ${VMAJOR}.${VMINOR}.${VPATCH})

#######################################

# platform detection
if (UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif (UNIX AND NOT APPLE)

if (APPLE)
	set(PLATFORM_NAME MacOS)
endif (APPLE)

if (LINUX)
	set(PLATFORM_NAME Linux)
endif (LINUX)

if (WIN32)
	set(PLATFORM_NAME Windows)
endif (WIN32)

#######################################

if (RELEASE)
	message("Build Simple Game Engine v${VERSION}")
else (RELEASE)
	message("Build Simple Game Engine in debug mode")
	set(DEBUG TRUE)
endif (RELEASE)

#######################################

# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #optimization

if (DEBUG)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (DEBUG)

add_definitions(-DNDBUG=${RELEASE}) # set release or not status

#######################################

include_directories(include) # setup SGE includes

file (GLOB SOURCE src/*.cpp)

#######################################

add_library(SGE SHARED ${SOURCE})

if (RELEASE)
	add_library(SGE_ST STATIC ${SOURCE})
endif (RELEASE)

#######################################

# find additional dependecies
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# define if glfw3 found
if (NOT glfw3_DIR)
	set(GLFW3_FOUND FALSE)
else (NOT glfw3_DIR)
	set(GLFW3_FOUND TRUE)
	get_target_property(GLFW_INC_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
endif (NOT glfw3_DIR)


set(GLM_INC_DIR modules/glm) # include libraries

find_library(SGR_LIB SGR)
# define if SGR found
if (NOT SGR_LIB)
	set(SGR_FOUND FALSE)
else (NOT SGR_LIB)
	set(SGR_FOUND TRUE)
endif (NOT SGR_LIB)

#######################################

include_directories(${GLM_INC_DIR})

include_directories(${Vulkan_INCLUDE_DIRS})

# status info about external dependecies
if (SGR_FOUND AND GLFW3_FOUND)
    message(STATUS "Found SGR, Including and Linking now")
	message(STATUS "Found GLFW3, Including and Linking now")
    include_directories(${GLFW_INC_DIR})
    target_link_libraries(SGE ${SGR_LIB} glfw)
	if (RELEASE)
		target_link_libraries(SGE_ST ${SGR_LIB})
	endif (RELEASE)
else (SGR_FOUND AND GLFW3_FOUND)
	if (NOT SGR_FOUND)
		message(FATAL_ERROR "SGR not found, please install SGR_FOUND using library README")
	endif (NOT SGR_FOUND)

	if (NOT GLFW3_FOUND)
		message(FATAL_ERROR "GLFW3 not found, please install GLFW3 using library README")
	endif (NOT GLFW3_FOUND)
endif (SGR_FOUND AND GLFW3_FOUND)

#######################################

# library prefix based on paltform
set(RELEASE_BUILD_DEST release/SGE-v${VERSION}-${PLATFORM_NAME}) # set release build destination
set(DEBUG_BUILD_DEST debug) # set release build destination

if (RELEASE)
	file(COPY include DESTINATION ${RELEASE_BUILD_DEST})
	set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/shared)
	set_target_properties(SGE_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/static)
else (RELEASE)
	set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY debug/shared)
endif (RELEASE)

#######################################

include(ProcessorCount)
ProcessorCount(N)
message("Ready to build on ${PLATFORM_NAME}")
message("Please build using: make -j${N}")

##########################################################################