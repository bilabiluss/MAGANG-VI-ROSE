cmake_minimum_required(VERSION 3.15)

project(TugasParsing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include/")

add_executable(tugas parsingtwo.cpp)

target_compile_definitions(tugas PRIVATE 
    DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/XL")