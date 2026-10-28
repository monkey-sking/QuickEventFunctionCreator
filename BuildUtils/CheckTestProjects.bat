@echo off
setlocal enabledelayedexpansion

REM --- Configuration ---
set "TEST_PROJECTS_ROOT=%~dp0"
set "PLUGIN_NAME=QuickEventFunctionCreator"
set "MODULE_NAME=SmartBlueprintFunctionCreator"

REM --- Main Script Logic ---
echo --- Checking for compiled plugins in Test Projects ---
echo Root Path: %TEST_PROJECTS_ROOT%
echo.

set "COMPLETED_PROJECTS="
set "MISSING_PROJECTS="

for /d %%d in ("%TEST_PROJECTS_ROOT%PluginTest_*") do (
    set "project_name=%%~nxd"
    set "dll_path_ue4=%%d\Plugins\%PLUGIN_NAME%\Binaries\Win64\UE4Editor-%MODULE_NAME%.dll"
    set "dll_path_ue5=%%d\Plugins\%PLUGIN_NAME%\Binaries\Win64\UnrealEditor-%MODULE_NAME%.dll"

    if exist "!dll_path_ue4!" (
        echo [ OK ] !project_name!: Found compiled plugin DLL.
        set "COMPLETED_PROJECTS=!COMPLETED_PROJECTS! "!project_name!""
    ) else if exist "!dll_path_ue5!" (
        echo [ OK ] !project_name!: Found compiled plugin DLL.
        set "COMPLETED_PROJECTS=!COMPLETED_PROJECTS! "!project_name!""
    ) else (
        echo [FAIL] !project_name!: Missing compiled plugin DLL.
        set "MISSING_PROJECTS=!MISSING_PROJECTS! "!project_name!""
    )
)

REM --- Summary Report ---
echo.
echo --- Summary ---
if defined COMPLETED_PROJECTS (
    echo ✅ Projects with compiled plugin:
    for %%V in (%COMPLETED_PROJECTS%) do (
        echo    - %%~V
    )
) else (
    echo ✅ No test projects currently have a compiled plugin.
)

echo.

if defined MISSING_PROJECTS (
    echo ❌ Projects MISSING a compiled plugin:
    for %%V in (%MISSING_PROJECTS%) do (
        echo    - %%~V
    )
) else (
    echo 🎉 All test projects have a compiled plugin!
)

echo ------------------------------------------------------------------

endlocal
exit /b 0 