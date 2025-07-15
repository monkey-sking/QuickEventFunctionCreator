# Quick Event Function Creator

## Introduction

Quick Event Function Creator is an Unreal Engine editor plugin that dramatically speeds up your Blueprint development workflow. It allows you to create new Blueprint Functions and Custom Events by simply typing a C++ style function signature, eliminating the need to manually add nodes and configure pins.

## Table of Contents

- [Features](#features)
- [Supported Engine Versions](#supported-engine-versions)
- [Installation](#installation)
- [How to Use](#how-to-use)
- [Customization](#customization)
- [Signature Syntax](#signature-syntax)
- [Default Aliases](#default-aliases)
- [Support](#support)

## Features

- **Rapid Function & Event Creation:** Type a signature like `MyFunction(int count, FString name)` and instantly generate the corresponding Blueprint nodes.
- **Full Parameter Support:** Supports all common Blueprint data types (e.g., `bool`, `int`, `float`, `FString`, `FVector`, `UObject*` references, etc.).
- **Smart Keyword Recognition:** Use keywords like `func`, `event`, `pure`, and `const` to define the exact type of node you need.
- **Seamless Integration:** Access the creation dialog with a simple hotkey (`Shift + Left Click`) directly within any Blueprint graph.
- **Works Everywhere:** Fully compatible with all project types, including Blueprint-only projects. No C++ environment needed.

## Supported Engine Versions

- 4.26
- 4.27
- 5.0
- 5.1
- 5.2
- 5.3
- 5.4
- 5.5
- 5.6


## How to Use

1.  Open any Blueprint graph (Event Graph, Function Graph, etc.).
2.  Hold down **Shift** and **Left-Click** anywhere on the empty graph space.
3.  A text input dialog will appear.
4.  Type the function or event signature you want to create.
5.  Press **Enter** or click **OK**.
6.  The corresponding Function or Custom Event node will be created at the cursor's location.

## Customization

Open *Project Settings → Plugins → Quick Event & Function Creator* to tweak the following options:

1. **Activation Hotkey**  
    Default is **Shift + Left Mouse Button**. Assign any key or mouse combination you like.
2. **Default Creation Type**  
    Decide whether an **Event** or a **Function** should be created when the `event` / `func` keyword is omitted.
3. **Alias Management**  
    Fully customize command keywords and data-type aliases (see table below) and reset them to defaults at any time.
4. **Usage Hint**  
    Toggle the syntax hint shown below the input box.

Click **Save Config** or simply close the window to automatically save your changes.

## Signature Syntax

The syntax is designed to be intuitive and mirrors C++ function declarations.

**Basic Structure:**
`NodeType FunctionName(ParamType ParamName, ...)`

- **NodeType (Optional):**
    - `func`: Creates a new Function.
    - `event`: Creates a new Custom Event.
    - If omitted, the plugin will use your default setting (configurable in Project Settings).
- **FunctionName:** The name of your new function or event.
- **Parameters (Optional):** A comma-separated list of `ParamType ParamName`.
- **Function Specifiers (Optional, for `func` only):**
    - `pure`: Creates a pure function (no execution pins).
    - `const`: Creates a const function.

**Examples:**

- `event OnPlayerDamaged(float Damage, AActor* DamageInstigator)`
- `func pure CalculateDistance(FVector A, FVector B)`
- `CheckSomething(bool bIsReady)`

## Default Aliases

Using aliases lets you type common keywords and data types faster. The table below lists the built-in shortcuts. They are completely optional and can be customized or disabled under *Project Settings → Quick Event & Function Creator → Aliases*.

### Command Keywords

| Alias | Full Write |
|-------|------------|
| e     | event      |
| fn    | func       |
| s     | server     |
| c     | client     |
| m     | multicast  |
| rel   | reliable   |
| p     | pure       |
| k     | const      |

### Data Types

| Alias          | Full Write |
|----------------|------------|
| b / bool       | boolean    |
| u8             | byte       |
| i / int        | integer    |
| i64 / long     | integer64  |
| float / f      | double     |
| str            | string     |
| n              | name       |
| txt            | text       |
| v / vec / vec3 | vector     |
| r / rot        | rotator    |
| t / xform      | transform  |

> **Example:**  
> `e OnHit(i Damage, v Location)` is interpreted as `event OnHit(int Damage, FVector Location)`
Video Tutorial: https://youtu.be/pf_2mYNIzng

## Support

For support, bug reports, or feature requests:

- Discord: <https://discord.gg/tvemMvE63r>

--- 