#!/bin/bash

# Set the source and destination directories
SOURCE_DIR="Release"
DEST_DIR="ZippedPackages"
PLUGIN_NAME="QuickEventFunctionCreator"

# Create the destination directory if it doesn't exist
mkdir -p "$DEST_DIR"

# Find all QuickEventFunctionCreator.uplugin files in the source directory
find "$SOURCE_DIR" -name "QuickEventFunctionCreator.uplugin" | while read -r uplugin_file; do
    # Get the parent directory of the uplugin file
    plugin_root=$(dirname "$uplugin_file")

    # Get the version from the directory path (e.g., Plugins_5.5)
    version_string=$(echo "$plugin_root" | grep -o 'Plugins_[^/]*' | cut -d'_' -f2)

    # Define the absolute path for the output zip file
    zip_file_path="$(pwd)/$DEST_DIR/${PLUGIN_NAME}_${version_string}.zip"

    echo "Packaging version ${version_string}..."

    # Go to the directory containing the "Plugins" folder to be zipped
    package_base_dir=$(dirname "$(dirname "$plugin_root")")

    # Zip the "Plugins" directory, excluding unnecessary folders and files
    (
        cd "$package_base_dir" || exit
        zip -r "$zip_file_path" "Plugins" -x "*/Binaries/*" "*/Intermediate/*" "*/Saved/*" "**/.DS_Store"
    )

    if [ $? -eq 0 ]; then
        echo "Successfully created ${zip_file_path}"
    else
        echo "Failed to create ${zip_file_path}"
    fi
done

echo "All versions have been packaged." 