cmake_minimum_required(VERSION 3.12)
project(123)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.66.0 COMPONENTS system context thread REQUIRED)

aux_source_directory(. DIR_SRCS)
add_subdirectory(algorithm)
add_subdirectory(data_fetch)
add_subdirectory(stock_base_info)
#add_executable(123 main.cpp data_fetch/DataFetch.cpp data_fetch/DataFetch.h algorithm/Calculaotr.cpp algorithm/Calculaotr.h stock_base_info/stock_info.cpp stock_base_info/stock_info.h data_fetch/HttpHeader.cpp data_fetch/HttpHeader.h)
#add_executable(123 ${DIR_SRCS} output/FileResultOutput.cpp output/FileResultOutput.h)
add_executable(123 ${DIR_SRCS} output/FileResultOutput.cpp output/FileResultOutput.h)

TARGET_LINK_LIBRARIES(123 data_fetch)
TARGET_LINK_LIBRARIES(123 algorithm)
TARGET_LINK_LIBRARIES(123 ${Boost_LIBRARIES})