cmake_minimum_required(VERSION 3.0.0)

# project settings

project(l3d CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# dependencies

set(dep_dir ${CMAKE_SOURCE_DIR}/dependencies)
set(glfw_dir ${dep_dir}/glfw)
set(logger_dir ${dep_dir}/logger)
set(CMAKE_PREFIX_PATH "${glfw_dir}")

find_package(glfw3)
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "glfw not found")
endif()
include_directories(${glfw_dir}/include)

find_library(logger logger PATHS ${logger_dir}/lib)
if(NOT logger)
    message(FATAL_ERROR "logger not found")
endif()
include_directories(${logger_dir}/include)

# sources

set(source_dir src)
set(include_dir include)

include_directories(${include_dir})

set(sources ${source_dir}/l3d.cpp)

# executable

add_executable(l3d ${headers} ${sources})
target_link_libraries(l3d PRIVATE glfw ${logger})
