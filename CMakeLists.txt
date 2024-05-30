##########################################################################

cmake_minimum_required(VERSION 3.10)

project(SGE DESCRIPTION "Simple Game Engine")

##########################################################################

file(STRINGS "./include/version.h" ver)

string(REGEX MATCH "SGE_VERSION_MAJOR ([0-9]*)" _ ${ver})
set(VMAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "SGE_VERSION_MINOR ([0-9]*)" _ ${ver})
set(VMINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "SGE_VERSION_PATCH ([0-9]*)" _ ${ver})
set(VPATCH ${CMAKE_MATCH_1})

set(VERSION ${VMAJOR}.${VMINOR}.${VPATCH})

# Config version file generate 
set(SGE_CONFIG_VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/SGEConfigVersion.cmake")
set(SGE_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/SGEConfig.cmake")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/SGEConfigVersion.cmake.in"
  "${SGE_CONFIG_VERSION_FILE}"
  @ONLY
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/SGEConfig.cmake.in"
  "${SGE_CONFIG_FILE}"
  @ONLY
)

#######################################

list(APPEND CMAKE_MODULE_PATH "/usr/local/include/SGR/")

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
find_package(Vulkan 1.3.283 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(SGR 0.7.4 REQUIRED)

set(GLM_INCLUDE_DIRS modules/glm) # include libraries
get_target_property(GLFW_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)

#######################################

include_directories(${GLM_INCLUDE_DIRS})
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${SGR_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})

target_link_libraries (SGE ${SGR_LIBRARIES} ${Vulkan_LIBRARIES} glfw)

if (RELEASE)
	target_link_libraries (SGE_ST ${SGR_LIBRARIES} ${Vulkan_LIBRARIES} glfw)
endif (RELEASE)

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