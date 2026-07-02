@echo off
REM ================================================================
REM Download and extract TCC (Tiny C Compiler) for portable C/C++
REM ================================================================

echo.
echo ========================================
echo   TCC Portable Compiler Setup
echo ========================================
echo.

set TCC_URL=http://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win64-bin.zip
set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..
set PORTABLE_DIR=%PROJECT_DIR%\portable

echo Creating portable directory...
if not exist "%PORTABLE_DIR%\tcc" mkdir "%PORTABLE_DIR%\tcc"

echo.
echo Please download TCC manually:
echo   URL: %TCC_URL%
echo.
echo Then extract contents to:
echo   %PORTABLE_DIR%\tcc\
echo.
echo After extraction, you should have:
echo   %PORTABLE_DIR%\tcc\tcc.exe
echo.

REM Try to use PowerShell to download
echo Attempting automatic download with PowerShell...
powershell -Command "& {Invoke-WebRequest -Uri '%TCC_URL%' -OutFile '%PORTABLE_DIR%\tcc.zip'}" 2>nul
if exist "%PORTABLE_DIR%\tcc.zip" (
    echo Download successful! Extracting...
    powershell -Command "& {Expand-Archive -Path '%PORTABLE_DIR%\tcc.zip' -DestinationPath '%PORTABLE_DIR%\tcc_temp' -Force}"
    move "%PORTABLE_DIR%\tcc_temp\tcc\*" "%PORTABLE_DIR%\tcc\" >nul 2>&1
    rmdir /s /q "%PORTABLE_DIR%\tcc_temp" 2>nul
    del "%PORTABLE_DIR%\tcc.zip"
    echo.
    echo TCC installed successfully!
    echo Location: %PORTABLE_DIR%\tcc\tcc.exe
) else (
    echo Could not download automatically.
    echo Please download manually from the URL above.
)

echo.
pause
