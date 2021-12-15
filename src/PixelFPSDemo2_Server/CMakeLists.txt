cmake_minimum_required (VERSION 3.8)

#set C++ version:
set(CMAKE_CXX_STANDARD 17)

#link:https://stackoverflow.com/questions/1662909/undefined-reference-to-pthread-create-in-linux
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

include_directories("../../asio-1.20.0_include/")

include_directories("../PixelFPSDemo2/")

add_executable (PixelFPSDemo2_Server "PixelFPSDemo2_Server.cpp" )
