cmake_minimum_required(VERSION 3.16)

project(KlinikSehat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(json-handler
    src/JsonHandler.cpp
    src/RequestBuilder.cpp
    src/AppConfig.cpp
    src/SerialPort.cpp
)

target_include_directories(json-handler
    PUBLIC
        include
        "\${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include"
)

add_executable(klinik-sehat
    src/main.cpp
)

target_link_libraries(klinik-sehat
    PRIVATE json-handler
)

target_include_directories(json-handler
    PUBLIC
        include
        "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include"
)