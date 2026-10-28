@echo off
setlocal enabledelayedexpansion

:: ==============================================================================
:: QuickEventFunctionCreator - One-Click Build Script for All UE Versions (Windows)
:: ==============================================================================
::
:: This script automates packaging the plugin for all supported Unreal Engine
:: versions on Windows. It uses an isolated build environment for each version
:: to ensure a clean and reliable build.
::
:: Usage:
:: 1. Make sure the configuration variables below are correct.
:: 2. Open Command Prompt (cmd.exe) or PowerShell.
:: 3. Navigate to the project root directory.
::
:: To build for all supported versions:
::   .\BuildAllVersions.bat
::
:: To build for a single, specific version:
::   .\BuildAllVersions.bat 5.6
::
:: NOTE: Ensure that the required Visual Studio version for each engine
::       version is installed correctly.
::
:: ==============================================================================

:: --- Configuration ---

:: Space-separated list of Unreal Engine versions to build for.
set "SUPPORTED_VERSIONS=4.26 4.27 5.0 5.1 5.2 5.3 5.4 5.5 5.6"

:: Base path where your Unreal Engine versions are installed.
set "UE_INSTALL_BASE=C:\Program Files\Epic Games"

:: Get the project root directory (where the script is located).
set "PROJECT_ROOT=%~dp0"
:: Remove trailing backslash if it exists.
if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

set "PLUGIN_FILE_NAME=QuickEventFunctionCreator.uplugin"
set "PLUGIN_NAME=QuickEventFunctionCreator"
set "RELEASE_DIR=%PROJECT_ROOT%\Release"


:: --- Version Selection Logic ---
set "VERSIONS_TO_BUILD="
if "%1"=="" (
    echo [INFO] No specific version requested. Building for all supported versions.
    set "VERSIONS_TO_BUILD=%SUPPORTED_VERSIONS%"
) else (
    echo [INFO] Requested to build for a specific version: %1
    set "VERSIONS_TO_BUILD=%1"
)

:: --- Pre-Build Checks and Cleanup ---
echo [INFO] Starting plugin build process for: %VERSIONS_TO_BUILD%
if not exist "%PROJECT_ROOT%\%PLUGIN_FILE_NAME%" (
    echo [ERROR] Plugin file not found at "%PROJECT_ROOT%\%PLUGIN_FILE_NAME%".
    exit /b 1
)

:: Clean the entire release directory if building all versions to ensure a clean slate.
if "%1"=="" (
    echo [INFO] Performing full pre-build cleanup by removing the entire Release directory...
    if exist "%RELEASE_DIR%" (
        rmdir /s /q "%RELEASE_DIR%"
    )
)
mkdir "%RELEASE_DIR%"


:: ========================= MAIN BUILD LOOP =========================
for %%v in (%VERSIONS_TO_BUILD%) do (
    echo.
    echo [INFO] ========== Building for Unreal Engine %%v ==========

    :: 1. Create a temporary, isolated build environment
    set "BUILD_DIR=%TEMP%\ue_build_%RANDOM%"
    mkdir "%BUILD_DIR%"
    echo [INFO] Created isolated build directory: "!BUILD_DIR!"

    :: 2. Copy only the necessary plugin files to the isolated directory
    echo [INFO] Copying latest plugin source to isolated directory...
    copy "%PROJECT_ROOT%\%PLUGIN_FILE_NAME%" "%BUILD_DIR%\" > NUL
    xcopy /s /e /i /q /y "%PROJECT_ROOT%\Source" "%BUILD_DIR%\Source\"
    if exist "%PROJECT_ROOT%\Resources" xcopy /s /e /i /q /y "%PROJECT_ROOT%\Resources" "%BUILD_DIR%\Resources\"
    if exist "%PROJECT_ROOT%\Config" xcopy /s /e /i /q /y "%PROJECT_ROOT%\Config" "%BUILD_DIR%\Config\"

    :: 3. Define paths for the current version
    set "ENGINE_PATH=%UE_INSTALL_BASE%\UE_%%v"
    set "PLUGIN_UPLUGIN_PATH_ISOLATED=%BUILD_DIR%\%PLUGIN_FILE_NAME%"
    set "PACKAGE_OUTPUT_PATH=%BUILD_DIR%\Output"

    if not exist "!ENGINE_PATH!" (
        echo [WARNING] Engine path not found for version %%v at "!ENGINE_PATH!". Skipping.
    ) else (
        :: 4. Run the build in the isolated directory
        echo [INFO] Running AutomationTool... This may take a few minutes.
        set "UAT_SCRIPT_PATH=!ENGINE_PATH!\Engine\Build\BatchFiles\RunUAT.bat"
        
        call "!UAT_SCRIPT_PATH!" BuildPlugin -Plugin="!PLUGIN_UPLUGIN_PATH_ISOLATED!" -Package="!PACKAGE_OUTPUT_PATH!" -rocket
        
        if !errorlevel! neq 0 (
            echo [ERROR] AutomationTool failed for UE %%v.
            rmdir /s /q "!BUILD_DIR!"
            exit /b 1
        )

        :: 5. Prepare final destination and move the packaged plugin
        set "FINAL_PLUGIN_DIR=%RELEASE_DIR%\UE_%%v\%PLUGIN_NAME%"
        echo [INFO] Packaging complete. Moving plugin to "!FINAL_PLUGIN_DIR!"
        mkdir "!FINAL_PLUGIN_DIR!" > NUL
        
        robocopy "!PACKAGE_OUTPUT_PATH!" "!FINAL_PLUGIN_DIR!" /e /move > NUL

        :: 6. Clean up the now-empty temporary build directory
        echo [INFO] Cleaning up temporary build directory...
        rmdir /s /q "!BUILD_DIR!"
        
        echo [SUCCESS] Successfully packaged plugin for UE %%v.
    )
)

:: --- Finalization ---
echo.
echo [INFO] ================================================================
echo [SUCCESS] Build process complete! All plugins are in "%RELEASE_DIR%".
echo.

endlocal
exit /b 0 