#!/bin/bash

#
# CheckTestProjectBinaries.sh
#
# This script scans through all Unreal Engine test project directories located in a specified
# root folder. For each project, it verifies the existence of compiled plugin binaries for
# both the Win64 and Mac platforms. It then generates a summary report of the findings.
#

# --- Configuration ---
TEST_PROJECTS_ROOT="/Volumes/SSD/UnrealProjects/PluginTests"
PLUGIN_NAME="QuickEventFunctionCreator"
MODULE_NAME="SmartBlueprintFunctionCreator"

# --- Colors for output ---
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# --- Main Script Logic ---
echo "--- Checking for compiled plugin binaries in Test Projects ---"
echo "Root Path: $TEST_PROJECTS_ROOT"
echo

# Lists for summary
declare -a all_ok_projects
declare -a win_only_projects
declare -a mac_only_projects
declare -a fail_projects

# Check if the root directory exists
if [ ! -d "$TEST_PROJECTS_ROOT" ]; then
    echo -e "${RED}Error: Test projects directory not found at '$TEST_PROJECTS_ROOT'${NC}"
    exit 1
fi

for project_path in "$TEST_PROJECTS_ROOT"/PluginTest_*; do
    if [ -d "$project_path" ]; then
        project_name=$(basename "$project_path")
        
        # Paths to binaries
        win_bin_path_ue4="$project_path/Plugins/$PLUGIN_NAME/Binaries/Win64/UE4Editor-$MODULE_NAME.dll"
        win_bin_path_ue5="$project_path/Plugins/$PLUGIN_NAME/Binaries/Win64/UnrealEditor-$MODULE_NAME.dll"
        mac_bin_path_ue4="$project_path/Plugins/$PLUGIN_NAME/Binaries/Mac/UE4Editor-$MODULE_NAME.dylib"
        mac_bin_path_ue5="$project_path/Plugins/$PLUGIN_NAME/Binaries/Mac/UnrealEditor-$MODULE_NAME.dylib"

        win_found=false
        mac_found=false

        if [ -f "$win_bin_path_ue4" ] || [ -f "$win_bin_path_ue5" ]; then
            win_found=true
        fi

        if [ -f "$mac_bin_path_ue4" ] || [ -f "$mac_bin_path_ue5" ]; then
            mac_found=true
        fi

        # Report status
        if $win_found && $mac_found; then
            echo -e "[  ${GREEN}OK${NC}  ] $project_name: Found binaries for Win64 and Mac."
            all_ok_projects+=("$project_name")
        elif $win_found; then
            echo -e "[${YELLOW}WIN ONLY${NC}] $project_name: Found binaries for Win64 ONLY."
            win_only_projects+=("$project_name")
        elif $mac_found; then
            echo -e "[${YELLOW}MAC ONLY${NC}] $project_name: Found binaries for Mac ONLY."
            mac_only_projects+=("$project_name")
        else
            echo -e "[  ${RED}FAIL${NC}  ] $project_name: Missing all compiled binaries."
            fail_projects+=("$project_name")
        fi
    fi
done

# --- Summary Report ---
echo
echo "--- Summary ---"

if [ ${#all_ok_projects[@]} -gt 0 ]; then
    echo -e "✅ ${GREEN}Projects with both Win64 & Mac binaries:${NC}"
    for proj in "${all_ok_projects[@]}"; do
        echo "   - $proj"
    done
fi

if [ ${#win_only_projects[@]} -gt 0 ]; then
    echo -e "🔵 ${YELLOW}Projects with ONLY Win64 binaries:${NC}"
    for proj in "${win_only_projects[@]}"; do
        echo "   - $proj"
    done
fi

if [ ${#mac_only_projects[@]} -gt 0 ]; then
    echo -e "🍎 ${YELLOW}Projects with ONLY Mac binaries:${NC}"
    for proj in "${mac_only_projects[@]}"; do
        echo "   - $proj"
    done
fi

if [ ${#fail_projects[@]} -gt 0 ]; then
    echo -e "❌ ${RED}Projects MISSING all binaries:${NC}"
    for proj in "${fail_projects[@]}"; do
        echo "   - $proj"
    done
fi

echo "------------------------------------------------------" 