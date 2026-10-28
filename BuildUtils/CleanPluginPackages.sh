#!/bin/bash

# =================================================================================
#
#  CleanPluginPackages.sh
#
#  This script cleans the packaged plugin directories in the 'Release' folder
#  on macOS. It removes large, non-essential files and folders to prepare the
#  plugin for distribution on the Unreal Engine Marketplace.
#
#  What it does:
#  1. Recursively deletes the entire 'Intermediate' folder.
#  2. In the 'Binaries' folder, it deletes any subdirectory that is NOT
#     'Win64' or 'Mac', removing unnecessary platform binaries.
#
# =================================================================================

# --- Configuration ---
# The script assumes it's being run from the project root.
RELEASE_ROOT="$(pwd)/Release"
PLUGIN_NAME="QuickEventFunctionCreator"

echo "--- Cleaning up packaged plugin folders for Marketplace release ---"
echo "Target Path: $RELEASE_ROOT"
echo

if [ ! -d "$RELEASE_ROOT" ]; then
    echo "[ERROR] Release directory not found at '$RELEASE_ROOT'."
    echo "Please run this script from the project root directory."
    exit 1
fi

for engine_dir in "$RELEASE_ROOT"/UE_*; do
    if [ -d "$engine_dir" ]; then
        engine_version_name=$(basename "$engine_dir")
        plugin_path="$engine_dir/$PLUGIN_NAME"

        if [ -d "$plugin_path" ]; then
            echo "--- Processing [$engine_version_name] ---"

            # 1. Delete the Intermediate folder
            intermediate_path="$plugin_path/Intermediate"
            if [ -d "$intermediate_path" ]; then
                echo "  - Removing 'Intermediate' folder..."
                rm -rf "$intermediate_path"
                echo "    Done."
            else
                echo "  - 'Intermediate' folder not found, skipping."
            fi

            # 2. Clean up the Binaries folder
            binaries_path="$plugin_path/Binaries"
            if [ -d "$binaries_path" ]; then
                echo "  - Cleaning 'Binaries' folder (keeping only Win64 & Mac)..."
                # Loop through items in the Binaries directory
                for item in "$binaries_path"/*; do
                    if [ -d "$item" ]; then # Check if it's a directory
                        folder_name=$(basename "$item")
                        if [ "$folder_name" != "Win64" ] && [ "$folder_name" != "Mac" ]; then
                            echo "    - Removing non-essential binary folder: '$folder_name'"
                            rm -rf "$item"
                        fi
                    fi
                done
                echo "    Done."
            else
                echo "  - 'Binaries' folder not found, skipping."
            fi
            echo
        fi
    fi
done

echo "------------------------------------------------------------------"
echo "--- Cleanup Complete ---"
echo "All packaged plugins in the 'Release' folder have been cleaned."
echo "------------------------------------------------------------------" 