cmake_minimum_required(VERSION 3.19)

# edit the build settings here
if(WIN32)
    set(configure_args -G "Visual Studio 16 2019" -A x64)
    set(build_args --config RelWithDebInfo --parallel 8)
else()
    set(configure_args -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo)
    set(build_args --parallel 8)
endif()

set(dependency_dir "dependencies")
set(src_dir "${dependency_dir}/src")

function(git dir url)
    set(source "${src_dir}/${dir}")
    set(target "${dependency_dir}/${dir}")
    if(EXISTS ${target})
        message(NOTICE "${dir} already exists")
        return()
    endif()
    set(args --depth 1)
    if(ARGC GREATER 2)
        set(args ${args} --branch ${ARGV2} --config advice.detachedHead=false)
    endif()
    execute_process(
        COMMAND git clone ${args} ${url} ${source}
        COMMAND_ERROR_IS_FATAL ANY)
endfunction()

function(build dir)
    set(source "${src_dir}/${dir}")
    set(target "${dependency_dir}/${dir}")
    if(EXISTS ${target})
        message(NOTICE "${dir} already exists")
        return()
    endif()
    execute_process(
        COMMAND cmake ${configure_args}
        -S ${source} -B ${source}
        -DCMAKE_INSTALL_PREFIX=${target} ${ARGV1}
        COMMAND_ERROR_IS_FATAL ANY)
    execute_process(
        COMMAND cmake --build ${source} --target install
        ${build_args}
        COMMAND_ERROR_IS_FATAL ANY)
endfunction()


git(logger https://github.com/lukaswagner/logger.git)
build(logger)

git(glfw https://github.com/glfw/glfw.git 3.3.5)
build(glfw)

git(glbinding https://github.com/cginternals/glbinding.git v3.1.0)
build(glbinding)

git(glm https://github.com/g-truc/glm.git 0.9.9.8)
file(COPY ${src_dir}/glm/glm DESTINATION ${dependency_dir}/glm)

git(cli11 https://github.com/CLIUtils/CLI11.git v2.1.2)
build(cli11 "-DCLI11_BUILD_DOCS=OFF;-DCLI11_BUILD_TESTS=OFF;-DCLI11_BUILD_EXAMPLES=OFF")

file(REMOVE_RECURSE ${src_dir})
