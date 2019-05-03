cmake_minimum_required(VERSION 3.12)
project(123)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.66.0 COMPONENTS system context thread filesystem REQUIRED)
aux_source_directory(. DIR_LIB_SRCS)
add_library(data_center ${DIR_LIB_SRCS} DataCenter.cpp DataFetch.cpp DataCenter.h DataFetch.h time_index_fetch.h PreProcess.cpp PreProcess.h HttpHeader.cpp PreProcess.cpp time_index_fetch.cpp db_connection_pool.h)

TARGET_LINK_LIBRARIES(data_center ${Boost_LIBRARIES})