cmake_minimum_required(VERSION 3.12)
project(123)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.66.0 COMPONENTS system context thread filesystem REQUIRED)
aux_source_directory(. DIR_LIB_SRCS)
add_library(data_fetch ${DIR_LIB_SRCS} DataCenter.cpp DataFetch.cpp DataCenter.h DataFetch.h PreProcess.cpp PreProcess.h HttpHeader.cpp PreProcess.cpp)

TARGET_LINK_LIBRARIES(data_fetch ${Boost_LIBRARIES})