@echo off
setlocal

cd /d "%~dp0.."

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

if not exist build mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

pause
