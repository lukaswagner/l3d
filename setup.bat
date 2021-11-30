@echo off

set CMAKE_ARGS=-G "Visual Studio 16 2019" -A x64
set BUILD_ARGS=--config Release --parallel 8
set DEPENCENCY_DIR=dependencies

if not exist %DEPENCENCY_DIR% mkdir %DEPENCENCY_DIR%
cd %DEPENCENCY_DIR%

if not exist src mkdir src

call :clone logger https://github.com/lukaswagner/logger.git
call :build logger
call :clone glfw https://github.com/glfw/glfw.git 3.3.5
call :build glfw
call :clone glbinding https://github.com/cginternals/glbinding.git v3.1.0
call :build glbinding
call :clone glm https://github.com/g-truc/glm.git 0.9.9.8
call :copy src\glm\glm glm\glm
call :clone cli11 https://github.com/CLIUtils/CLI11.git v2.1.2
call :build cli11 "-DCLI11_BUILD_DOCS=OFF -DCLI11_BUILD_TESTS=OFF -DCLI11_BUILD_EXAMPLES=OFF"

rmdir /s /q src

goto :eof

:check_dir
if exist %2 (
    echo %1: %2 already exists
    exit /b 1
)
exit /b 0

:clone
call :check_dir clone %1
if errorlevel 1 exit /b
setlocal
set args=--depth 1
if [%3] neq [] set args=%args% --branch %3 --config advice.detachedHead=false
call git clone %args% %2 src/%1
endlocal
exit /b

:build
call :check_dir build %1
if errorlevel 1 exit /b
mkdir %1
call cmake %CMAKE_ARGS% -S src/%1 -B src/%1 -DCMAKE_INSTALL_PREFIX=./%1 %~2
call cmake --build src/%1 --target install %BUILD_ARGS%
exit /b

:copy
call :check_dir copy %2
if errorlevel 1 exit /b
xcopy %1 %2 /s /i /q /y > nul
exit /b
