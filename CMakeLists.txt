##########################################################################

cmake_minimum_required(VERSION 3.10)

project(SGE DESCRIPTION "Simple Game Engine")

#######################################

if (RELEASE AND NOT VERSION)
	message(FATAL_ERROR "Release version was not setted! Use -DVERSION=X.Y.Z")
endif (RELEASE AND NOT VERSION)

if (NOT VERSION)
	message("Build Simple Game Engine in debug mode")
else (NOT VERSION)
	message("Build Simple Game Engine v"${VERSION})
endif (NOT VERSION)

if (NOT RELEASE)
	set(RELEASE false)
endif (NOT RELEASE)

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

# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") #optimization

if (NOT RELEASE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT RELEASE)

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
find_package(glfw3 3.3 REQUIRED)

# define if glfw3 found
if (NOT glfw3_DIR)
	set(GLFW3_FOUND FALSE)
else (NOT glfw3_DIR)
	set(GLFW3_FOUND TRUE)
	get_target_property(GLFW_INC_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
endif (NOT glfw3_DIR)


set(GLM_INC_DIR modules/glm) # include libraries

set(SGR_VERSION 0.6.0)
set(SGR_DIR ~/Programming/Library/SGR/${SGR_VERSION})
find_library(SGR_LIB SGR ${SGR_DIR}/lib/shared)
# define if SGR found
if (NOT SGR_LIB)
	set(SGR_FOUND FALSE)
else (NOT SGR_LIB)
	set(SGR_FOUND TRUE)
endif (NOT SGR_LIB)

#######################################

include_directories(${GLM_INC_DIR} )

# status info about external dependecies
if (SGR_FOUND AND GLFW3_FOUND)
    message(STATUS "Found SGR, Including and Linking now")
	message(STATUS "Found GLFW3, Including and Linking now")
    include_directories(${SGR_DIR}/include ${GLFW_INC_DIR})
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

if (RELEASE)
	set(RELEASE_BUILD_DEST build/SGE-v${VERSION}-${PLATFORM_NAME}) # set release build destination
	file(COPY include DESTINATION ${RELEASE_BUILD_DEST})
	set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/shared)
	set_target_properties(SGE_ST PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${RELEASE_BUILD_DEST}/lib/static)
else (RELEASE)
	set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY build/shared)
endif (RELEASE)

#######################################

include(ProcessorCount)
ProcessorCount(N)
message("Ready to build on "${PLATFORM_NAME})
message("Please build using: make -j"  ${N})

##########################################################################