# project
cmake_minimum_required (VERSION 2.8)
project (libelas)

# directories
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (LIBELAS_SRC_DIR src)

ADD_DEFINITIONS(-std=c++11 -fPIC -DENABLE_SSE -D__DEBUG -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2)

# use opencv to read and show img
find_package(OpenCV 2.4 REQUIRED)
# use Pangoliin to show 3d points
find_package(Pangolin REQUIRED)
include_directories(${OPENCV_INCLUDE_DIRS}   ${Pangolin_INCLUDE_DIRS})

# include directory
include_directories("${LIBELAS_SRC_DIR}")

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")

# sources
FILE(GLOB LIBELAS_SRC_FILES "src/*.cpp")
add_library(elaslib ${LIBELAS_SRC_FILES} )
# make release version
set(CMAKE_BUILD_TYPE Debug)

# build demo program
add_executable(elas src/main.cpp)
target_link_libraries(elas elaslib ${OpenCV_LIBS} ${Pangolin_LIBRARIES} )

# build demo program with opencv
add_executable(elas_opencv src/main_opencv.cpp)
target_link_libraries(elas_opencv elaslib ${OpenCV_LIBS} ${Pangolin_LIBRARIES} )
