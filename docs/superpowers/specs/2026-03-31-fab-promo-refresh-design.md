# Fab Promo Refresh Design

## Summary
Refresh the Quick Event Function Creator Fab gallery from the old 4-image set to a new 5-image set that preserves the established dark Blueprint-oriented promo style while accurately reflecting the current product scope: unified syntax, variables, macros, macro pin directions, aliases, and parser customization.

## Problem
The existing Fab gallery was built around the early plugin scope:
- cover image focused on `Shift + Click. Type. Done.`
- one event workflow comparison slide
- one function workflow comparison slide
- one feature summary slide

That structure no longer matches the current plugin. The product has expanded beyond event/function creation into:
- unified typed syntax for multiple Blueprint workflows
- variable creation with global/local scope behavior
- Get/Set node spawning
- macro creation with `in`, `out`, `inout`, and `exec` directions
- alias and parser customization

If the refreshed gallery stays at the old content level, it undersells the current plugin. If it shifts too far into documentation-style slides, it loses the strong marketplace-style impact of the original visuals.

## Audience
Primary audience: Unreal Engine Blueprint users browsing Fab who want to move faster inside graphs without setting up nodes manually.

Secondary audience: technical users who care about syntax flexibility, parser behavior, and advanced macro support.

## Goals
- Preserve the recognizable dark, high-contrast Fab promo tone of the old gallery.
- Keep the first impression simple and speed-oriented.
- Show that the plugin now does more than event/function generation.
- Give variables and macros enough dedicated space to feel like real upgrades.
- Keep each slide visually distinct and useful.

## Non-Goals
- Do not turn the gallery into dense documentation.
- Do not overload slides with small body copy or long syntax tables.
- Do not rely on version numbers in the artwork until marketplace and local metadata are aligned.
- Do not change the product identity into a generic white SaaS-style layout.

## Design Direction
Chosen direction: preserve the old visual identity.

Required traits:
- dark background with Blueprint/editor mood
- cyan or blue-highlight accents with occasional supporting red/orange for contrast
- large headline-led layout
- diagram-like framing that reads quickly at thumbnail size
- stronger hierarchy than the old images so the new set feels updated, not merely repeated

## Messaging Hierarchy
The new gallery should communicate in this order:
1. This plugin lets you create Blueprint content by typing.
2. It is faster than manual node setup.
3. The workflow is now unified across functions, events, variables, and macros.
4. Variables and macros are first-class capabilities, not minor add-ons.
5. Advanced parsing and alias customization make the tool adaptable.

## Deliverables
Produce 5 landscape marketplace images:
1. Cover / identity slide
2. Efficiency comparison slide
3. Unified syntax slide
4. Variable workflow slide
5. Macro and parser slide

## Slide Plan

### Slide 1: Cover
**Role**
Establish product identity and the core interaction pattern in one glance.

**Headline direction**
`Quick Event Function Creator`

**Support line direction**
`Shift + Click. Type. Create.`

**Visual composition**
- center the product name
- show a simple three-step left-to-right flow:
  - trigger input
  - typed syntax in the dialog
  - resulting Blueprint node or graph artifact
- include a compact capability strip or tag group:
  - `Functions`
  - `Events`
  - `Variables`
  - `Macros`

**Purpose**
This slide should feel like an evolved version of the original cover, not a reinvention.

**Avoid**
- detailed feature copy
- version number badge
- too many examples on the first slide

### Slide 2: Efficiency Comparison
**Role**
Keep the strongest selling mechanic from the old gallery: the plugin compresses long manual graph-building steps into a short typed workflow.

**Headline direction**
`From Typed Syntax to Blueprint Nodes`

**Subhead direction**
`Short input on the left. Manual setup on the right.`

**Visual composition**
- left side: plugin flow with 3 concise steps
- right side: longer manual setup flow with visibly more steps
- use size, spacing, and contrast so the left side feels obviously faster

**Content scope**
This is no longer a dedicated Event slide or Function slide. It should represent the plugin as a general Blueprint acceleration tool.

**Purpose**
This slide preserves the strongest conversion pattern from the old gallery while modernizing the message.

**Avoid**
- splitting the slide into too many examples
- fine print that only reads at full size

### Slide 3: Unified Syntax
**Role**
Show that the plugin has grown from a narrow event/function helper into a unified typed-creation workflow.

**Headline direction**
`One Input Box. Multiple Blueprint Workflows.`

**Syntax examples to feature**
- `event OnHit(float Damage, AActor* Instigator)`
- `func pure CalculateDistance(FVector A, FVector B)`
- `var float Health`
- `macro DealDamage(exec In, float Power, out exec Done)`

**Visual composition**
- center or stack 3 to 4 example commands inside a prominent input panel
- pair each command with a small visual result chip, node, or graph outcome
- reinforce that these all come from the same entry point

**Purpose**
This slide bridges the old product identity to the current feature set.

**Avoid**
- dumping every supported keyword
- showing configuration details here

### Slide 4: Variables
**Role**
Give the variable workflow its own space because it is a meaningful product expansion absent from the original gallery.

**Headline direction**
`Create Variables with Context-Aware Shortcuts`

**Support points**
- `Global and local variable flows`
- `Smart context behavior`
- `Optional Get / Set node generation`

**Visual composition**
- one strong syntax sample such as `var float Health` or `local int32 LocalCount`
- a side-by-side result or mini-flow showing:
  - variable created
  - optional Get/Set node spawn
  - context-sensitive behavior
- use slightly different accent framing from slide 2 so it does not read like a duplicate comparison slide

**Purpose**
This slide makes the “newness” of the current plugin tangible.

**Avoid**
- burying the variable story as a minor bullet on a summary slide
- mixing macro details into this slide

### Slide 5: Macros and Parser
**Role**
Finish with the most advanced capabilities and the customization story.

**Headline direction**
`Advanced Macros. Smarter Parser.`

**Support points**
- `Macro creation from typed syntax`
- `in / out / inout / exec pin directions`
- `Alias and parser customization`

**Visual composition**
- highlight one macro example prominently
- visualize pin direction support using colored chips, pin labels, or flow tags
- reserve a compact area for alias/settings cues so the slide feels advanced without becoming cluttered

**Purpose**
This slide replaces the old generic `Main Features` slide with a summary that actually reflects the current high-end capabilities.

**Avoid**
- turning the slide into a settings screenshot dump
- repeating the same general benefits already stated on earlier slides

## Copy Rules
- Every slide should work at thumbnail size.
- Headline first, proof second, details third.
- Keep visible copy short enough to scan in under 3 seconds.
- Prefer 1 to 3 proof points per slide.
- Use real plugin syntax wherever possible.
- Do not include a version number in the artwork unless plugin metadata and marketplace copy are explicitly aligned first.

## Visual Rules
- Preserve the dark Blueprint-editor mood from the old gallery.
- Use one dominant accent family across the set so all 5 images feel like one campaign.
- Increase spacing and hierarchy relative to the old set; the refresh should feel more mature and less cramped.
- Use larger syntax snippets and larger labels than before.
- Keep comparison and diagram structures simple enough to read on Fab thumbnails.

## Source Truth For Claims
Claims shown in the images must stay within what is already supported in the current repo/docs:
- function and event creation from typed syntax
- variable creation including local/global behavior
- macro creation
- macro parameter directions including execution flow
- alias customization and parser settings
- support messaging can mention modern engine coverage in listing copy, but engine-version details should not dominate the image set

## Acceptance Criteria
- The set contains 5 slides with clearly distinct roles.
- Slide 1 still feels recognizably related to the original Fab cover.
- At least one slide preserves the old “plugin vs manual workflow” selling pattern.
- Variables get their own dedicated slide.
- Macros and parser customization get their own dedicated slide.
- No slide depends on tiny text to carry its message.
- No slide includes claims unsupported by the current repo or docs.

## Risks
- Overfitting to documentation language can weaken marketplace appeal.
- Repeating the comparison-slide structure too often can make the gallery feel stale.
- Adding a version number to the artwork before metadata is aligned can create inconsistency.

## Recommended Next Step
Create a concrete production plan for the 5 slides:
- exact canvas size
- headline copy options
- screenshot/mock composition method
- final asset output paths
