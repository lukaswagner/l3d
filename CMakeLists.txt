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

set(shared_include_dir ${include_dir}/shared)

set(l2d_source_dir ${source_dir}/l2d)
set(l2d_sources ${l2d_source_dir}/l2d.cpp)

set(l3d_source_dir ${source_dir}/l3d)
set(l3d_sources ${l3d_source_dir}/l3d.cpp)

# executables

add_executable(l2d ${l2d_sources})
target_link_libraries(l2d PRIVATE glfw ${logger})
target_include_directories(l2d PRIVATE ${shared_include_dir})

add_executable(l3d ${l3d_sources})
target_link_libraries(l3d PRIVATE glfw ${logger})
target_include_directories(l3d PRIVATE ${shared_include_dir})
