#include <iostream>
#include "data_fetch/DataFetch.h"
#include "data_fetch/PreProcess.h"
#include "data_fetch/DataCenter.h"
#include "./algorithm/Calculator.h"
#include <fstream>
#include <boost/filesystem.hpp>
std::map<std::string,std::string> res_map;
#ifdef WINDOWS_PLATFORM
#define DllExport __declspec( dllexport )//宏定义
#else
#define DllExport
#endif

DllExport std::string get_str() {
    return std::string("12213");
}