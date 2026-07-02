@echo off
REM ================================================================
REM FailFirst Windows Build Script
REM Run this on Windows with Qt 6.x and MinGW installed
REM ================================================================

echo.
echo ========================================
echo   FailFirst Windows Build Script
echo ========================================
echo.

REM Configuration - Adjust these paths as needed
set QT_PATH=C:\Qt\6.9.2\mingw_64
set BUILD_DIR=build-windows
set DEPLOY_DIR=deploy

REM Check Qt
if not exist "%QT_PATH%\bin\qmake6.exe" (
    echo [ERROR] Qt not found at %QT_PATH%
    echo Please install Qt 6.x with MinGW and update QT_PATH in this script.
    exit /b 1
)

echo [1/5] Setting up environment...
set PATH=%QT_PATH%\bin;%PATH%

echo [2/5] Creating build directory...
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo [3/5] Running CMake...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_PATH%" ..
if errorlevel 1 (
    echo [ERROR] CMake configuration failed!
    exit /b 1
)

echo [4/5] Building...
mingw32-make -j%NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    echo [ERROR] Build failed!
    exit /b 1
)

echo [5/5] Deploying Qt dependencies...
cd ..
if exist %DEPLOY_DIR% rmdir /s /q %DEPLOY_DIR%
mkdir %DEPLOY_DIR%
copy %BUILD_DIR%\FailFirst.exe %DEPLOY_DIR%\
cd %DEPLOY_DIR%
windeployqt --release --no-translations FailFirst.exe

echo.
echo ========================================
echo   BUILD COMPLETE!
echo ========================================
echo.
echo Output: %CD%\FailFirst.exe
echo.
echo Next steps:
echo   1. Test the app in the deploy folder
echo   2. Use Enigma Virtual Box to create single .exe
echo   3. Optionally add portable\tcc\ folder for C/C++ support
echo.

pause
