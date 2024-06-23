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


if (BUILD_EXAMPLE)
	add_subdirectory(examplesData)
	return()
endif()

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

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	message("Build Simple Game Engine v${VERSION}")
else ()
	message("Build Simple Game Engine v${VERSION} in debug mode")
endif()

#######################################


# compiler options
set (CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use

if (WIN32)
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2") #optimization
else ()
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3") #optimization
endif ()

if(CMAKE_BUILD_TYPE STREQUAL "Release")  # set release or not status for source code
	add_definitions(-DNDBUG=FALSE)
else ()
	add_definitions(-DNDBUG=TRUE)
endif ()

#######################################

include_directories(include) # setup SGE includes

file (GLOB SOURCE src/*.cpp)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(SGE SHARED ${SOURCE})

#######################################

# find additional dependecies
find_package(Vulkan 1.3.283.0 REQUIRED)
find_package(SGR 0.8.2 EXACT)

set(GLM_INCLUDE_DIRS modules/glm) # include libraries
set(STB_INC_DIR modules/stb)

#######################################

include_directories(${GLM_INCLUDE_DIRS})
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${STB_INC_DIR})
include_directories(${SGR_INCLUDE_DIRS})

target_link_libraries (SGE ${SGR_LIBRARIES} ${Vulkan_LIBRARIES})

if (WIN32)
	find_library(glfw glfw3 $ENV{GLFW3_LIB}/lib NO_DEFAULT_PATH)
	include_directories($ENV{GLFW3_LIB}/include/GLFW)
	target_link_libraries (SGE ${glfw})
else (WIN32)
	find_package(glfw3 3.3 REQUIRED)
	target_link_libraries (SGE glfw)
	get_target_property(GLFW3_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)
	include_directories(${GLFW3_INCLUDE_DIRS}/GLFW)
endif (WIN32)

# linking MacOSX framework libraries
if (APPLE)
	target_link_libraries(SGE "-framework CoreFoundation")
endif (APPLE)

#######################################

set(BUILD_DEST ${CMAKE_CURRENT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/SGE-v${VERSION}) # set build destination
set_target_properties(SGE PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BUILD_DEST}/lib OUTPUT_NAME "SGE")

# install process
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/SGE-v${VERSION})
endif ()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SGE)
install(TARGETS SGE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)

if (NOT WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(TARGET SGE
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} --install .
	)
endif ()

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

##########################################################################