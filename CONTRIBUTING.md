# CONTRIBUTING — FMC-320U

A unified Firmware, Hardware, Product docs, and Verification that keeps strong traceability.

## Repository map (traceability anchors)

These paths are considered “traceability anchors” and must stay consistent:

- `COMPATIBILITY_HW_FW`
- `hardware/.../CHANGELOG.md` — hardware technical history per revision.
- `verification/TR-*.md` — executed test-run evidence per release/hardware.
- `hardware/PCBA/hardware_variant_impact_matrix` — requirements/use-cases/policies (product behavior).

## Branching model (kept as-is)

- `develop` is the integration branch for ongoing work.
- `main` is the released/stable branch. Only release/hotfix merges land here.
- Feature/bugfix work branches off `develop`.
- Urgent production fixes may branch off `main` as `hotfix/*`.

We use tags for releases (example: `v1.1.11`). Tags are created on `main`.

## Branch naming (simple and consistent)

Use one of the mandatory purpose prefixes:

- `feat/` new functionality
- `fix/` bug fix (non-urgent)
- `hotfix/` urgent fix for `main`
- `refactor/` internal change (no intended functional change)
- `perf/` performance/power optimization
- `test/` validation / experiments
- `docs/` documentation-only
- `release/` release preparation

Add the discipline in the branch name for clarity:

Preferred:
- `feat/firmware-logger-quota`
- `fix/hardware-connector-pinout`
- `docs/product-logging-policy`
- `test/verification-endurance`

Rule of thumb: branch name = purpose + discipline + short description.

## Commit messages (kept, with minimal metadata)

Commit header format (required):

`[AREA] type(scope): imperative message`

Allowed AREA tags:
- `[FIRMWARE] [HARDWARE] [MECHANICAL] [PRODUCT] [DOCS] [TEST] [CI] [BUILD] [SECURITY] [RELEASE]`

Allowed `type`:
- `feat fix refactor perf docs test chore`

Examples:
- `[HARDWARE] feat(pcb): add REV-C layout for enclosure MODEL-B`
- `[FIRMWARE] fix(uart3): handle DMA idle stall`
- `[DOCS] docs(requirements): clarify logging policy thresholds`

Minimal required metadata (must appear in the commit body OR PR description):
- `Issue:` an issue/ticket ID, or `NONE`
- `HW_REV:` if hardware is affected (e.g., `FMC-320-A1 REV-B`)
- `TR:` if the change is validated by a test run (e.g., `TR-2026-02-21_v1.1.11_hwA`)


## Pull Requests (PRs)

All merges into `develop`/`main` are done via PR (or reviewed fast-forward where applicable).

PR title template (recommended):
`[AREA] type(scope): short summary (Issue: FMC-123)`

PR description must include:
- What changed and why (2–5 lines)
- How it was verified (test, measurement, bench steps)
- Traceability metadata (Issue / HW_REV / TR as applicable)
- Changelog impact (which changelog was updated)

Minimum PR checklist:
- [ ] `Issue:` included (or `NONE`)
- [ ] Verification described (even if “not applicable”)
- [ ] If hardware changed: update `COMPATIBILITY_HW_FW`
- [ ] If this is a release candidate: add `verification/TR-*.md`

## Hardware revisions (REV) — when to create a new one

A new HW revision (REV) must be created when a change affects any of:
- Electrical behavior, netlist, or PCB layout (routing/stackup/critical placement)
- Connectors/pinout/test points
- Power architecture or safety/EMC relevant circuitry
- Mechanical fit (mounting holes, enclosure interfaces) that impacts integration
- Component changes that require re-qualification (timing, accuracy, power, availability)

BOM-only substitutions MAY remain the same REV only if form/fit/function is unchanged and validation shows no measurable impact. When in doubt: bump REV.

### How to record a new HW revision (minimal, non-bureaucratic)

1) Create a revision folder (recommended structure):
- `hardware/<HW_ID>/REV-A/`
- `hardware/<HW_ID>/REV-B/`

2) Add a small manifest file in the revision folder:
- `hardware/<HW_ID>/REV-B/manifest.yml`

Minimal `manifest.yml` example:
```yml
hw_id: FMC-320-A1
rev: B
date: 2026-02-21
derived_from: REV-A
summary: "Layout update for enclosure MODEL-B; added BTN3 footprint"
notes: "No schematic changes; re-qualified sleep current"


Release Workflow (Firmware + Verification + Tagging)

This section defines the official release process for FMC-320U.
It aligns firmware freeze, verification evidence (verification/TR-*.md), tagging, and traceability.

This workflow is mandatory for every official release.

1. Overview

Release flow: feature/* → develop → release/<version> → main (tag) → develop
Where: develop = integration branch
release/<version> = frozen candidate under validation

main = released/stable state
Tag is created only on main

2. Creating a Release Branch

When develop reaches a stable state:
git checkout develop
git pull
git checkout -b release/<version>

Example:
git checkout -b release/1.2.0

From this moment:
No new features are allowed.
Only critical fixes are permitted.
Firmware version must be updated in code (if applicable).

Commit example:
[RELEASE] chore(version): prepare firmware 1.2.0 for validation

3. Firmware Freeze and Validation Candidate
The release/<version> branch represents the validation candidate.

Before starting verification:
Ensure the firmware builds cleanly.
Confirm reproducible build.
Confirm compatibility with declared hardware revision.
Confirm COMPATIBILITY_HW_FW is updated if required.
The commit being validated must be clearly identifiable.

4. Verification and Test Evidence (TR)
All validation must follow the official test definition (use cases, behavior, etc.).

Executed validation must be documented as:
verification/TR-<date>_<version>_<hw>.md

Example:
verification/TR-2026-03-01_v1.2.0_hwA.md

This TR file must include:
Firmware version
Hardware revision
Git commit hash validated
Test coverage summary
PASS / FAIL summary
Blocking vs non-blocking classification
Final decision (RELEASED / CONDITIONAL / BLOCKED)

Important:
The commit hash recorded must correspond exactly to the firmware binary that was compiled and validated.
TR files are evidence, not test definitions.

5. Decision Rules
A release may be:
RELEASED → no blocking failures
CONDITIONAL → known minor issues accepted
BLOCKED → at least one blocking issue or insufficient coverage

Blocking issues include (non-exhaustive):
Boot failure
Data corruption
Loss of pulses
Electrical spec violation
Safety-relevant malfunction
Major functional regression

If BLOCKED:
Fixes must be committed in release/<version>
Re-test affected scope
Update TR accordingly

6. Merge to Main and Tagging
If decision is RELEASED:

Merge release branch into main:
git checkout main
git merge release/<version>

Create annotated tag on main:
git tag -a v<version> -m "FMC-320U v<version>"
git push origin main --tags

Example:
git tag -a v1.2.0 -m "FMC-320U v1.2.0"

Tag rules:
Tags are created only on main.
Tag must reference the validated commit.
Tag version must match firmware version reported in TR.

7. Merge Back to Develop
After tagging:
git checkout develop
git merge release/<version>
This prevents divergence between main and develop.

Then delete the release branch:
git branch -d release/<version>
8. Traceability Requirements

For every release, the following must be traceable:
Firmware version
Hardware revision
Git commit hash
Git tag
TR file
The TR file must match the tagged commit.

No release is valid without:
A corresponding TR-*.md
A tag on main

9. Post-Release Rules
After a tag is created:
Do not modify the TR file of that release.
Do not alter verification evidence retroactively.
Any correction requires a new release version.

Example:
v1.2.0  → original release
v1.2.1  → fix after release
Never rewrite history of released versions.