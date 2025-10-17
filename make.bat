@echo off
setlocal

set BUILD_DIR=build
set GENERATOR=MinGW Makefiles

if not exist "%BUILD_DIR%" (
	echo Configuring project with generator %GENERATOR%...
	cmake -S . -B "%BUILD_DIR%" -G "%GENERATOR%"
	if errorlevel 1 exit /b %errorlevel%
)

rem Re-run configure to pick up CMake changes without wiping the build tree.
cmake -S . -B "%BUILD_DIR%"
if errorlevel 1 exit /b %errorlevel%

echo Building targets via mingw32-make...
mingw32-make -C "%BUILD_DIR%" %*
if errorlevel 1 exit /b %errorlevel%

if /I "%~1"=="clean" goto :EOF

echo Running tests...
ctest --test-dir "%BUILD_DIR%"
exit /b %errorlevel%
