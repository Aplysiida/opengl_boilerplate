cmake_minimum_required(VERSION 3.1)
project(opengl_boilerplate)

#Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/src/*.cpp)
	
#Add header files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp)

#Add lib files
link_directories(${CMAKE_SOURCE_DIR}/ext/lib)

#Configure assets header file
configure_file(src/helpers/RootDir.h.in src/helpers/RootDir.h)
include_directories(${CMAKE_BINARY_DIR}/src)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# We need a CMAKE_DIR with some code to find external dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR} /cmake/")

#Libraries
# OpenGL
find_package(OpenGL REQUIRED)

# GLM
#find_package(GLM REQUIRED)
#message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

# GLFW
add_subdirectory("${PROJECT_SOURCE_DIR}/ext/glfw")
include_directories("${PROJECT_SOURCE_DIR}/ext/glfw/include")

#this doesn't work since require glfw to be in program x86 folder, just put glfw into project
#find_package(GLFW3 REQUIRED)
#message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

#header-only libraries
# GLAD
add_library(GLAD "${CMAKE_SOURCE_DIR} /ext/cfiles/glad.c")

# Put all libraries into a variable
set(LIBS glfw3 opengl32 GLAD)

# Define the include DIRs
include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/ext/include"
)

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})

# Create virtual folders to make it look nicer in VS
if(MSVC_IDE)
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

		foreach(child ${children})
			if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()

	# Run macro
	GroupSources(src)
endif()
