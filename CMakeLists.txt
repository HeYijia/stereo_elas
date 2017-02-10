# project
cmake_minimum_required (VERSION 2.8)
project (libelas)

# directories
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBELAS_SRC_DIR src)

# include directory
include_directories("${LIBELAS_SRC_DIR}")

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

# sources
FILE(GLOB LIBELAS_SRC_FILES "src/*.cpp")
add_library(elaslib ${LIBELAS_SRC_FILES})
# make release version
set(CMAKE_BUILD_TYPE Debug)

# build demo program
add_executable(elas src/main.cpp)
target_link_libraries(elas elaslib)
