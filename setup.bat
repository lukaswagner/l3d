@echo off

set CMAKE_ARGS=-G "Visual Studio 16 2019" -A x64
set BUILD_ARGS=--config Release

if not exist dependencies mkdir dependencies
cd dependencies

if not exist src mkdir src

call :build logger https://github.com/lukaswagner/logger.git
call :build glfw https://github.com/glfw/glfw

rmdir /s /q src

goto :eof

:build
if exist %1 (
    echo %1 already exists
    exit /b
)
call git clone %2 src/%1
mkdir %1
call cmake %CMAKE_ARGS% -S src/%1 -B src/%1 -DCMAKE_INSTALL_PREFIX=./%1
call cmake --build src/%1 --target install %BUILD_ARGS%
exit /b
