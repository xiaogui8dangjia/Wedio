@echo off

REM Build project batch script
REM Author: Auto-Generated
REM Date: 2026-04-22

cd /d "%~dp0"

echo Building project...
echo ======================

REM Create build directory if not exists
if not exist "build" (
    mkdir "build"
)

REM Configure project with CMake
echo Configuring project...
cmake -S . -B build

if %errorlevel% neq 0 (
    echo Configuration failed!
    pause
    exit /b 1
)

REM Build project
echo Building project...
cmake --build build --config Release

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo ======================
echo Build completed!
echo Executable: build\Release\Medio.exe

pause
