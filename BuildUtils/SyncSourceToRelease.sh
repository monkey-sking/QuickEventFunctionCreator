#!/bin/bash
#
# This script synchronizes the master source code from the project root
# to all individual plugin directories within the Release/ folder.

set -e

# --- Configuration ---
PROJECT_ROOT=$(git rev-parse --show-toplevel)
RELEASE_DIR="$PROJECT_ROOT/Release"
PLUGIN_NAME="QuickEventFunctionCreator"

# Source files and directories to be synchronized
SOURCE_FILES=(
    "Source"
    "Resources"
    "QuickEventFunctionCreator.uplugin"
)

# --- Helper Functions ---
log_info() { echo -e "\033[0;34m[INFO] $1\033[0m"; }
log_success() { echo -e "\033[0;32m[SUCCESS] $1\033[0m"; }
log_warning() { echo -e "\033[0;33m[WARNING] $1\033[0m"; }

# --- Main Sync Logic ---
log_info "Starting synchronization of source files to all release packages..."
cd "$RELEASE_DIR"

for version_dir in Plugins_*; do
    if [ -d "$version_dir" ]; then
        TARGET_PLUGIN_DIR="$version_dir/Plugins/$PLUGIN_NAME"
        
        if [ ! -d "$version_dir/Plugins" ]; then
            log_warning "Plugins directory in $version_dir does not exist. Skipping."
            continue
        fi

        log_info "Recreating a clean plugin directory at $TARGET_PLUGIN_DIR..."

        # Completely remove the old plugin directory for a clean slate
        if [ -d "$TARGET_PLUGIN_DIR" ]; then
            rm -rf "$TARGET_PLUGIN_DIR"
        fi
        mkdir -p "$TARGET_PLUGIN_DIR"

        # Copy fresh files and directories from the project root
        for item in "${SOURCE_FILES[@]}"; do
            if [ -e "$PROJECT_ROOT/$item" ]; then
                cp -R "$PROJECT_ROOT/$item" "$TARGET_PLUGIN_DIR/"
            fi
        done

        # --- DYNAMICALLY SET ENGINE VERSION ---
        # Extract version from directory name (e.g., "Plugins_5.4" -> "5.4")
        engine_version=$(echo "$version_dir" | sed 's/Plugins_//')
        target_uplugin_file="$TARGET_PLUGIN_DIR/$PLUGIN_NAME.uplugin"

        log_info "Dynamically setting EngineVersion to ${engine_version}.0 in $target_uplugin_file"
        
        # Replace the EngineVersion value. This is macOS compatible.
        # It assumes the EngineVersion field already exists in the source .uplugin file.
        sed -i '' "s/\"EngineVersion\": \".*\"/\"EngineVersion\": \"${engine_version}.0\"/" "$target_uplugin_file"
        
        log_success "Successfully synced to $TARGET_PLUGIN_DIR"
    fi
done

echo " "
log_success "Synchronization complete."
exit 0

