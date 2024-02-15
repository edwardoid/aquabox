pushd %~dp0
set SCRIPT_DIR=%CD%
popd


call %SCRIPT_DIR%/set_env.bat %1%

if "%1%"=="x86_64" (
    set DOCKER_EXTRA_ARGS_INT=-p 1213:1213 -p 1214:1214 -p 5555:5555 -p 5556:5556 -p 8974:8974 -p 2159:2159 -p 1800:1800 -p 1883:1883 --hostname %1%
)

if "%1"=="armv6" (
    set DOCKER_EXTRA_ARGS_INT=-v %SOURCES_DIR%\Docker\conan_armv6:/root/.conan/profiles/armv6 -e LD_FLAGS='-lpthread -lm'
)

if "%1"=="armv7" (
    set DOCKER_EXTRA_ARGS_INT=-v %SOURCES_DIR%\Docker\conan_armv7:/root/.conan/profiles/armv7
)

if defined DOCKER_EXTRA_ARGS (
    set DOCKER_EXTRA_ARGS_INT=%DOCKER_EXTRA_ARGS% %DOCKER_EXTRA_ARGS_INT%
)

echo "Running conan with args: %DOCKER_EXTRA_ARGS_INT%"

docker run --privileged ^
-v %SOURCES_DIR%:/sources ^
-v %BUILD_DIR%:/build %DOCKER_EXTRA_ARGS_INT% ^
-e BUILD_PLATFORM=%1% ^
-e CONAN_CPU_COUNT=16 ^
-it --rm %IMAGE%:latest ^
/bin/bash

