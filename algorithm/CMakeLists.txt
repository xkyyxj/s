cmake_minimum_required(VERSION 3.12)
project(123)

set(CMAKE_CXX_STANDARD 14)
include_directories(../stock_base_info)
aux_source_directory(. DIR_LIB_SRCS)
add_library(algorithm ${DIR_LIB_SRCS} Calculator.h)