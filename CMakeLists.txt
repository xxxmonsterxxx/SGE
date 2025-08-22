############ S I M P L E   G A M E   E N G I N E #########################

cmake_minimum_required(VERSION 3.16)

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

set(BUILD_DEST ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/SGE-v${VERSION}-${CMAKE_SYSTEM_NAME}) # set build destination

#######################################

if(CMAKE_BUILD_TYPE STREQUAL "Release") # set release or not status for source code
	message("Build Simple Game Engine v${VERSION}")
	add_definitions(-DSGE_DEBUG=false)
else ()
	message("Build Simple Game Engine v${VERSION} in debug mode")
	add_definitions(-DSGE_DEBUG=true)
	add_definitions(-DNDBUG=true)
endif()

#######################################

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


# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use

if (WIN32)
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2") #optimization
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")  # /MD runtime library
else ()
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3") #optimization
endif ()

#######################################

include_directories(include) # setup SGE includes
file (GLOB SOURCE src/*.cpp)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH "@executable_path")
endif()

add_library(SGE SHARED ${SOURCE})

set_target_properties(SGE PROPERTIES
    VERSION ${VERSION}
    SOVERSION ${VMAJOR}.${VMINOR}.0
)

#######################################

# find additional dependecies
find_package(Vulkan 1.3.283.0 REQUIRED)
find_package(SGR 0.12 REQUIRED)

set(GLM_INCLUDE_DIRS modules/glm) # include libraries
set(STB_INC_DIR modules/stb)
set(TINY_OBJ_LOADER_INC_DIR modules/tiny_obj_loader)
set(TINY_GLTF_LOADER_INC_DIR modules/tiny_gltf_loader)

#######################################

include_directories(${TINY_OBJ_LOADER_INC_DIR})
include_directories(${TINY_GLTF_LOADER_INC_DIR})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${STB_INC_DIR})
include_directories(${SGR_INCLUDE_DIRS})

target_link_libraries (SGE PRIVATE ${SGR_LIBRARIES} ${Vulkan_LIBRARIES})

if (WIN32)
	find_library(glfw glfw3 $ENV{GLFW3_LIB}/lib NO_DEFAULT_PATH)
	include_directories($ENV{GLFW3_LIB}/include/GLFW)
	target_link_libraries (SGE PRIVATE ${glfw})
else (WIN32)
	find_package(glfw3 3.3 REQUIRED)
	target_link_libraries (SGE PRIVATE glfw)
	get_target_property(GLFW3_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)
	include_directories(${GLFW3_INCLUDE_DIRS}/GLFW)
endif (WIN32)

# linking MacOSX framework libraries
if (APPLE)
	target_link_libraries(SGE PRIVATE "-framework CoreFoundation")

	add_custom_command(TARGET SGE POST_BUILD
    	COMMAND install_name_tool -change /opt/homebrew/opt/glfw/lib/libglfw.3.dylib @rpath/libglfw.3.dylib $<TARGET_FILE:SGE>
	)
endif (APPLE)

#######################################

FILE(COPY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${BUILD_DEST}/include/SGE)
set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUILD_DEST}/lib OUTPUT_NAME "SGE"
									 ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DEST}/lib OUTPUT_NAME "SGE"
									 RUNTIME_OUTPUT_DIRECTORY  ${BUILD_DEST}/lib OUTPUT_NAME "SGE")

# install process
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/SGE-v${VERSION})
endif ()

#######################################

# post build actions

# post build command for copy include files into build directory
add_custom_command(
    TARGET SGE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/include
        ${BUILD_DEST}/include/SGE
    COMMENT "Copying includes to build directory..."
)

# install options

install(DIRECTORY ${BUILD_DEST}/include/SGE DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS SGE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)

if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
	install(CODE "
		execute_process(
        COMMAND reg add \"HKCU\\\\Environment\" /v SGE_LIB /t REG_SZ /d \"${CMAKE_INSTALL_PREFIX}/lib\" /f
        OUTPUT_VARIABLE result
        ERROR_VARIABLE result
        RESULT_VARIABLE exit_code
		)
	"
	)

	install(CODE "
		execute_process(
        COMMAND reg add \"HKCU\\\\Environment\" /v CMAKE_PREFIX_PATH /t REG_SZ /d \"${CMAKE_INSTALL_PREFIX}/include/SGE;$ENV{CMAKE_PREFIX_PATH}\" /f
        OUTPUT_VARIABLE result
        ERROR_VARIABLE result
        RESULT_VARIABLE exit_code
		)
	"
	)
endif ()
#######################################

include(ProcessorCount)
ProcessorCount(N)

#######################################

if (BUILD_EXAMPLE)
	add_subdirectory(examplesData)
	return()
endif()

##########################################################################