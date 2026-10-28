@echo off
setlocal enabledelayedexpansion

REM --- Configuration ---
set "PLUGIN_RELEASE_PATH=%~dp0Release"
set "PLUGIN_NAME=QuickEventFunctionCreator"
set "MODULE_NAME=SmartBlueprintFunctionCreator"

REM --- Main Script Logic ---
echo --- Checking for Windows (Win64) binaries in Release folder ---
echo Release Path: %PLUGIN_RELEASE_PATH%
echo.

if not exist "%PLUGIN_RELEASE_PATH%" (
    echo [ERROR] Release directory not found at '%PLUGIN_RELEASE_PATH%'.
    exit /b 1
)

set "COMPLETED_VERSIONS="
set "MISSING_VERSIONS="

for /d %%d in ("%PLUGIN_RELEASE_PATH%\UE_*") do (
    set "VERSION_NAME=%%~nd"
    set "WIN64_BINARIES_PATH=%%d\%PLUGIN_NAME%\Binaries\Win64"
    
    set "DLL_FOUND=false"
    if exist "!WIN64_BINARIES_PATH!\" (
        REM Check for the core DLL file for either UE4 or UE5
        if exist "!WIN64_BINARIES_PATH!\UE4Editor-!MODULE_NAME!*.dll" set "DLL_FOUND=true"
        if exist "!WIN64_BINARIES_PATH!\UnrealEditor-!MODULE_NAME!*.dll" set "DLL_FOUND=true"
    )
    
    if !DLL_FOUND! == true (
        echo [ OK ] !VERSION_NAME!: Found core plugin DLL.
        set "COMPLETED_VERSIONS=!COMPLETED_VERSIONS! !VERSION_NAME!"
    ) else (
        echo [FAIL] !VERSION_NAME!: Missing core plugin DLL.
        set "MISSING_VERSIONS=!MISSING_VERSIONS! !VERSION_NAME!"
    )
)

REM --- Summary Report ---
echo.
echo --- Summary ---
if defined COMPLETED_VERSIONS (
    echo ✅ Versions with Windows binaries:
    for %%V in (%COMPLETED_VERSIONS%) do (
        echo    - %%V
    )
) else (
    echo ✅ No versions currently have Windows binaries.
)

echo.

if defined MISSING_VERSIONS (
    echo ❌ Versions MISSING Windows binaries:
    for %%V in (%MISSING_VERSIONS%) do (
        echo    - %%V
    )
) else (
    echo 🎉 All found versions have Windows binaries!
)

echo ------------------------------------------------------------------

endlocal
exit /b 0 