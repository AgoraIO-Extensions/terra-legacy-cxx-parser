cmake_minimum_required(VERSION 3.11)

include(FetchContent)

project(cppast_backend)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Fetching cxxopts")
FetchContent_Declare(cxxopts URL https://github.com/jarro2783/cxxopts/archive/v3.0.0.zip)
FetchContent_MakeAvailable(cxxopts)

add_subdirectory("${TERRA_PATH}" terra)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(cppast_backend "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc")
target_link_libraries(cppast_backend PRIVATE terra cxxopts ${OPENSSL_LIBRARIES})
