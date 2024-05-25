cmake_minimum_required(VERSION 3.10)

project(engine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(engine main.cpp)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(IL QUIET)  # Attempt to find DevIL

if(NOT IL_FOUND)
  # If CMake doesn't find DevIL, attempt manual configuration
  find_library(DEVIL_LIBRARY NAMES IL PATHS /usr/lib /usr/local/lib)
  find_path(DEVIL_INCLUDE_DIR IL/il.h PATHS /usr/include /usr/local/include)
  if(DEVIL_LIBRARY AND DEVIL_INCLUDE_DIR)
    set(IL_LIBRARIES ${DEVIL_LIBRARY})
    include_directories(${DEVIL_INCLUDE_DIR})
  endif()
endif()

# Include directories
include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# Link libraries
target_link_libraries(engine ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_LIBRARY} ${IL_LIBRARIES})

# Check if IL was found and output result
if(IL_FOUND OR (DEVIL_LIBRARY AND DEVIL_INCLUDE_DIR))
  message(STATUS "DevIL library found.")
else()
  message(FATAL_ERROR "DevIL library not found.")
endif()
