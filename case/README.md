# 3D-printed case

Two-part snap-fit shell, ~50 × 35 × 9 mm. All dimensions are tunable at
the top of `case.scad`.

## Render

```sh
openscad -o bottom.stl -D 'render="bottom"' case.scad   # see comments
openscad -o top.stl    -D 'render="top"'    case.scad
openscad -o caps.stl   -D 'render="caps"'   case.scad
```

Or just open `case.scad` in OpenSCAD GUI, comment out two of the three
default modules at the bottom of the file, and `Render` (F6) → `Export
STL`.

## Print settings

- **Material:** PLA or PETG (PETG if you want it to survive a hot car)
- **Layer height:** 0.16–0.20 mm
- **Perimeters:** 3
- **Infill:** 20 %
- **Supports:** none for the bottom; the top's button bezel and OLED
  window are short overhangs and print fine without supports

## Assembly

1. Solder PCB, fit 4× M2×4 mm self-tapping screws through the corner
   posts in the bottom shell.
2. Drop button caps into the top shell from the inside (the lip keeps
   them captive once the PCB is screwed down).
3. Plug battery, route wire under the SD card socket.
4. Snap top shell onto bottom — the lip-and-rebate joint is friction
   fit; if too loose, increase `SHELL_GAP` to a smaller number (e.g.
   0.10) and reprint the top shell.

## Cutout reference

The bottom shell carries the I/O cutouts; the top carries the screen and
buttons.

| Edge        | Feature              | Center location       |
| ----------- | -------------------- | --------------------- |
| Bottom (Y0) | USB-C connector      | X = 25 mm             |
| Bottom (Y0) | 3.5 mm headphone     | X = 8 mm              |
| Right (XW)  | microSD slot         | Y = 38 mm (along edge) ¹ |
| Top (YH)    | Power slide switch   | X = 5 mm              |

¹ — microSD slot is on the right edge near the top corner so the card
ejector points away from the headphone jack.
