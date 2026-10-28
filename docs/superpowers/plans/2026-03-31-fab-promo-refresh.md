# Fab Promo Refresh Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Produce 5 finished Fab-ready promo images for Quick Event Function Creator, plus the editable source files and local render/export tooling needed to revise them later.

**Architecture:** Build the promo set as static 1920x1080 HTML slides with one shared stylesheet and one slide file per image. Render the slides locally to PNG with a Swift `WKWebView` snapshot script so the workflow stays deterministic and does not depend on Node.js or external design tools. Keep sources, exports, and usage notes together under one repo-local marketing folder.

**Tech Stack:** HTML, CSS, local plugin asset reuse, Swift with WebKit/AppKit, macOS command-line validation (`sips`)

---

## File Structure

### New files
- `docs/marketing/fab-promo-refresh/README.md`
  - usage notes, output list, render command, and manual review checklist
- `docs/marketing/fab-promo-refresh/src/styles.css`
  - shared visual system for all 5 slides
- `docs/marketing/fab-promo-refresh/src/slide-01-cover.html`
  - cover / identity slide
- `docs/marketing/fab-promo-refresh/src/slide-02-efficiency.html`
  - plugin-vs-manual workflow comparison
- `docs/marketing/fab-promo-refresh/src/slide-03-unified-syntax.html`
  - unified input box / multiple workflow slide
- `docs/marketing/fab-promo-refresh/src/slide-04-variables.html`
  - variable workflow slide
- `docs/marketing/fab-promo-refresh/src/slide-05-macros-parser.html`
  - macros and parser slide
- `docs/marketing/fab-promo-refresh/scripts/render_slides.swift`
  - local renderer that opens each HTML file at 1920x1080 and writes PNG exports

### Generated outputs
- `docs/marketing/fab-promo-refresh/output/fab-slide-01-cover.png`
- `docs/marketing/fab-promo-refresh/output/fab-slide-02-efficiency.png`
- `docs/marketing/fab-promo-refresh/output/fab-slide-03-unified-syntax.png`
- `docs/marketing/fab-promo-refresh/output/fab-slide-04-variables.png`
- `docs/marketing/fab-promo-refresh/output/fab-slide-05-macros-parser.png`

### Existing files to reference
- `Resources/Icon128.png`
  - source icon reused on the cover or branding lockups
- `docs/superpowers/specs/2026-03-31-fab-promo-refresh-design.md`
  - approved design source of truth

## Task 1: Create Marketing Workspace

**Files:**
- Create: `docs/marketing/fab-promo-refresh/README.md`
- Create: `docs/marketing/fab-promo-refresh/src/styles.css`
- Create: `docs/marketing/fab-promo-refresh/output/`

- [ ] **Step 1: Create the folder structure**

Run:
```bash
mkdir -p docs/marketing/fab-promo-refresh/src
mkdir -p docs/marketing/fab-promo-refresh/scripts
mkdir -p docs/marketing/fab-promo-refresh/output
```

Expected: the three directories exist under `docs/marketing/fab-promo-refresh/`.

- [ ] **Step 2: Write the README with render workflow and asset inventory**

Create `docs/marketing/fab-promo-refresh/README.md` with:
```md
# Fab Promo Refresh Assets

## Outputs
- fab-slide-01-cover.png
- fab-slide-02-efficiency.png
- fab-slide-03-unified-syntax.png
- fab-slide-04-variables.png
- fab-slide-05-macros-parser.png

## Render
swift scripts/render_slides.swift

## Validate
sips -g pixelWidth -g pixelHeight output/*.png
```

Expected: the README tells another engineer where sources live and how to regenerate the PNGs.

- [ ] **Step 3: Write the shared stylesheet**

Create `docs/marketing/fab-promo-refresh/src/styles.css` with these required sections:
```css
:root {
  --bg-0: #05080d;
  --bg-1: #0c1420;
  --panel: rgba(14, 28, 43, 0.92);
  --line: rgba(120, 220, 255, 0.24);
  --cyan: #77dfff;
  --cyan-strong: #3dc8ff;
  --green: #6dff9a;
  --orange: #ff8d57;
  --text: #eef7ff;
  --muted: rgba(238, 247, 255, 0.68);
}

body {
  margin: 0;
  width: 1920px;
  height: 1080px;
  font-family: "Avenir Next", "Helvetica Neue", Arial, sans-serif;
  background: radial-gradient(circle at top, #13263d 0%, var(--bg-0) 55%, #030507 100%);
  color: var(--text);
}

.canvas { position: relative; width: 1920px; height: 1080px; overflow: hidden; }
.headline { font-size: 92px; line-height: 0.95; font-weight: 700; letter-spacing: -0.03em; }
.subhead { font-size: 34px; color: var(--muted); }
.kicker { font-size: 20px; letter-spacing: 0.32em; text-transform: uppercase; color: var(--cyan); }
.panel { background: var(--panel); border: 1px solid var(--line); border-radius: 28px; box-shadow: 0 24px 60px rgba(0,0,0,0.35); }
.code { font-family: ui-monospace, SFMono-Regular, Menlo, monospace; }
```

Expected: one shared visual system exists before slide-specific layout work begins.

- [ ] **Step 4: Commit the workspace scaffold**

Run:
```bash
git add docs/marketing/fab-promo-refresh/README.md docs/marketing/fab-promo-refresh/src/styles.css
git commit -m "chore: scaffold Fab promo asset workspace"
```

Expected: one commit containing only the reusable workspace scaffold.

## Task 2: Build Slides 1 and 2

**Files:**
- Create: `docs/marketing/fab-promo-refresh/src/slide-01-cover.html`
- Create: `docs/marketing/fab-promo-refresh/src/slide-02-efficiency.html`

- [ ] **Step 1: Build the cover slide**

Create `docs/marketing/fab-promo-refresh/src/slide-01-cover.html` with:
```html
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <link rel="stylesheet" href="./styles.css">
</head>
<body>
  <div class="canvas slide-cover">
    <div class="kicker">Blueprint Workflow Accelerator</div>
    <h1 class="headline">Quick Event<br>Function Creator</h1>
    <p class="subhead">Shift + Click. Type. Create.</p>
  </div>
</body>
</html>
```

Then expand it with:
- a left-to-right 3-step visual flow
- a dialog-style syntax panel
- a result node/graph card
- four capability tags: `Functions`, `Events`, `Variables`, `Macros`
- optional use of `Resources/Icon128.png` if the lockup benefits from it

Expected: slide 1 reads as an evolved version of the old cover at thumbnail size.

- [ ] **Step 2: Build the efficiency comparison slide**

Create `docs/marketing/fab-promo-refresh/src/slide-02-efficiency.html` with:
```html
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <link rel="stylesheet" href="./styles.css">
</head>
<body>
  <div class="canvas slide-efficiency">
    <div class="kicker">Why It Matters</div>
    <h1 class="headline">From Typed Syntax<br>to Blueprint Nodes</h1>
    <p class="subhead">Short input on the left. Manual setup on the right.</p>
  </div>
</body>
</html>
```

Then expand it with:
- one short green or cyan plugin workflow column containing 3 steps
- one longer orange/red manual workflow column containing 6 to 7 steps
- a visual ratio that makes the plugin column feel obviously faster
- one syntax sample that represents the workflow without turning the slide into documentation

Expected: slide 2 preserves the strongest old-gallery sales mechanic without repeating the old event/function split.

- [ ] **Step 3: Preview slides 1 and 2 in a browser-capable renderer**

Run:
```bash
open docs/marketing/fab-promo-refresh/src/slide-01-cover.html
open docs/marketing/fab-promo-refresh/src/slide-02-efficiency.html
```

Expected: both slides load with no missing stylesheet and the largest text is readable immediately.

- [ ] **Step 4: Commit slides 1 and 2**

Run:
```bash
git add docs/marketing/fab-promo-refresh/src/slide-01-cover.html docs/marketing/fab-promo-refresh/src/slide-02-efficiency.html
git commit -m "feat: add cover and efficiency promo slides"
```

Expected: one focused commit for the first two slides.

## Task 3: Build Slides 3, 4, and 5

**Files:**
- Create: `docs/marketing/fab-promo-refresh/src/slide-03-unified-syntax.html`
- Create: `docs/marketing/fab-promo-refresh/src/slide-04-variables.html`
- Create: `docs/marketing/fab-promo-refresh/src/slide-05-macros-parser.html`

- [ ] **Step 1: Build the unified syntax slide**

Create `docs/marketing/fab-promo-refresh/src/slide-03-unified-syntax.html` with these required content blocks:
```html
<div class="kicker">Unified Entry Point</div>
<h1 class="headline">One Input Box.<br>Multiple Blueprint Workflows.</h1>
<div class="syntax-list code">
  <div>event OnHit(float Damage, AActor* Instigator)</div>
  <div>func pure CalculateDistance(FVector A, FVector B)</div>
  <div>var float Health</div>
  <div>macro DealDamage(exec In, float Power, out exec Done)</div>
</div>
```

Then add one small output cue beside each syntax row so the slide reads as workflow, not plain code.

Expected: slide 3 communicates the product expansion clearly without small-print overload.

- [ ] **Step 2: Build the variables slide**

Create `docs/marketing/fab-promo-refresh/src/slide-04-variables.html` with:
```html
<div class="kicker">Variable Workflow</div>
<h1 class="headline">Create Variables with<br>Context-Aware Shortcuts</h1>
<div class="proof-points">
  <div>Global and local variable flows</div>
  <div>Smart context behavior</div>
  <div>Optional Get / Set node generation</div>
</div>
```

Then add:
- one prominent syntax sample such as `var float Health`
- one local variable sample such as `local int32 LocalCount`
- one result area that visually shows variable creation plus Get/Set spawning

Expected: slide 4 makes variables feel like a first-class upgrade, not a footnote.

- [ ] **Step 3: Build the macros and parser slide**

Create `docs/marketing/fab-promo-refresh/src/slide-05-macros-parser.html` with:
```html
<div class="kicker">Advanced Workflow</div>
<h1 class="headline">Advanced Macros.<br>Smarter Parser.</h1>
<div class="proof-points">
  <div>Macro creation from typed syntax</div>
  <div>in / out / inout / exec pin directions</div>
  <div>Alias and parser customization</div>
</div>
```

Then add:
- one macro syntax hero example
- one compact pin-direction visualization
- one compact alias/settings callout area

Expected: slide 5 closes the set with the most advanced value story.

- [ ] **Step 4: Commit slides 3, 4, and 5**

Run:
```bash
git add docs/marketing/fab-promo-refresh/src/slide-03-unified-syntax.html docs/marketing/fab-promo-refresh/src/slide-04-variables.html docs/marketing/fab-promo-refresh/src/slide-05-macros-parser.html
git commit -m "feat: add remaining Fab promo slides"
```

Expected: a focused commit for the remaining three slide sources.

## Task 4: Build the Local PNG Renderer

**Files:**
- Create: `docs/marketing/fab-promo-refresh/scripts/render_slides.swift`

- [ ] **Step 1: Write the Swift renderer**

Create `docs/marketing/fab-promo-refresh/scripts/render_slides.swift` with these required capabilities:
```swift
import Foundation
import AppKit
import WebKit

let width = 1920
let height = 1080

struct Slide: Identifiable {
    let id: String
    let htmlPath: String
    let outputPath: String
}
```

Implement all of the following:
- a fixed slide list for the 5 HTML files
- one `WKWebView` configured to 1920x1080
- a local file loader using `loadFileURL`
- a wait until the page finishes loading
- bitmap snapshot to PNG using `takeSnapshot` or `bitmapImageRepForCachingDisplay`
- write each file to `docs/marketing/fab-promo-refresh/output/`
- print one success line per exported slide

Expected: one local renderer can regenerate all final PNGs from source files.

- [ ] **Step 2: Run the renderer**

Run:
```bash
cd docs/marketing/fab-promo-refresh
swift scripts/render_slides.swift
```

Expected:
```text
exported fab-slide-01-cover.png
exported fab-slide-02-efficiency.png
exported fab-slide-03-unified-syntax.png
exported fab-slide-04-variables.png
exported fab-slide-05-macros-parser.png
```

- [ ] **Step 3: Commit the renderer**

Run:
```bash
git add docs/marketing/fab-promo-refresh/scripts/render_slides.swift
git commit -m "feat: add local Fab promo renderer"
```

Expected: rendering logic is versioned separately from slide content.

## Task 5: Export, Verify, and Document Final Assets

**Files:**
- Modify: `docs/marketing/fab-promo-refresh/README.md`
- Create: `docs/marketing/fab-promo-refresh/output/fab-slide-01-cover.png`
- Create: `docs/marketing/fab-promo-refresh/output/fab-slide-02-efficiency.png`
- Create: `docs/marketing/fab-promo-refresh/output/fab-slide-03-unified-syntax.png`
- Create: `docs/marketing/fab-promo-refresh/output/fab-slide-04-variables.png`
- Create: `docs/marketing/fab-promo-refresh/output/fab-slide-05-macros-parser.png`

- [ ] **Step 1: Export the final PNGs**

Run:
```bash
cd docs/marketing/fab-promo-refresh
swift scripts/render_slides.swift
```

Expected: all 5 PNGs are regenerated in `output/`.

- [ ] **Step 2: Validate the output dimensions**

Run:
```bash
sips -g pixelWidth -g pixelHeight output/*.png
```

Expected: every exported PNG reports `pixelWidth: 1920` and `pixelHeight: 1080`.

- [ ] **Step 3: Do a visual pass on the exported images**

Check all 5 exports for:
- readable headline hierarchy
- no clipped shadows or cut-off panels
- no tiny copy carrying key meaning
- no unsupported claims or stray version number
- consistent accent and spacing system across the set

Expected: all 5 images read as one coherent Fab gallery.

- [ ] **Step 4: Update the README with final filenames and validation status**

Add this section to `docs/marketing/fab-promo-refresh/README.md`:
```md
## Validation
- Exported locally via `swift scripts/render_slides.swift`
- Verified dimensions with `sips`
- Reviewed slide-by-slide against `docs/superpowers/specs/2026-03-31-fab-promo-refresh-design.md`
```

Expected: anyone opening the marketing folder can see how the files were produced and checked.

- [ ] **Step 5: Commit the final assets**

Run:
```bash
git add docs/marketing/fab-promo-refresh/README.md docs/marketing/fab-promo-refresh/output/*.png
git commit -m "feat: export refreshed Fab promo images"
```

Expected: the repo contains both editable sources and finished marketplace images.

## Self-Review

### Spec coverage
- 5 slides are explicitly planned.
- Cover continuity is handled in Task 2.
- The old efficiency-comparison selling mechanic is preserved in Task 2.
- Unified syntax is given a dedicated slide in Task 3.
- Variables are given a dedicated slide in Task 3.
- Macros and parser customization are given a dedicated slide in Task 3.
- Export and validation are covered in Tasks 4 and 5.

### Placeholder scan
- No `TODO` or `TBD` markers are included.
- Output paths, commands, and slide filenames are explicit.
- Validation commands are explicit.

### Type consistency
- Slide filenames, output filenames, and renderer targets use one stable naming scheme.
- The plan consistently uses 1920x1080 output.

## Execution Choice
User already asked for direct execution in this session. Proceed with inline execution of this plan rather than stopping for a separate handoff.
