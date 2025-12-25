@echo off
setlocal

set "BUILD_DIR=build"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    set "VS_CMAKE_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    if exist "%VS_CMAKE_PATH%" (
        set "CMAKE_CMD=%VS_CMAKE_PATH%"
    ) else (
        echo CMake not found!
        pause
        exit /b 1
    )
) else (
    set "CMAKE_CMD=cmake"
)

cd "%BUILD_DIR%"

"%CMAKE_CMD%" .. -G "Visual Studio 17 2022" -A x64

pause
endlocal