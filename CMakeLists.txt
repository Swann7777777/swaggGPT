cmake_minimum_required(VERSION 3.15)

project(swaggGPT LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

add_executable(main src/main.cpp src/softmax.cpp)

include_directories(include)
