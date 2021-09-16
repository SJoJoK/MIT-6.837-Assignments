cmake_minimum_required(VERSION 2.8)
# Set a project name.
if (POLICY CMP0072)
  set(OpenGL_GL_PREFERENCE LEGACY)
endif()
project(raytracer)
# Suppress warnings of the deprecation of glut functions on macOS.
# Find the packages we need.
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
# Linux# If not on macOS, we need glew.
find_package(GLEW REQUIRED)
# OPENGL_INCLUDE_DIR, GLUT_INCLUDE_DIR, OPENGL_LIBRARIES, and GLUT_LIBRARIES
# are CMake built-in variables defined when the packages are found.
set(INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
set(LIBRARIES ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
# If not on macOS, add glew include directory and library path to lists.
list(APPEND INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
list(APPEND LIBRARIES ${GLEW_LIBRARIES})
# Add the list of include paths to be used to search for include files.
include_directories(${INCLUDE_DIRS})
# Search all the .cpp files in the directory where CMakeLists lies and set them to ${SOURCE_FILES}.
# Search all the .h files in the directory where CMakeLists lies and set them to ${INCLUDE_FILES}.
file(GLOB SOURCE_FILES src/*.cpp)
# Add the executable Example to be built from the source files
add_executable(raytracer ${SOURCE_FILES})
# Link the executable to the libraries
target_link_libraries(raytracer ${LIBRARIES})

target_include_directories(raytracer
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)
