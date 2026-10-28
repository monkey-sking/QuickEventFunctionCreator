#!/bin/bash

# ==============================================================================
# QuickEventFunctionCreator - One-Click Build Script for All UE Versions
# ==============================================================================
#
# This script automates the process of packaging the plugin for all supported
# Unreal Engine versions on macOS. It uses an isolated build environment for
# each version to ensure a clean and reliable build.
#
# ==============================================================================

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration ---
SUPPORTED_VERSIONS=("4.26" "4.27" "5.0" "5.1" "5.2" "5.3" "5.4" "5.5" "5.6" "5.7")
UE_INSTALL_BASE="/Volumes/SSD/UnrealEngine"
PROJECT_ROOT=$(pwd)
PLUGIN_FILE_NAME="QuickEventFunctionCreator.uplugin"
PLUGIN_NAME="QuickEventFunctionCreator"
RELEASE_DIR="$PROJECT_ROOT/Release"

# Paths to required Xcode versions
XCODE_13_2_PATH="/Applications/Xcode13.2.1.app"
XCODE_13_4_PATH="/Applications/Xcode13.4.1.app"
XCODE_14_PATH="/Applications/Xcode14.3.1.app"
XCODE_LATEST_PATH="/Applications/Xcode.app"

# --- Helper Functions ---
log_info() { echo -e "\033[0;34m[INFO] $1\033[0m"; }
log_success() { echo -e "\033[0;32m[SUCCESS] $1\033[0m"; }
log_warning() { echo -e "\033[0;33m[WARNING] $1\033[0m"; }
log_error() {
    echo -e "\033[0;31m[ERROR] $1\033[0m"
    # Clean up build directory on error
    if [ -n "$BUILD_DIR" ] && [ -d "$BUILD_DIR" ]; then
        log_info "Cleaning up temporary build directory: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi
    # Restore Xcode path
    if [ -n "$ORIGINAL_XCODE_PATH" ]; then
      log_info "Attempting to restore original Xcode path..."
      sudo xcode-select -s "$ORIGINAL_XCODE_PATH"
    fi
    exit 1
}

# --- Version Selection Logic ---
VERSIONS_TO_BUILD=()
if [ -z "$1" ]; then
    log_info "Please choose from the menu below:"
    PS3="Enter your choice: "
    options=()
    for version in "${SUPPORTED_VERSIONS[@]}"; do options+=("UE $version"); done
    options+=("Build All Supported Versions")
    options+=("Quit")
    select opt in "${options[@]}"; do
        case $opt in
            "Build All Supported Versions") VERSIONS_TO_BUILD=("${SUPPORTED_VERSIONS[@]}"); break;;
            "Quit") exit 0;;
            *)
                if [[ " ${options[*]} " =~ " ${opt} " ]]; then
                    VERSIONS_TO_BUILD=($(echo "$opt" | cut -d' ' -f2)); break
                else
                    log_error "Invalid selection."; continue
                fi;;
        esac
    done
else
    if [[ " ${SUPPORTED_VERSIONS[@]} " =~ " $1 " ]]; then
        VERSIONS_TO_BUILD=("$1")
    else
        log_error "Invalid version specified: '$1'. Choose from: ${SUPPORTED_VERSIONS[*]}"
        exit 1
    fi
fi

if [ ${#VERSIONS_TO_BUILD[@]} -eq 0 ]; then log_error "No versions selected. Exiting."; exit 1; fi

# --- Pre-Build Checks ---
log_info "Starting plugin build process for: ${VERSIONS_TO_BUILD[*]}"
if [ ! -f "$PROJECT_ROOT/$PLUGIN_FILE_NAME" ]; then log_error "Plugin file not found."; fi
ORIGINAL_XCODE_PATH=$(xcode-select -p)
log_info "Original Xcode path is '$ORIGINAL_XCODE_PATH'. It will be restored."
log_info "Sudo privileges are required to switch Xcode versions."
sudo -v; while true; do sudo -n true; sleep 60; kill -0 "$$" || exit; done 2>/dev/null &

# ========================= MAIN BUILD LOOP =========================
for UE_VERSION in "${VERSIONS_TO_BUILD[@]}"; do
    BUILD_DIR="" # Ensure BUILD_DIR is reset for the error trap
    echo " "; log_info "========== Building for Unreal Engine $UE_VERSION =========="

    # 1. Create a temporary, isolated build environment
    BUILD_DIR=$(mktemp -d)
    log_info "Created isolated build directory: $BUILD_DIR"

    # 2. Copy only the necessary plugin files to the isolated directory
    log_info "Copying latest plugin source to isolated directory..."
    cp "$PROJECT_ROOT/$PLUGIN_FILE_NAME" "$BUILD_DIR/"
    cp -R "$PROJECT_ROOT/Source" "$BUILD_DIR/"
    [ -d "$PROJECT_ROOT/Resources" ] && cp -R "$PROJECT_ROOT/Resources" "$BUILD_DIR/"
    [ -d "$PROJECT_ROOT/Config" ] && cp -R "$PROJECT_ROOT/Config" "$BUILD_DIR/"

    # 3. Define paths using the isolated environment
    ENGINE_PATH="$UE_INSTALL_BASE/UE_$UE_VERSION"
    PLUGIN_UPLUGIN_PATH_ISOLATED="$BUILD_DIR/$PLUGIN_FILE_NAME"
    PACKAGE_OUTPUT_PATH="$BUILD_DIR/Output"

    # 4. Select and switch Xcode version
    log_info "Selecting appropriate Xcode for UE $UE_VERSION..."
    TARGET_XCODE_PATH=""
    case "$UE_VERSION" in
        4.26|4.27) TARGET_XCODE_PATH="$XCODE_13_2_PATH";;
        5.0) TARGET_XCODE_PATH="$XCODE_13_4_PATH";;
        5.1|5.2|5.3) TARGET_XCODE_PATH="$XCODE_14_PATH";;
        5.4|5.5|5.6|5.7) TARGET_XCODE_PATH="$XCODE_LATEST_PATH";;
        *) log_warning "No specific Xcode for UE $UE_VERSION. Using latest."; TARGET_XCODE_PATH="$XCODE_LATEST_PATH";;
    esac
    if [ ! -d "$TARGET_XCODE_PATH" ]; then log_error "Xcode not found at $TARGET_XCODE_PATH"; continue; fi
    log_info "Switching to Xcode at $TARGET_XCODE_PATH..."
    sudo xcode-select -s "$TARGET_XCODE_PATH/Contents/Developer"
    if [ ! -d "$ENGINE_PATH" ]; then log_warning "Engine path not found for $UE_VERSION. Skipping."; continue; fi

    # 5. Run the build in the isolated directory
    log_info "Running AutomationTool... This may take a few minutes."
    UAT_SCRIPT_PATH="$ENGINE_PATH/Engine/Build/BatchFiles/RunUAT.sh"
    "$UAT_SCRIPT_PATH" BuildPlugin \
        -Plugin="$PLUGIN_UPLUGIN_PATH_ISOLATED" \
        -Package="$PACKAGE_OUTPUT_PATH" \
        -rocket

    # 6. Prepare final destination and move the packaged plugin
    FINAL_PLUGIN_PARENT="$RELEASE_DIR/Plugins_$UE_VERSION/Plugins"
    FINAL_PLUGIN_DIR="$FINAL_PLUGIN_PARENT/$PLUGIN_NAME"
    log_info "Packaging complete. Moving plugin to $FINAL_PLUGIN_DIR"
    mkdir -p "$FINAL_PLUGIN_PARENT"
    rm -rf "$FINAL_PLUGIN_DIR"
    mkdir -p "$FINAL_PLUGIN_DIR"
    # Using shopt to ensure dotfiles are also moved
    shopt -s dotglob
    mv "$PACKAGE_OUTPUT_PATH"/* "$FINAL_PLUGIN_DIR/"
    shopt -u dotglob

    # 7. Clean unwanted folders for Epic Games submission
    log_info "Cleaning packaged plugin for marketplace submission..."
    find "$FINAL_PLUGIN_DIR" -type d \( -name "Binaries" -o -name "Intermediate" -o -name "Build" -o -name "Saved" \) -exec rm -rf {} +

    # 8. Clean up the temporary isolated directory
    log_info "Cleaning up temporary build directory..."
    rm -rf "$BUILD_DIR"
    
    log_success "Successfully packaged plugin for UE $UE_VERSION."
done

# --- Finalization ---
log_info "================================================================"
log_info "All builds finished. Restoring original Xcode path..."
sudo xcode-select -s "$ORIGINAL_XCODE_PATH"
kill %% # Kill the sudo keep-alive loop
echo " "; log_success "Build process complete! All plugins are in '$RELEASE_DIR'."
echo " "
exit 0 
