@echo off
title NeoVerse AI City Engine - Build Tool
color 0A

echo.
echo  +==========================================+
echo  ^|   NeoVerse AI City Engine - Build Tool   ^|
echo  +==========================================+
echo.

REM -----------------------------------------------------------------------
REM Step 1: Try to find g++ from common MinGW/MSYS2 install locations
REM -----------------------------------------------------------------------
set COMPILER=
set FOUND=0

REM Check if g++ is already on the system PATH
where g++ >nul 2>&1
if %errorlevel% == 0 (
    set COMPILER=g++
    set FOUND=1
    echo  [OK] Found g++ on system PATH.
    goto :COMPILE
)

REM Check MSYS2 UCRT64 (most modern MinGW install)
if exist "C:\msys64\ucrt64\bin\g++.exe" (
    set COMPILER=C:\msys64\ucrt64\bin\g++.exe
    set FOUND=1
    echo  [OK] Found g++ in MSYS2 UCRT64.
    goto :COMPILE
)

REM Check MSYS2 MINGW64
if exist "C:\msys64\mingw64\bin\g++.exe" (
    set COMPILER=C:\msys64\mingw64\bin\g++.exe
    set FOUND=1
    echo  [OK] Found g++ in MSYS2 MinGW64.
    goto :COMPILE
)

REM Check classic MinGW install
if exist "C:\MinGW\bin\g++.exe" (
    set COMPILER=C:\MinGW\bin\g++.exe
    set FOUND=1
    echo  [OK] Found g++ in C:\MinGW.
    goto :COMPILE
)

REM Check Dev-C++ bundled MinGW (common university install)
if exist "C:\Dev-Cpp\MinGW64\bin\g++.exe" (
    set COMPILER=C:\Dev-Cpp\MinGW64\bin\g++.exe
    set FOUND=1
    echo  [OK] Found g++ bundled with Dev-C++.
    goto :COMPILE
)

if exist "C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe" (
    set "COMPILER=C:\Program Files (x86)\Dev-Cpp\MinGW64\bin\g++.exe"
    set FOUND=1
    echo  [OK] Found g++ bundled with Dev-C++ (x86).
    goto :COMPILE
)

REM Check Code::Blocks bundled MinGW
if exist "C:\Program Files\CodeBlocks\MinGW\bin\g++.exe" (
    set "COMPILER=C:\Program Files\CodeBlocks\MinGW\bin\g++.exe"
    set FOUND=1
    echo  [OK] Found g++ bundled with Code::Blocks.
    goto :COMPILE
)

if exist "C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe" (
    set "COMPILER=C:\Program Files (x86)\CodeBlocks\MinGW\bin\g++.exe"
    set FOUND=1
    echo  [OK] Found g++ bundled with Code::Blocks (x86).
    goto :COMPILE
)

REM -----------------------------------------------------------------------
REM No compiler found - show helpful instructions
REM -----------------------------------------------------------------------
echo.
echo  [ERROR] Could not find g++ on this machine.
echo.
echo  To fix this, install one of the following:
echo.
echo  Option 1 - MSYS2 (Recommended):
echo    1. Go to https://www.msys2.org
echo    2. Download and install MSYS2
echo    3. Open MSYS2 terminal and run:
echo       pacman -S mingw-w64-ucrt-x86_64-gcc
echo    4. Re-run this compile.bat
echo.
echo  Option 2 - MinGW:
echo    1. Go to https://winlibs.com
echo    2. Download the latest GCC for Windows
echo    3. Extract to C:\MinGW
echo    4. Re-run this compile.bat
echo.
echo  Option 3 - Use Dev-C++ or Code::Blocks IDE directly:
echo    See the README.txt file for IDE-specific instructions.
echo.
pause
exit /b 1

REM -----------------------------------------------------------------------
REM Step 2: Compile the project
REM -----------------------------------------------------------------------
:COMPILE
echo.
echo  Compiling NeoVerse...
echo  Files: main.cpp + CityEngine.cpp
echo.

REM Try C++17 first, fall back to C++14 then C++11
REM Build to a temp name first to avoid file-lock if old exe is still running
"%COMPILER%" -std=c++17 -Wall -o NeoVerseEngine_build.exe main.cpp CityEngine.cpp 2>compile_errors.txt
if %errorlevel% == 0 goto :SUCCESS

echo  [WARN] C++17 failed, trying C++14...
"%COMPILER%" -std=c++14 -Wall -o NeoVerseEngine_build.exe main.cpp CityEngine.cpp 2>compile_errors.txt
if %errorlevel% == 0 goto :SUCCESS

echo  [WARN] C++14 failed, trying C++11...
"%COMPILER%" -std=c++11 -Wall -o NeoVerseEngine_build.exe main.cpp CityEngine.cpp 2>compile_errors.txt
if %errorlevel% == 0 goto :SUCCESS

REM All standards failed - show the errors
echo.
echo  [ERROR] Compilation failed. Error details:
echo  -----------------------------------------------
type compile_errors.txt
echo  -----------------------------------------------
echo.
echo  Please check compile_errors.txt for details.
echo.
pause
exit /b 1

REM -----------------------------------------------------------------------
REM Step 3: Success - run the program
REM -----------------------------------------------------------------------
:SUCCESS
del compile_errors.txt >nul 2>&1
if exist NeoVerseEngine.exe del NeoVerseEngine.exe >nul 2>&1
rename NeoVerseEngine_build.exe NeoVerseEngine.exe
echo  +------------------------------------------+
echo  ^|   Build Successful! Launching NeoVerse... ^|
echo  +------------------------------------------+
echo.
echo  LOGIN CREDENTIALS:
echo    Username: admin    Password: admin123  (High clearance)
echo    Username: tech     Password: tech123   (Medium clearance)
echo    Username: viewer   Password: view123   (Low clearance)
echo.
pause
echo.
NeoVerseEngine.exe
