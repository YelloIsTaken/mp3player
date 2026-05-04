// Box insert tray.
// 3D-printed tray that drops into the bottom of the printed carton and
// holds the MP3 player + a coiled USB-C cable in fixed positions.
//
// Print: PLA, 0.20 mm, 2 perimeters, 15 % infill, no supports.
// Mass:  ~6 g / unit, ~3 min to slice on most machines.

$fn = 64;

// === Inner box dimensions (must match dieline.svg) ===
BOX_W = 75;     // X
BOX_H = 55;     // Y
BOX_D = 20;     // Z (only ~half is used by the tray)

// === Tray ===
TRAY_T   = 1.2;     // wall + floor thickness
TRAY_H   = 9;       // tray height (sits in lower half, lid clears it)
SLOP     = 0.4;     // clearance against the carton walls

// === Pocket: device cradle (50 x 35 x 9 mm) ===
DEV_W   = 50.4;
DEV_H   = 35.4;
DEV_D   =  9.4;
DEV_X   = 4;        // from tray inner left
DEV_Y   = 4;        // from tray inner bottom

// === Pocket: coiled cable (round well, ~36 mm OD coil) ===
CABLE_OD = 38;
CABLE_X  = BOX_W - SLOP*2 - CABLE_OD/2 - 4;   // upper-right of tray
CABLE_Y  = BOX_H - SLOP*2 - CABLE_OD/2 - 4;
CABLE_DEPTH = 8;

// === Lift-out finger notch ===
NOTCH_W = 14;
NOTCH_D =  5;

module tray() {
    outer_w = BOX_W - SLOP*2;
    outer_h = BOX_H - SLOP*2;

    difference() {
        // tray body (solid)
        translate([0, 0, 0])
            cube([outer_w, outer_h, TRAY_H]);

        // device pocket
        translate([DEV_X, DEV_Y, TRAY_T])
            cube([DEV_W, DEV_H, DEV_D + 1]);

        // cable well (round)
        translate([CABLE_X, CABLE_Y, TRAY_T])
            cylinder(h = CABLE_DEPTH + 1, d = CABLE_OD);

        // lift-out finger notch (between the two pockets)
        translate([(outer_w - NOTCH_W)/2,
                   DEV_Y + DEV_H - 1,
                   TRAY_H - NOTCH_D + 0.01])
            cube([NOTCH_W, 4, NOTCH_D + 0.1]);

        // logo deboss on the device pocket floor (subtle, optional)
        translate([DEV_X + 4, DEV_Y + 3, 0])
            linear_extrude(0.4)
            text("mp3 player", size = 3.0,
                 font = "Liberation Sans:style=Bold");
    }
}

tray();
