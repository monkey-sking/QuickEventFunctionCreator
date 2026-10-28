# Contributing to Quick Event Function Creator

Welcome! We appreciate your interest in contributing to Quick Event Function Creator. This document provides guidelines and instructions for setting up your development environment, building the plugin, and submitting changes.

## Project Structure

*   **Plugins/QuickEventFunctionCreator/**: The core plugin source code.
    *   **Source/**: C++ source files (`Public` and `Private`).
    *   **Resources/**: Icons and other assets.
    *   **BuildUtils/**: Shell and Batch scripts for building and packaging.
    *   **docs/**: Documentation and design diagrams.
*   **Release/**: Output directory for packaged plugins.

## Development Setup

### Prerequisites

*   **Unreal Engine**: You need the specific Engine versions installed that you intend to build for (e.g., 4.26, 5.0, 5.7, etc.).
*   **IDE**: Visual Studio (Windows) or Xcode (macOS).
*   **Git**: For version control.
*   **Platform**: Windows or macOS (Scripts are optimized for macOS/Bash but Windows batches exist).

### Building Locally

You can open the `.uproject` file in the root directory (if available) or generate project files for the plugin in your own test project.

1.  Clone the repository.
2.  Right-click `Next.uproject` (or your test project) and select "Generate Project Files".
3.  Open the solution/workspace in your IDE.
4.  Build the project in "Development Editor" and "Mac/Win64" configuration.

## Packaging for Release

We provide automated scripts to package the plugin for multiple UE versions.

### macOS

Use the scripts in `Plugins/QuickEventFunctionCreator/BuildUtils/`:

*   **`BuildAllVersions.sh`**: The main script to build binaries for all supported versions.
    *   Requires `sudo` access to switch active Xcode versions relative to the UE version.
    *   Generates output in the `Release/` folder.
*   **`SyncSourceToRelease.sh`**: Synchronizes the *current* C++ source code to all versions in the `Release/` folder without rebuilding binaries.
    *   Use this for rapid updates when only source code changes (packaging source-only plugins).
*   **`PackageAllVersions.sh`**: Zips the content of `Release/` into encrypted archives for Fab/Marketplace submission.

**Example Workflow:**

```bash
cd Plugins/QuickEventFunctionCreator
./BuildUtils/BuildAllVersions.sh  # Build binaries (optional if source-only)
./BuildUtils/SyncSourceToRelease.sh # Sync latest source
./BuildUtils/PackageAllVersions.sh # Create final zip
```

### Windows

Use the `.bat` files in `BuildUtils/` (e.g., `BuildAllVersions.bat`). Note that you may need to adjust paths for your local environment.

## Code Style

*   Follow the [Unreal Engine Coding Standard](https://docs.unrealengine.com/5.0/en-US/coding-standard/).
*   Use `TArray`, `FString`, `FName` etc. over STL equivalents.
*   Prefix member variables with `b` (bools) or other standard prefixes.

## New Features & Roadmap

Check `docs/` for implementation details on recent features like **Macros** and **Variable Creation**.

*   `variable_creation_implementation.md`: Details on `var` syntax.
*   `macro_creation_implementation.md`: Details on `macro` / `mc` syntax.

## Reporting Issues

Please report bugs via Discord or GitHub Issues. Include reproduction steps and the Engine version used.
