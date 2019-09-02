cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

################### Variables. ####################
# Change if you want modify path or other values. #
###################################################

set(PROJECT_NAME VtkToUnityPlugin)
# Output Variables
set(OUTPUT_DEBUG ../build)
set(OUTPUT_REL ../build)
# Folders files
set(GLEW_DIR ../source/GLEW)
set(CPP_DIR ../source)
set(HEADER_DIR_1 ../source/GLEW)
set(HEADER_DIR_2 ../source)
set(HEADER_DIR_3 ../source/Unity)


############## CMake Project ################
#        The main options of project        #
#############################################

project(${PROJECT_NAME} CXX)


# Define Release by default.
# if(NOT CMAKE_BUILD_TYPE)
  # set(CMAKE_BUILD_TYPE "Release")
  # message(STATUS "Build type not specified: Use Release by default.")
# endif(NOT CMAKE_BUILD_TYPE)

# Definition of Macros
add_definitions(
   -DGLEW_STATIC 
   -D_WINDOWS 
   -D_USRDLL 
   -DRENDERINGPLUGIN_EXPORTS 
   -DUNICODE
   -D_UNICODE
)

# Define VTK directory

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


#set(VTK_INCLUDE_DIR "" CACHE PATH "directory for vtk headers")
#set(VTK_LIB_DIR "" CACHE PATH "directory for vtk libs")
#set(VTK_VERSION "8.0" CACHE STRING "version number of the vtk libs")

# Add vtk headers and lib directory and change their backslash to forward slash in Windows
#include_directories(${VTK_INCLUDE_DIR})
#link_directories(${VTK_LIB_DIR})

if(MSVC AND VTK_DIR)
	#set(VTK_INCLUDE_DIR $ENV{VTK_INCLUDE_DIR})
	#string(REPLACE "\\" "/" VTK_INCLUDE_DIR ${VTK_INCLUDE_DIR})
	string(REPLACE "\\" "/" VTK_DIR ${VTK_DIR})
	#set(VTK_LIB_DIR $ENV{VTK_LIB_DIR})
	#string(REPLACE "\\" "/" VTK_LIB_DIR ${VTK_LIB_DIR})
endif()


############## Artefacts Output #################
# Defines outputs , depending Debug or Release. #
#################################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
endif()

# Set install path, which should be Unity project's plugin folder
SET(MY_OWN_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../../VtkToUnityExample/Assets/Plugins" CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX "${MY_OWN_INSTALL_PREFIX}" CACHE INTERNAL "Prefix prepended to install directories" FORCE)
# Set Unity plugin path
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	set(UNITY_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/x86_64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	# 32 bits
	set(UNITY_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/x86)
endif()	


################# Flags ################
# Defines Flags for Windows and Linux. #
########################################

if(MSVC)
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /Od /Zi /EHsc")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Od /Oi /Gy /Zi /EHsc")
endif(MSVC)
if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   endif()
endif(NOT MSVC)

################ Files ################
#   --   Add files to project.   --   #
#######################################
SET_SOURCE_FILES_PROPERTIES( ${GLEW_DIR}/glew.c PROPERTIES LANGUAGE CXX )

file(GLOB SRC_FILES
    ${GLEW_DIR}/*.c
    ${CPP_DIR}/*.cpp
    ${HEADER_DIR_1}/*.h
    ${HEADER_DIR_2}/*.h
    ${HEADER_DIR_3}/*.h
	${VTK_INCLUDE_DIR}/**/*.h
)

# Add library to build.
add_library(${PROJECT_NAME} SHARED
   ${SRC_FILES}
)

# Add post build events to Windows
if(MSVC)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND rmdir /q /s \"${UNITY_PLUGIN_PATH}\"
					COMMAND mklink /d \"${UNITY_PLUGIN_PATH}\" \"${VTK_DIR}/bin/$<CONFIGURATION>\"
				
					COMMAND copy /Y \"$(TargetPath)\" \"${VTK_DIR}/bin/$<CONFIGURATION>/$(TargetFileName)\")
endif()


if(MSVC)
   target_link_libraries(${PROJECT_NAME} opengl32.lib 
     ${VTK_LIBRARIES})
endif(MSVC)

# Install generated library to target directory
if(MSVC)
	# Windows install to VTK bin
	install(TARGETS ${PROJECT_NAME} DESTINATION ${VTK_DIR}/bin/$<CONFIGURATION>)
else()
	# Otherwise install to UnityProject/Assets/Plugin/
	install(TARGETS ${PROJECT_NAME} DESTINATION ${UNITY_PLUGIN_PATH})
endif()
