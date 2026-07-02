# 🎮 FailFirst - Complete Portable Windows Build Guide

## TL;DR - Fastest Method

1. **Copy this project to Windows**
2. **Install Qt 6.x** from https://qt.io (with MinGW)
3. **Run**: `.\scripts\build_windows.ps1`
4. **Package with Enigma Virtual Box** (optional - creates single .exe)

---

## What You Get

A **100% portable Windows app** that:
- ✅ Runs on any Windows 10/11 PC
- ✅ No DLL errors
- ✅ No installation required
- ✅ Saves data next to the .exe
- ✅ Works without g++/gcc (shows helpful message)
- ✅ Supports C, C++, Java, Python challenges (if compilers available)

---

## Build Options

### Option 1: Build on Windows (RECOMMENDED)

**Time: ~10 minutes**

#### Step 1: Install Qt
1. Download Qt installer: https://www.qt.io/download-qt-installer
2. Select: Qt 6.9 > Desktop > MinGW 64-bit
3. Install (~5 GB)

#### Step 2: Build
```powershell
cd C:\path\to\FailFirst
.\scripts\build_windows.ps1 -QtPath "C:\Qt\6.9.2\mingw_64"
```

#### Step 3: Test
- Navigate to `deploy\` folder
- Run `FailFirst.exe`
- Test login, challenges, etc.

#### Step 4: Create Single .exe (Optional)
1. Download Enigma Virtual Box: https://enigmaprotector.com
2. Input: `deploy\FailFirst.exe`
3. Add all DLLs and folders
4. Click Process → Get single `FailFirst_Portable.exe`!

---

### Option 2: Cross-Compile from Linux

**Time: 2-4 hours (first-time MXE build)**

```bash
# Install MXE
sudo apt install mxe-x86-64-w64-mingw32.static-qtbase

# Build
./scripts/cross_compile.sh
```

---

## Adding Portable Compilers

For C/C++ challenges to work without MinGW installed:

### Download TCC (Tiny C Compiler)
1. Go to: http://download.savannah.gnu.org/releases/tinycc/
2. Download: `tcc-0.9.27-win64-bin.zip`
3. Extract to: `portable\tcc\`

Your structure should look like:
```
FailFirst.exe
portable/
  └── tcc/
      ├── tcc.exe
      └── lib/
```

---

## Language Support Matrix

| Language | Portable Compiler | System Fallback |
|----------|-------------------|-----------------|
| C | TCC ✅ | gcc, clang |
| C++ | TCC (limited) | g++, clang++ |
| Python | Not portable | python3 |
| Java | Not portable | javac + java |

> **Note**: Java and Python require system installation. The app will show helpful messages if not found.

---

## Troubleshooting

### "Missing VCRUNTIME140.dll"
Download Visual C++ Redistributable:
https://aka.ms/vs/17/release/vc_redist.x64.exe

### App crashes immediately
Check that all Qt DLLs are in the same folder as FailFirst.exe.

### "Database error"
Make sure the folder where .exe resides is writable.

### Code doesn't compile
- For C/C++: Add TCC to `portable\tcc\` or install MinGW
- For Java: Install JDK 11+
- For Python: Install Python 3.x

---

## File Size Reference

| Component | Size |
|-----------|------|
| FailFirst.exe (packed) | ~25-40 MB |
| TCC portable | ~1.5 MB |
| **Total portable** | **~30-45 MB** |

---

## Quick Reference Commands

**Build (PowerShell)**:
```powershell
.\scripts\build_windows.ps1
```

**Build (CMD)**:
```cmd
scripts\build_windows.bat
```

**Deploy Qt**:
```powershell
windeployqt --release FailFirst.exe
```

---

## Need Help?

Check the main README.md for more info, or refer to the scripts in `scripts/` folder.
