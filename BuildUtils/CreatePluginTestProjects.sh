#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# --- Configuration ---
# Path to the directory where Unreal Engine versions are installed.
# [[memory:1492725]]
UE_ENGINE_ROOT="/Volumes/SSD/UnrealEngine"
# Path to the directory containing the packaged plugins. Uses the current directory's "Release" subfolder.
PLUGIN_RELEASE_PATH="$(pwd)/Release"
# Path to the directory where test projects will be created.
TEST_PROJECTS_ROOT="/Volumes/SSD/UnrealProjects/PluginTests"
# The name of the plugin folder.
PLUGIN_NAME="QuickEventFunctionCreator"

# --- Engine Versions to Test ---
# List of UE versions to create test projects for.
# This should correspond to the subdirectories in your PLUGIN_RELEASE_PATH.
VERSIONS=(4.26 4.27 5.0 5.1 5.2 5.3 5.4 5.5 5.6)

# --- Main Script Logic ---
echo "Starting test project creation process..."
echo "Engine Root: $UE_ENGINE_ROOT"
echo "Plugin Release Path: $PLUGIN_RELEASE_PATH"
echo "Test Projects Root: $TEST_PROJECTS_ROOT"
echo "--------------------------------------------------"

# Create the root directory for test projects if it doesn't exist.
mkdir -p "$TEST_PROJECTS_ROOT"

for VERSION in "${VERSIONS[@]}"; do
  echo "--- Processing UE $VERSION ---"

  # Define paths for this specific version
  UE_FOLDER="$UE_ENGINE_ROOT/UE_$VERSION"
  PROJECT_FOLDER="$TEST_PROJECTS_ROOT/PluginTest_$VERSION"
  PLUGIN_SOURCE="$PLUGIN_RELEASE_PATH/UE_$VERSION/$PLUGIN_NAME"
  PLUGIN_TARGET="$PROJECT_FOLDER/Plugins/$PLUGIN_NAME"

  # --- 1. Validations ---
  if [ ! -d "$UE_FOLDER" ]; then
      echo "[WARN] Engine for UE $VERSION not found at $UE_FOLDER. Skipping."
      continue
  fi

  if [ ! -d "$PLUGIN_SOURCE" ]; then
      echo "[WARN] Packaged plugin for UE $VERSION not found at $PLUGIN_SOURCE. Skipping."
      continue
  fi

  # --- 2. Clean up old project if it exists ---
  if [ -d "$PROJECT_FOLDER" ]; then
      echo "  -> Removing existing project folder..."
      rm -rf "$PROJECT_FOLDER"
  fi

  # --- 4. Create a blank uproject file first ---
  echo "  -> Creating a blank .uproject file..."
  PROJECT_NAME="PluginTest"
  UPROJECT_FILE="$PROJECT_FOLDER/$PROJECT_NAME.uproject"
  mkdir -p "$PROJECT_FOLDER"
  
  # --- 4a. Create the .uproject file for a BLUEPRINT-ONLY project ---
  echo "  -> Creating .uproject file for a blueprint-only project..."
  # The "Modules" array must be empty for a blueprint-only project.
cat <<EOT > "$UPROJECT_FILE"
 {
 	"FileVersion": 3,
 	"EngineAssociation": "$VERSION",
 	"Category": "",
 	"Description": "",
 	"Modules": [],
 	"Plugins": [
 		{
 			"Name": "$PLUGIN_NAME",
 			"Enabled": true
 		}
 	]
 }
EOT
 
  # --- 4b. Create a config file to minimize shader compilation and speed up launch ---
  echo "  -> Creating config file to disable shader compilation..."
  CONFIG_DIR="$PROJECT_FOLDER/Config"
  mkdir -p "$CONFIG_DIR"
cat <<EOF > "$CONFIG_DIR/DefaultEngine.ini"
[/Script/Engine.RendererSettings]
; This is a more direct way to prevent the editor from compiling shaders on startup.
r.ShaderCompiler=0

; The following settings further ensure a minimal rendering environment for the fastest possible launch.
r.DefaultFeatureLevel=ES3_1
r.Nanite=0
r.Lumen.ScreenProbeGather.Allow=0
r.Shadow.Virtual.Enable=0
EOF

   # --- 5. Copy the packaged plugin into the new project FIRST ---
   echo "  -> Copying plugin..."
   mkdir -p "$(dirname "$PLUGIN_TARGET")"
   cp -R "$PLUGIN_SOURCE" "$PLUGIN_TARGET"

   # --- 6. Generate project files using the dedicated script ---
   echo "  -> Generating project files using GenerateProjectFiles.sh..."
   # This must be run AFTER the plugin is copied, so UBT can find the plugin referenced in the .uproject.
   GENERATE_SCRIPT_PATH="$UE_FOLDER/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh"
   
   if [ ! -f "$GENERATE_SCRIPT_PATH" ]; then
       echo "[ERROR] GenerateProjectFiles.sh not found at '$GENERATE_SCRIPT_PATH'. Skipping."
       continue
   fi
 
   "$GENERATE_SCRIPT_PATH" -project="$UPROJECT_FILE" -game
 
  echo "[SUCCESS] Project for UE $VERSION ready at: $PROJECT_FOLDER"
  echo "-------------------------------------"
done

echo "All test projects created successfully." 