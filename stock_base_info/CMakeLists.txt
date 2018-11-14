cmake_minimum_required(VERSION 3.12)
project(123)

set(CMAKE_CXX_STANDARD 14)
aux_source_directory(. DIR_LIB_SRCS)
add_library(stock_base_info ${DIR_LIB_SRCS})