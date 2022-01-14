cmake_minimum_required(VERSION 3.15)
#project name
PROJECT(ffmpegTest)

find_package(SDL2 REQUIRED)
INCLUDE_DIRECTORIES(
include
${SDL2_INCLUDE_DIRS}
)
#source directory
AUX_SOURCE_DIRECTORY(./source DIR_SRCS)

add_compile_options(-std=c++11)


SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

link_directories(lib)

#add executable file
ADD_EXECUTABLE(ffmpegTest ${DIR_SRCS})
#add link library
TARGET_LINK_LIBRARIES(ffmpegTest 
    avdevice
    avfilter
    avformat
    avcodec
    avutil
    swscale
    ${SDL2_LIBRARIES}
    )
 