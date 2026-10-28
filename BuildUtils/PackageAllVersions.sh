#!/bin/bash
#
# This script packages all versioned plugins from the Release directory
# into individual, encrypted .zip files suitable for Fab marketplace submission.

set -e

# --- Configuration ---
PROJECT_ROOT=$(git rev-parse --show-toplevel)
RELEASE_DIR="$PROJECT_ROOT/Release"
DEST_DIR="$PROJECT_ROOT/ZippedPackages"
PLUGIN_NAME="QuickEventFunctionCreator"

# --- Helper Functions ---
log_info() { echo -e "\033[0;34m[INFO] $1\033[0m"; }
log_success() { echo -e "\033[0;32m[SUCCESS] $1\033[0m"; }
log_warning() { echo -e "\033[0;33m[WARNING] $1\033[0m"; }

# --- Main Packaging Logic ---
log_info "Starting packaging process for Fab submission..."
mkdir -p "$DEST_DIR"

# --- Password Input (only once) ---
if [ -z "${ZIP_PASSWORD:-}" ]; then
    read -s -p "Enter password: " ZIP_PASSWORD
    echo
    read -s -p "Confirm password: " ZIP_PASSWORD_CONFIRM
    echo
    if [ "$ZIP_PASSWORD" != "$ZIP_PASSWORD_CONFIRM" ]; then
        echo "Passwords do not match. Aborting."
        exit 1
    fi
else
    log_info "Using ZIP_PASSWORD from environment."
fi

cd "$RELEASE_DIR"

for version_dir in Plugins_*; do
    if [ -d "$version_dir" ]; then
        # Extract version number (e.g., 5.6 from Plugins_5.6)
        UE_VERSION=$(echo "$version_dir" | cut -d'_' -f2)
        
        SOURCE_PLUGIN_PATH="$version_dir/Plugins/$PLUGIN_NAME"
        # Use absolute path for destination to avoid issues with pushd/popd
        DEST_ZIP_FILE="$(cd "$DEST_DIR" && pwd)/${PLUGIN_NAME}_${UE_VERSION}.zip"

        if [ ! -d "$SOURCE_PLUGIN_PATH" ]; then
            log_warning "Plugin source for version $UE_VERSION not found at $SOURCE_PLUGIN_PATH. Skipping."
            continue
        fi

        log_info "Packaging plugin for UE ${UE_VERSION}..."

        # Navigate to the directory containing the plugin folder to set the archive root correctly
        pushd "$(dirname "$SOURCE_PLUGIN_PATH")" > /dev/null

        rm -f "$DEST_ZIP_FILE"

        # Create an encrypted, recursive zip file using the provided password (non-interactive)
        zip -r -P "$ZIP_PASSWORD" "$DEST_ZIP_FILE" "$PLUGIN_NAME" \
            -x "*/.DS_Store" "*/__MACOSX*" "*/.git/*" "*/Intermediate/*" "*/Binaries/*" "*/Saved/*"

        popd > /dev/null
        
        log_success "Successfully created encrypted zip: $DEST_ZIP_FILE"
    fi
done

echo " "
log_success "All versions have been packaged into '$DEST_DIR'."
exit 0
