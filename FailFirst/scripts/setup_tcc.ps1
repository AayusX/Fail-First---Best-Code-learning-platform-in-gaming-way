# setup_tcc.ps1
# This script downloads a portable Tiny C Compiler (TCC) to the 'portable' folder

$portableDir = "c:\Users\DELL\Documents\FailFirst\portable"
$tccDir = Join-Path $portableDir "tcc"
$zipPath = Join-Path $portableDir "tcc.zip"
$downloadUrl = "http://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win64-bin.zip"

if (-not (Test-Path $portableDir)) {
    New-Item -ItemType Directory -Path $portableDir -Force
}

Write-Host "Downloading Tiny C Compiler..." -ForegroundColor Cyan
try {
    Invoke-WebRequest -Uri $downloadUrl -OutFile $zipPath -ErrorAction Stop
    Write-Host "Download complete." -ForegroundColor Green
} catch {
    Write-Host "Failed to download TCC. Please ensure you have internet access." -ForegroundColor Red
    exit 1
}

Write-Host "Extracting TCC..." -ForegroundColor Cyan
if (Test-Path $tccDir) {
    Remove-Item -Path $tccDir -Recurit -Force
}

Expand-Archive -Path $zipPath -DestinationPath $portableDir -Force

# Rename the weirdly named git snapshot folder to just 'tcc'
$extractedFolder = Get-ChildItem -Path $portableDir -Directory | Where-Object { $_.Name -like "tinycc-*" } | Select-Object -First 1
if ($extractedFolder) {
    Rename-Item -Path $extractedFolder.FullName -NewName "tcc"
}

Remove-Item -Path $zipPath -Force

Write-Host "TCC setup complete in $tccDir" -ForegroundColor Green
Write-Host "The FailFirst app will now automatically detect and use TCC as a fallback." -ForegroundColor Green
