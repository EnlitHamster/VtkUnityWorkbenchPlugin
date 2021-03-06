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
set(ADAPTERS_DIR ../source/Adapters)
set(INITIALISATION_DIR ../source/Initialisation)
set(HEADER_DIR_1 ../source/GLEW)
set(HEADER_DIR_2 ../source)
set(HEADER_DIR_3 ../source/Unity)
set(HEADER_DIR_4 ../source/Adapters)
set(INTROSPECTION_DIR ../source/Introspection)

############## CMake Project ################
#        The main options of project        #
#############################################

project(${PROJECT_NAME} CXX)
# CXX version
set(CMAKE_CXX_STANDARD 11)


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


# Define Python libraries
set(PythonLibs_VERSION 3.7 CACHE STRING "Python version used to build VTK")
find_package(PythonLibs ${PythonLibs_VERSION} EXACT REQUIRED)
string(REPLACE "." "" Python_VERSION ${PythonLibs_VERSION})
include_directories(${PYTHON_INCLUDE_DIRS})


# Define VTK directory
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


# Add vtk headers and lib directory and change their backslash to forward slash in Windows
#include_directories(${VTK_INCLUDE_DIR})
#link_directories(${VTK_LIB_DIR})

if(MSVC AND VTK_DIR)
    string(REPLACE "\\" "/" VTK_DIR ${VTK_DIR})
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
SET(MY_OWN_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../../VtkToUnityExample/Assets/VtkToUnity/vtktounitybinary/Plugins" CACHE PATH "Prefix prepended to install directories")
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

file(GLOB INC_UNITY "${HEADER_DIR_3}/*.h")
file(GLOB SRC_GLEW "${GLEW_DIR}/*.c")
file(GLOB INC_GLEW "${HEADER_DIR_1}/*.h")
file(GLOB SRC_INFRA "${CPP_DIR}/*.cpp" "${INITIALISATION_DIR}/*.cpp")
file(GLOB INC_INFRA "${HEADER_DIR_2}/*.h" "${INITIALISATION_DIR}/*.h")
file(GLOB SRC_ADAPT "${ADAPTERS_DIR}/*.cpp")
file(GLOB INC_ADAPT "${HEADER_DIR_4}/*.h")
file(GLOB SPT_ADAPT "${ADAPTERS_DIR}/*.py")
file(GLOB SRC_INTRO "${INTROSPECTION_DIR}/*.cpp")
file(GLOB INC_INTRO "${INTROSPECTION_DIR}/*.h")
file(GLOB SPT_INTRO "${INTROSPECTION_DIR}/*.py")

# Add library to build.
add_library(${PROJECT_NAME} SHARED
    ${INC_UNITY}
    ${SRC_GLEW}
    ${INC_GLEW}
    ${SRC_INFRA}
    ${INC_INFRA}
    ${SRC_ADAPT}
    ${INC_ADAPT}
    ${SPT_ADAPT}
    ${SRC_INTRO}
    ${INC_INTRO}
    ${SPT_INTRO}
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    source_group(Unity FILES ${INC_UNITY})
    source_group("GLEW\\Source Files" FILES ${SRC_GLEW})
    source_group("GLEW\\Header Files" FILES ${INC_GLEW})
    source_group("Infrastructure\\Source Files" FILES ${SRC_INFRA})
    source_group("Infrastructure\\Header Files" FILES ${INC_INFRA})
    source_group("Adapters\\Source Files" FILES ${SRC_ADAPT})
    source_group("Adapters\\Header Files" FILES ${INC_ADAPT})
    source_group(Adapters\\Scripts FILES ${SPT_ADAPT})
    source_group("Introspection\\Source Files" FILES ${SRC_INTRO})
    source_group("Introspection\\Header Files" FILES ${INC_INTRO})
    source_group(Introspection\\Scripts FILES ${SPT_INTRO})
endif()

# these are the VTK depedencies required for volume load and display
set (VTK_REQUIRED_LIBS_A 
    vtkalglib-8.1.dll 
    vtkCommonColor-8.1.dll 
    vtkCommonComputationalGeometry-8.1.dll 
    vtkCommonCore-8.1.dll 
    vtkCommonDataModel-8.1.dll 
    vtkCommonExecutionModel-8.1.dll 
    vtkCommonMath-8.1.dll 
    vtkCommonMisc-8.1.dll 
    vtkCommonSystem-8.1.dll 
    vtkCommonTransforms-8.1.dll 
    vtkDICOMParser-8.1.dll 
    vtkFiltersCore-8.1.dll 
    vtkFiltersExtraction-8.1.dll 
    vtkFiltersGeneral-8.1.dll 
    vtkFiltersGeometry-8.1.dll 
    vtkFiltersSources-8.1.dll
	vtkWrappingPython${Python_VERSION}Core-8.1.dll
)

set (VTK_REQUIRED_LIBS_B 
    vtkFiltersSources-8.1.dll 
    vtkFiltersStatistics-8.1.dll 
    vtkfreetype-8.1.dll 
    vtkgl2ps-8.1.dll 
    vtkglew-8.1.dll 
    vtkImagingCore-8.1.dll 
    vtkImagingFourier-8.1.dll 
    vtkImagingMath-8.1.dll 
    vtkInteractionStyle-8.1.dll 
    vtkIOImage-8.1.dll 
    vtkjpeg-8.1.dll 
)

set (VTK_REQUIRED_LIBS_C 
    vtkmetaio-8.1.dll 
    vtkpng-8.1.dll 
    vtkRenderingCore-8.1.dll 
    vtkRenderingExternal-8.1.dll 
    vtkRenderingFreeType-8.1.dll 
    vtkRenderingGL2PSOpenGL2-8.1.dll 
    vtkRenderingOpenGL2-8.1.dll 
    vtkRenderingVolume-8.1.dll 
    vtkRenderingVolumeOpenGL2-8.1.dll 
    vtksys-8.1.dll 
    vtktiff-8.1.dll 
    vtkzlib-8.1.dll
)

# Generate libraries for copying
list(GET PYTHON_LIBRARIES 1 PYTHON_LIB_0)
get_filename_component(PYTHON_LIBRARY_DIR_RELEASE ${PYTHON_LIB_0} DIRECTORY)
message(STATUS "Python Library path ${PYTHON_LIBRARY_DIR_RELEASE} from ${PYTHON_LIB_0}")
foreach(PYTHON_FILE ${PYTHON_LIBRARIES})
    if(EXISTS ${PYTHON_FILE})
        get_filename_component(TMP_COMPONENT ${PYTHON_FILE} NAME)
        list(APPEND PYTHON_LIBRARY_NAMES ${TMP_COMPONENT})
    endif()
endforeach()
message(STATUS "Python library names: ${PYTHON_LIBRARY_NAMES}")

# Add post build events to Windows
if(MSVC)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND del /Q \"${UNITY_PLUGIN_PATH}\\*.dll\"

        COMMAND copy /Y \"$(TargetPath)\" \"${UNITY_PLUGIN_PATH}/$(TargetFileName)\"
        COMMAND robocopy \"${PYTHON_LIBRARY_DIR_RELEASE}\" \"${UNITY_PLUGIN_PATH}/\" ${PYTHON_LIBRARY_NAMES} /NFL /NDL /NJH /NJS /nc /ns /np \n if %errorlevel% leq 1 set errorlevel = 0
        COMMAND robocopy \"${VTK_DIR}/bin/$<CONFIGURATION>\" \"${UNITY_PLUGIN_PATH}/\" ${VTK_REQUIRED_LIBS_A} /NFL /NDL /NJH /NJS /nc /ns /np \n if %errorlevel% leq 1 set errorlevel = 0
        COMMAND robocopy \"${VTK_DIR}/bin/$<CONFIGURATION>\" \"${UNITY_PLUGIN_PATH}/\" ${VTK_REQUIRED_LIBS_B} /NFL /NDL /NJH /NJS /nc /ns /np \n if %errorlevel% leq 1 set errorlevel = 0
        COMMAND robocopy \"${VTK_DIR}/bin/$<CONFIGURATION>\" \"${UNITY_PLUGIN_PATH}/\" ${VTK_REQUIRED_LIBS_C} /NFL /NDL /NJH /NJS /nc /ns /np \n if %errorlevel% leq 1 set errorlevel = 0
    )
endif()


if(MSVC)
   target_link_libraries(${PROJECT_NAME} opengl32.lib 
     ${PYTHON_LIBRARIES}
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
