pushd %~dp0
set SCRIPT_DIR=%CD%
popd

call %SCRIPT_DIR%\conan_run.bat x86_64
