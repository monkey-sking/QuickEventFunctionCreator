@echo off
setlocal enabledelayedexpansion

REM =================================================================================
REM
REM  CleanPluginPackages.bat
REM
REM  This script cleans the packaged plugin directories in the 'Release' folder.
REM  It removes large, non-essential files and folders to prepare the plugin
REM  for distribution on the Unreal Engine Marketplace.
REM
REM  What it does:
REM  1. Recursively deletes the entire 'Intermediate' folder.
REM  2. In the 'Binaries' folder, it deletes any subdirectory that is NOT
REM     'Win64' or 'Mac', removing unnecessary platform binaries.
REM
REM =================================================================================

REM --- Configuration ---
set "RELEASE_ROOT=%~dp0Release"
set "PLUGIN_NAME=QuickEventFunctionCreator"

echo --- Cleaning up packaged plugin folders for Marketplace release ---
echo Target Path: %RELEASE_ROOT%
echo.

if not exist "%RELEASE_ROOT%" (
    echo [ERROR] Release directory not found at '%RELEASE_ROOT%'.
    echo Please run this script from the project root directory.
    pause
    exit /b 1
)

for /d %%d in ("%RELEASE_ROOT%\UE_*") do (
    set "engine_version_name=%%~nxd"
    set "plugin_path=%%d\%PLUGIN_NAME%"

    if exist "!plugin_path!" (
        echo --- Processing [!engine_version_name!] ---

        REM 1. Delete the Intermediate folder
        set "intermediate_path=!plugin_path!\Intermediate"
        if exist "!intermediate_path!" (
            echo   - Removing 'Intermediate' folder...
            rmdir /s /q "!intermediate_path!"
            echo     Done.
        ) else (
            echo   - 'Intermediate' folder not found, skipping.
        )

        REM 2. Clean up the Binaries folder
        set "binaries_path=!plugin_path!\Binaries"
        if exist "!binaries_path!" (
            echo   - Cleaning 'Binaries' folder (keeping only Win64 & Mac)...
            for /d %%b in ("!binaries_path!\*") do (
                set "folder_name=%%~nb"
                if /i not "!folder_name!"=="Win64" if /i not "!folder_name!"=="Mac" (
                    echo     - Removing non-essential binary folder: '!folder_name!'
                    rmdir /s /q "%%b"
                )
            )
            echo     Done.
        ) else (
            echo   - 'Binaries' folder not found, skipping.
        )
        echo.

    )
)

echo ------------------------------------------------------------------
echo --- Cleanup Complete ---
echo All packaged plugins in the 'Release' folder have been cleaned.
echo ------------------------------------------------------------------
pause
endlocal
exit /b 0 