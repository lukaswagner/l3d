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
set(glbinding_dir ${dep_dir}/glbinding)
set(glm_dir ${dep_dir}/glm)
set(cli11_dir ${dep_dir}/cli11)

set(CMAKE_PREFIX_PATH "${glfw_dir}" "${glbinding_dir}")

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

find_package(glbinding)
if(NOT glbinding_FOUND)
    message(FATAL_ERROR "glbinding not found")
endif()
include_directories(${glbinding_dir}/include)

include_directories(${cli11_dir}/include)

# sources

set(include_dir include)
set(source_dir src)

set(shared_include_dir ${include_dir}/shared)
set(shared_source_dir ${source_dir}/shared)
set(shared_sources
    ${shared_source_dir}/logUtil.cpp
    ${shared_source_dir}/shader.cpp
    ${shared_source_dir}/timer.cpp
    ${shared_source_dir}/uniforms.cpp
    ${shared_source_dir}/dataSource/time.cpp)

set(l2d_include_dir ${include_dir}/l2d)
set(l2d_source_dir ${source_dir}/l2d)
set(l2d_sources
    ${l2d_source_dir}/l2d.cpp
    ${l2d_source_dir}/effectPass.cpp
    ${l2d_source_dir}/fadePass.cpp
    ${l2d_source_dir}/program.cpp
    ${l2d_source_dir}/options.cpp
    ${l2d_source_dir}/renderer.cpp
    ${l2d_source_dir}/viewer.cpp)

set(l3d_source_dir ${source_dir}/l3d)
set(l3d_sources ${l3d_source_dir}/l3d.cpp)

# embedded data

set(data_dir ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(generated_dir ${CMAKE_CURRENT_BINARY_DIR}/generated)

# https://stackoverflow.com/a/47801116
function(make_includable file)
    set(input_file ${data_dir}/${file})
    set(output_file ${generated_dir}/${file})
    file(READ ${input_file} content)
    set(delim "EMBEDDED_DATA")
    string(REGEX REPLACE ^[^/]*/ "" varName ${file})
    string(REGEX REPLACE [^A-Za-z0-9_] _ varName ${varName})
    set(content "constexpr char* ${varName} = R\"${delim}(\n${content})${delim}\";")
    file(WRITE ${output_file} "${content}")
endfunction(make_includable)

file(GLOB_RECURSE data_files RELATIVE ${data_dir} ${data_dir}/*)

foreach(file ${data_files})
    make_includable(${file})
endforeach()

# executables

add_executable(l2d ${l2d_sources} ${shared_sources})
target_link_libraries(l2d PRIVATE glfw ${logger} glbinding::glbinding glbinding::glbinding-aux)
target_include_directories(l2d
    PRIVATE ${l2d_include_dir} ${shared_include_dir} ${glm_dir} ${generated_dir}/l2d)

add_executable(l3d ${l3d_sources})
target_link_libraries(l3d PRIVATE glfw ${logger} glbinding::glbinding)
target_include_directories(l3d
    PRIVATE ${shared_include_dir} ${glm_dir})

# dlls for runtime
if(WIN32)
    add_custom_command(TARGET l2d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${glbinding_dir}/glbinding.dll" $<TARGET_FILE_DIR:l2d>)
    add_custom_command(TARGET l2d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${glbinding_dir}/glbinding-aux.dll" $<TARGET_FILE_DIR:l2d>)
    add_custom_command(TARGET l3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${glbinding_dir}/glbinding.dll" $<TARGET_FILE_DIR:l3d>)
endif()
