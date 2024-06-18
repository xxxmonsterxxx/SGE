##########################################################################

project(SGE_test)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17) #standard version is 17 - minimum for "optional" use

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exampleApplication)

get_filename_component(SGE_FOLDER ../ ABSOLUTE)

file(GLOB SOURCES test.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

if (WIN32)
	SET(glfw $ENV{GLFW3_LIBRARIES})
	include_directories($ENV{GLFW3_INCLUDE_DIRS})
	target_link_libraries (${PROJECT_NAME} ${glfw})
else (WIN32)
    find_package(glfw3 3.3 REQUIRED)
    target_link_libraries (${PROJECT_NAME} glfw)
    get_target_property(GLFW3_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)
    include_directories(${GLFW3_INCLUDE_DIRS}/GLFW)
endif (WIN32)

find_package(Vulkan 1.3.283.0 REQUIRED)
find_package(SGR 0.8.1 EXACT)

set(GLM_INC_DIR ${SGE_FOLDER}/modules/glm)

set(SGE_FOLDER ${CMAKE_SOURCE_DIR}/build/SGE-v${VERSION})
find_library(SGE_LIB SGE ${SGE_FOLDER}/lib NO_DEFAULT_PATH)

include_directories(${SGE_FOLDER}/include/SGE ${Vulkan_INCLUDE_DIRS} ${GLM_INC_DIR} ${SGR_INCLUDE_DIRS})
target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} ${SGR_LIBRARIES} ${SGE_LIB})

file(COPY Resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

if (WIN32)
    set(DLL_FILES ${CMAKE_SOURCE_DIR}/build/SGE-v${VERSION}/lib/SGE.dll)

    add_custom_target(CopyDlls ALL
        COMMENT "Copying dynamic libraries to output directory"
        DEPENDS ${DLL_FILES}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL_FILES}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    add_dependencies(${PROJECT_NAME} CopyDlls)
endif()

###########################################################################