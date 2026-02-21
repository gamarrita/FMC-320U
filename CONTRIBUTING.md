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
