# ================================================================
# FailFirst Windows Build Script (PowerShell)
# Run this on Windows with Qt 6.x and MinGW installed
# ================================================================

param(
    [string]$QtPath = "C:\Qt\6.9.2\mingw_64",
    [string]$BuildType = "Release"
)

Write-Host ""
Write-Host "========================================"
Write-Host "   FailFirst Windows Build Script"
Write-Host "========================================"
Write-Host ""

$ErrorActionPreference = "Stop"
$ProjectDir = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectDir "build-windows"
$DeployDir = Join-Path $ProjectDir "deploy"

# Check Qt installation
if (-not (Test-Path "$QtPath\bin\qmake6.exe")) {
    Write-Host "[ERROR] Qt not found at $QtPath" -ForegroundColor Red
    Write-Host "Please install Qt 6.x with MinGW and provide correct path:" -ForegroundColor Yellow
    Write-Host "  .\build_windows.ps1 -QtPath 'C:\Qt\6.x.x\mingw_64'" -ForegroundColor Yellow
    exit 1
}

# Update PATH
$env:PATH = "$QtPath\bin;$env:PATH"

# Step 1: Create build directory
Write-Host "[1/6] Creating build directory..." -ForegroundColor Cyan
if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
New-Item -ItemType Directory -Path $BuildDir | Out-Null
Set-Location $BuildDir

# Step 2: Run CMake
Write-Host "[2/6] Running CMake..." -ForegroundColor Cyan
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=$BuildType -DCMAKE_PREFIX_PATH="$QtPath" "$ProjectDir"
if ($LASTEXITCODE -ne 0) { throw "CMake failed" }

# Step 3: Build
Write-Host "[3/6] Building (this may take a few minutes)..." -ForegroundColor Cyan
$cores = [Environment]::ProcessorCount
mingw32-make -j$cores
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

# Step 4: Create deploy directory
Write-Host "[4/6] Creating deployment package..." -ForegroundColor Cyan
if (Test-Path $DeployDir) { Remove-Item -Recurse -Force $DeployDir }
New-Item -ItemType Directory -Path $DeployDir | Out-Null
Copy-Item "$BuildDir\FailFirst.exe" $DeployDir

# Step 5: Deploy Qt dependencies
Write-Host "[5/6] Deploying Qt dependencies..." -ForegroundColor Cyan
Set-Location $DeployDir
windeployqt --release --no-translations FailFirst.exe

# Step 6: Create portable folder structure
Write-Host "[6/6] Creating portable folder structure..." -ForegroundColor Cyan
New-Item -ItemType Directory -Path "$DeployDir\portable\tcc" -Force | Out-Null

Write-Host ""
Write-Host "========================================"
Write-Host "   BUILD COMPLETE!" -ForegroundColor Green
Write-Host "========================================"
Write-Host ""
Write-Host "Output: $DeployDir\FailFirst.exe" -ForegroundColor Yellow
Write-Host ""
Write-Host "File size: $([math]::Round((Get-Item FailFirst.exe).Length / 1MB, 2)) MB"
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Download TCC from https://bellard.org/tcc/ and extract to portable\tcc\"
Write-Host "  2. Use Enigma Virtual Box to create single portable .exe"
Write-Host "  3. Test on a clean Windows machine"
Write-Host ""

Set-Location $ProjectDir
