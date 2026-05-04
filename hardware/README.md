# Hardware

KiCad 8 project for the ESP32 MP3 player.

## Files

- `mp3player.kicad_pro` — KiCad project
- `mp3player.kicad_sch` — schematic (block-level overview)
- `mp3player.kicad_pcb` — board outline + mounting holes (47 × 32 mm)
- `SCHEMATIC.md` — **authoritative** netlist (use this when wiring up
  the schematic in KiCad)
- `BOM.md` — bill of materials with package types and notes

## Status

The shipped `.kicad_sch` is a block-level overview only — it loads in
KiCad 8, shows each subsystem as a labeled block, and lists the
connection summary. **Recreate the detailed schematic from
`SCHEMATIC.md`** before exporting gerbers; the netlist there is
component-accurate down to pin numbers, pull-up values, and bypass
caps.

The `.kicad_pcb` ships as a board outline (47 × 32 mm rectangle) with
4 mounting holes at corner inset 3 mm — drop in footprints from the BOM
following the placement notes at the bottom of `SCHEMATIC.md`.

## Typical workflow

1. Open `mp3player.kicad_pro` in KiCad 8.
2. Open the schematic, place each part listed in `BOM.md`, and wire it
   per `SCHEMATIC.md`.
3. Annotate, run ERC, assign footprints (most are stock KiCad libs).
4. Update PCB from schematic; arrange components per the placement
   notes; route 2-layer with a solid GND on top and pours on the
   bottom.
5. Run DRC, plot gerbers, send to fab.

## Board outline rationale

47 × 32 mm fits inside the 50 × 35 mm case interior with 1.5 mm
clearance for the wall and lip. PCB total height including the tallest
component (USB-C, ~3.6 mm above board) lands under the 9 mm case
envelope when the PCB is mounted 1.6 mm above the bottom shell floor.
