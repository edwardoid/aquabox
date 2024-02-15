
pushd %~dp0
set SCRIPT_DIR=%CD%
popd

echo %SCRIPT_DIR%

#CPU_COUNT="grep -c ^processor /proc/cpuinfo"

set SOURCES_DIR=%SCRIPT_DIR%\..
set BUILD_DIR=%SOURCES_DIR%\build\%1%
set LOG_FILE=%SOURCES_DIR%\build\%1%.log

echo "Sources located in %SOURCES_DIR%"

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

echo "Build will be done in %BUILD_DIR%"
echo "Logs will be %LOG_FILE%"


set IMAGE=conan_base

docker image build -t %IMAGE% - < %SCRIPT_DIR%\Dockerfile.base