# FailFirst Portable Compilers

This folder contains portable compilers for the FailFirst app to use
when system compilers are not available.

## TCC (Tiny C Compiler) - For C/C++

### Windows
1. Download from: http://download.savannah.gnu.org/releases/tinycc/tcc-0.9.27-win64-bin.zip
2. Extract contents here so you have: `tcc/tcc.exe`

### Linux
```bash
sudo apt install tcc
# Or place portable TCC in ./tcc/ folder
```

## Structure

```
portable/
└── tcc/
    ├── tcc.exe (or tcc on Linux)
    ├── lib/
    └── include/
```

The FailFirst app will automatically detect and use these compilers!
