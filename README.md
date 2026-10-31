# Quick Event Function Creator v2.1

<!-- badges:start -->
[![License](https://img.shields.io/github/license/monkey-sking/QuickEventFunctionCreator?style=flat-square)](https://github.com/monkey-sking/QuickEventFunctionCreator/blob/main/LICENSE) [![Last commit](https://img.shields.io/github/last-commit/monkey-sking/QuickEventFunctionCreator?style=flat-square)](https://github.com/monkey-sking/QuickEventFunctionCreator/commits/main) [![views](https://hits.sh/github.com/monkey-sking/QuickEventFunctionCreator.svg?style=flat-square&label=views&color=0e75b6)](https://github.com/monkey-sking/QuickEventFunctionCreator) [![Stars](https://img.shields.io/github/stars/monkey-sking/QuickEventFunctionCreator?style=flat-square)](https://github.com/monkey-sking/QuickEventFunctionCreator/stargazers) [![Forks](https://img.shields.io/github/forks/monkey-sking/QuickEventFunctionCreator?style=flat-square)](https://github.com/monkey-sking/QuickEventFunctionCreator/network/members)
<!-- badges:end -->

## Introduction

Quick Event Function Creator is a revolutionary Unreal Engine editor plugin that transforms your Blueprint development workflow. Using unified natural language syntax, you can instantly create variables, functions, events, macros, and variable nodes with smart aliases and intelligent positioning. What used to take minutes of clicking now takes seconds of typing.

## Table of Contents

- [🚀 What's New in v2.1](#-whats-new-in-v21)
- [✨ Features](#-features)
- [🔧 Supported Engine Versions](#-supported-engine-versions)
- [📦 Installation](#-installation)
- [🎮 How to Use](#-how-to-use)
- [⚙️ Settings](#️-settings)
- [📝 Unified Syntax](#-unified-syntax)
- [⚡ Smart Aliases](#-smart-aliases)
- [🧪 Examples](#-examples)
- [💬 Support](#-support)

## 🚀 What's New in v2.1

- **🆕 Macro Creation System:** Create Blueprint macros with full parameter support (in/out/inout/exec)
- **⚡ Enhanced Alias System:** Extended type aliases including exec→execute and parameter directions
- **🔧 Complete Parameter Support:** All macro parameter types with intelligent pin generation
- **🎯 Improved UI Experience:** Enhanced input hints with comprehensive syntax examples
- **🐛 Critical Fixes:** Resolved parameter parsing issues for seamless macro creation

### Previous v2.0 Features
- **🔥 Unified Syntax System:** One consistent language for variables, functions, and events
- **⚡ Smart Alias System:** 5 categories of shortcuts for maximum efficiency
- **🎯 Variable Node Creation:** Instantly generate Get/Set nodes for existing variables
- **🧠 Intelligent Context Detection:** Automatically creates local variables inside functions
- **🎨 Redesigned Settings UI:** Organized categories with comprehensive tooltips

## ✨ Features

- **🚀 Unified Creation:** Create variables, functions, events, macros, and variable nodes with one syntax
- **⚡ Smart Aliases:** Type 70% less with intelligent shortcuts (e→event, g→get, i→integer)
- **🧠 Context Awareness:** Automatically detects scope and creates appropriate variable types
- **🎯 Intelligent Positioning:** Places nodes exactly where you click in the graph
- **🔧 Full Type Support:** Works with all UE types including custom UCLASS, USTRUCT, UENUM
- **⚙️ Seamless Integration:** Hotkey access (`Shift + Left Click`) with complete settings panel
- **🌐 Universal Compatibility:** Works in all project types, no C++ environment needed

## 🔧 Supported Engine Versions

- **Unreal Engine 4:** 4.26, 4.27
- **Unreal Engine 5:** 5.0, 5.1, 5.2, 5.3, 5.4, 5.5, 5.6
- **Platforms:** Windows 64-bit, macOS

## 📦 Installation

1. **Download:** Get the plugin version that matches your Unreal Engine version
2. **Extract:** Create a `Plugins` folder in your project's root directory if needed
3. **Install:** Place the `QuickEventFunctionCreator` folder into your project's `Plugins` directory
4. **Restart:** Restart Unreal Engine editor - the plugin will be enabled automatically
5. **Verify:** Look for the toolbar button in Blueprint editor or use the hotkey `Shift + Left Click`

## 🎮 How to Use

### Quick Start
1. **Open** any Blueprint graph (Event Graph, Function Graph, etc.)
2. **Activate** with **Shift + Left-Click** anywhere on the graph
3. **Type** your creation command using the unified syntax
4. **Press Enter** - the node appears exactly where you clicked!

### Basic Workflow
```
Create Variable → Use Variable → Create Functions → Create Events
```

**Example Session:**
```
var i PlayerHealth          // Create global integer variable
g PlayerHealth             // Create Get node for PlayerHealth
st PlayerHealth            // Create Set node for PlayerHealth
fn CalculateDamage         // Create function
e OnPlayerDeath            // Create custom event
```

## ⚙️ Settings

Navigate to **Project Settings → Plugins → Quick Event Function Creator** for comprehensive customization:

### 📁 General
- **Show Usage Hints:** Toggle syntax help in the input dialog
- **Show Toolbar Button:** Display/hide the toolbar button in Blueprint editor
- **Reset All Settings:** One-click restore to defaults

### ⌨️ Hotkeys
- **Activation Chord:** Customize the hotkey (default: **Shift + Left Mouse Button**)

### 🎯 Behavior
- **Default Creation Type:** Choose what gets created when no keyword is specified (Event/Function/Variable)
- **Variable Node Auto-Spawn Mode:** Auto-generate Get/Set nodes when creating variables (Get/Set/None)

### 🏷️ Aliases (5 Categories)
- **Event Aliases:** Shortcuts for event-related commands
- **Function Aliases:** Shortcuts for function-related commands
- **Variable Aliases:** Shortcuts for variable-related commands
- **Node Aliases:** Shortcuts for Get/Set node operations
- **Data Type Aliases:** Shortcuts for all data types

All settings save automatically and take effect immediately.

## 📝 Unified Syntax

v2.0 introduces a unified syntax system that handles all Blueprint creation with one consistent language.

### 🎯 Basic Structure
```
[Modifiers] Type Name [Parameters]
```

### 📋 Creation Types

#### Variables
```
var <type> <name>           // Global variable
local <type> <name>         // Local variable (or use 'lv')
```

#### Functions
```
func <name> [parameters]    // Standard function
pure func <name>            // Pure function (or use 'p func')
const func <name>           // Const function (or use 'k func')
```

#### Events
```
event <name> [parameters]   // Custom event
server event <name>         // Server event (or use 's event')
client event <name>         // Client event (or use 'c event')
multicast event <name>      // Multicast event (or use 'm event')
reliable server event       // Reliable server event (or use 'rel s event')
```

#### 🆕 Macros (v2.1)
```
macro <name> [parameters]       // Blueprint macro
macro Calculate(float Input, out float Result)
macro ProcessData(exec In, string Data, inout bool Flag, out exec Success)
```

**Parameter Directions:**
- `in` / `input`: Input parameter (default)
- `out` / `output`: Output parameter
- `inout` / `ref` / `io`: Input/Output parameter
- `exec` / `execute`: Execution pin

#### Variable Nodes (for existing variables)
```
get <variable_name>         // Create Get node
set <variable_name>         // Create Set node
```

### 🧠 Smart Context Detection
- **In Functions:** `var` automatically creates local variables
- **In Event Graphs:** `var` creates global variables
- **Anywhere:** Use `local`/`lv` to force local variable creation

## ⚡ Smart Aliases

v2.1 features a comprehensive 6-category alias system that reduces typing by up to 70%. All aliases are fully customizable in Project Settings.

### 🎭 Event Aliases
| Alias | Full Command | Example |
|-------|-------------|---------|
| `e` | event | `e OnDeath` |
| `s` | server | `s OnUpdate` |
| `c` | client | `c OnInput` |
| `m` | multicast | `m OnBroadcast` |
| `rel` | reliable | `rel s OnSync` |

### ⚙️ Function Aliases
| Alias | Full Command | Example |
|-------|-------------|---------|
| `fn` | func | `fn Calculate` |
| `p` | pure | `p fn GetScore` |
| `k` | const | `k fn GetMax` |

### 📦 Variable Aliases
| Alias | Full Command | Example |
|-------|-------------|---------|
| `var` | var | `var i Health` |
| `lv` | local | `lv str Name` |
| `local` | local | `local b IsReady` |

### 🆕 Macro Aliases (v2.1)
| Alias | Full Command | Example |
|-------|-------------|---------|
| `m` | macro | `m Calculate(float Input, out float Result)` |
| `macro` | macro | `macro ProcessData(exec In, string Data, inout bool Flag)` |

**Parameter Direction Aliases:**
- `in` / `input`: Input parameter (default)
- `out` / `output`: Output parameter
- `inout` / `ref` / `io`: Input/Output parameter
- `exec` / `execute`: Execution pin

### 🔗 Node Operation Aliases
| Alias | Full Command | Example |
|-------|-------------|---------|
| `g` | get | `g PlayerHealth` |
| `st` | set | `st PlayerHealth` |

### 🏷️ Data Type Aliases
| Alias | Full Type | Alternative Aliases |
|-------|-----------|-------------------|
| `b` | boolean | `bool`, `boolean` |
| `u8` | byte | `uint8`, `byte` |
| `i` | integer | `int`, `int32`, `integer` |
| `i64` | integer64 | `int64`, `long`, `integer64` |
| `f` | float | `float`, `double` |
| `str` | string | `string` |
| `n` | name | `name` |
| `txt` | text | `text` |
| `v` | vector | `vec`, `vec3`, `vector` |
| `r` | rotator | `rot`, `rotator` |
| `t` | transform | `xform`, `transform` |
| `exec` | execute | `execute` |

### 💡 Alias Examples
```
e OnHit(i Damage, v Location)           // event OnHit(integer Damage, vector Location)
p fn GetDist(v A, v B)                  // pure func GetDistance(vector A, vector B)
var str PlayerName                      // var string PlayerName
lv b IsReady                           // local boolean IsReady
g PlayerHealth                         // get PlayerHealth (creates Get node)
st PlayerHealth                        // set PlayerHealth (creates Set node)

// 🆕 Macro Examples (v2.1)
m Calculate(f Input, out f Result)      // macro Calculate(float Input, out float Result)
macro ProcessData(exec In, str Data, inout b Flag, out exec Success)
macro ValidateInput(in str Data, out b IsValid, out str ErrorMsg)
```

## 🧪 Examples

### 🎮 Complete Workflow Example
```
// 1. Create variables
var i PlayerHealth                      // Global integer variable
var f MaxSpeed                         // Global float variable
lv str PlayerName                      // Local string variable (in functions)

// 2. Use variables (create nodes)
g PlayerHealth                         // Create Get node for PlayerHealth
st PlayerHealth                        // Create Set node for PlayerHealth

// 3. Create functions
fn CalculateDamage                     // Standard function
p fn GetPlayerScore                    // Pure function
k fn GetMaxHealth                      // Const function

// 4. Create events
e OnPlayerDeath                        // Custom event
s OnServerUpdate                       // Server event
rel m OnGameStateChanged               // Reliable multicast event

// 5. With parameters
fn CalculateDamage(i BaseDamage, f Multiplier)
e OnPlayerHit(i Damage, v HitLocation, str AttackerName)
```

### 🚀 Speed Comparison
**Traditional Method:** 15+ clicks, 2-3 minutes
**v2.0 Method:** Type `var i Health` → 2 seconds ⚡

### 🎯 Pro Tips
- Use `lv` in functions for quick local variables
- Combine modifiers: `rel s e OnSync` = reliable server event
- Chain workflow: create variable → use `g`/`st` for nodes
- Customize aliases in settings for your preferred shortcuts

## 💬 Support

### 🆘 Get Help
- **Discord Community:** <https://discord.gg/tvemMvE63r>
- **FAB Marketplace:** <https://www.fab.com/zh-cn/listings/85e81b75-ee98-4d47-83e0-c6374dedf5ae>
- **GitHub Issues:** Report bugs and request features

### 📚 Resources
- **Project Settings:** Complete customization options
- **In-Editor Help:** Enable usage hints in General settings
- **Community:** Share workflows and tips on Discord

---

**Transform your Blueprint development today!** 🚀
*What used to take minutes now takes seconds.*
