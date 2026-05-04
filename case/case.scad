// ESP32 MP3 Player — parametric case.
//
// Render order:
//   $fn = 96;
//   bottom_shell();                 // bottom half
//   translate([0, 60, 0]) top_shell();
//   translate([0,-30, 0]) button_caps();
//
// Print:
//   PLA or PETG, 0.2 mm layer, 3 perimeters, 20 % infill, no supports
//   needed for the bottom; supports for the top's button bezel are optional.

// ====== Tunables ========================================================
$fn = 96;

// Outer envelope — 50 x 35 x 9 mm, slightly thinner than a Clip Jam.
W           = 50;     // X — width
H           = 35;     // Y — height
D           =  9;     // Z — total thickness (top + bottom shells combined)

WALL        = 1.4;    // outer wall thickness
SHELL_GAP   = 0.20;   // print clearance for snap-fit lip
CORNER_R    = 3.0;    // outer corner radius
TOP_FRAC    = 0.45;   // top shell takes 45 % of total Z
LIP_H       = 1.6;    // overlap height between top and bottom

// PCB sits in the bottom shell, suspended on 4 corner posts.
PCB_W       = 47;
PCB_H       = 32;
PCB_T       = 1.0;    // PCB thickness
PCB_STANDOFF= 1.6;    // gap from inside floor to bottom of PCB
PCB_HOLE_D  = 2.2;    // M2 self-tap
PCB_HOLE_INSET = 2.0; // hole center inset from PCB edge

// Cutouts (X position is from board left edge; Y, Z from feature center).
USBC_W      = 9.2;
USBC_H      = 3.6;
USBC_X      = 25;     // centered on bottom edge

JACK_D      = 6.2;    // 3.5 mm headphone jack OD
JACK_X      = 8;      // from left edge along bottom

SD_W        = 12.5;
SD_H        = 2.2;
SD_X        = 38;     // along right edge

SLIDE_W     = 8.0;
SLIDE_H     = 2.5;
SLIDE_X     = 5;      // along top edge

// OLED window in the top shell.
OLED_W      = 26.5;
OLED_H      = 15.5;
OLED_OFF_Y  = 9;      // distance from top of case face

// Buttons on the front face. 6 caps in a 2x3 grid below the OLED.
BTN_D       = 4.5;    // cap diameter (slop fit through bezel)
BTN_PITCH_X = 9;
BTN_PITCH_Y = 8;
BTN_GRID_OFFY = -8;   // below OLED center

// ====== Shapes ==========================================================

module rrect(w, h, r) {
    hull() {
        translate([ r,         r,        0]) circle(r);
        translate([ w - r,     r,        0]) circle(r);
        translate([ r,         h - r,    0]) circle(r);
        translate([ w - r,     h - r,    0]) circle(r);
    }
}

module rrect_prism(w, h, d, r) {
    linear_extrude(height = d) rrect(w, h, r);
}

// 4 corner posts that the PCB screws into (M2 self-tap).
module pcb_posts(z0, height) {
    inset = (W - PCB_W) / 2 + PCB_HOLE_INSET;
    insety= (H - PCB_H) / 2 + PCB_HOLE_INSET;
    for (x = [inset, W - inset], y = [insety, H - insety]) {
        translate([x, y, z0]) {
            difference() {
                cylinder(h = height, d = 4.4);
                translate([0, 0, -0.1])
                    cylinder(h = height + 0.2, d = PCB_HOLE_D);
            }
        }
    }
}

// ====== Bottom shell ====================================================
BOT_H = D * (1 - TOP_FRAC);

module bottom_shell() {
    difference() {
        // outer body
        rrect_prism(W, H, BOT_H, CORNER_R);

        // inner pocket
        translate([WALL, WALL, WALL])
            rrect_prism(W - 2*WALL, H - 2*WALL,
                        BOT_H,                  // open top
                        max(CORNER_R - WALL, 0.5));

        // snap lip rebate at top edge so top shell drops in flush
        translate([WALL - SHELL_GAP, WALL - SHELL_GAP, BOT_H - LIP_H])
            rrect_prism(W - 2*(WALL - SHELL_GAP),
                        H - 2*(WALL - SHELL_GAP),
                        LIP_H + 0.1,
                        max(CORNER_R - (WALL - SHELL_GAP), 0.5));

        // --- side cutouts (centered on PCB midline z) ---
        // PCB midline absolute Z:
        pcb_z = WALL + PCB_STANDOFF + PCB_T / 2;

        // USB-C, bottom edge
        translate([USBC_X - USBC_W/2, -0.1, pcb_z - USBC_H/2 + 0.2])
            cube([USBC_W, WALL + 0.2, USBC_H]);

        // Headphone jack, bottom edge
        translate([JACK_X, -0.1, pcb_z])
            rotate([-90, 0, 0])
            cylinder(h = WALL + 0.2, d = JACK_D);

        // microSD, right edge
        translate([W - WALL - 0.1, SD_X - SD_W/2, pcb_z - SD_H/2])
            cube([WALL + 0.2, SD_W, SD_H]);

        // Power slide switch, top edge
        translate([SLIDE_X, H - WALL - 0.1, pcb_z + 0.5])
            cube([SLIDE_W, WALL + 0.2, SLIDE_H]);
    }

    // PCB support posts inside the cavity
    pcb_posts(WALL, PCB_STANDOFF);
}

// ====== Top shell =======================================================
TOP_H = D * TOP_FRAC;

module top_shell() {
    difference() {
        union() {
            // outer body
            rrect_prism(W, H, TOP_H, CORNER_R);
            // inner snap-fit tongue that sits inside the bottom shell rebate
            translate([WALL, WALL, -LIP_H])
                rrect_prism(W - 2*WALL,
                            H - 2*WALL,
                            LIP_H,
                            max(CORNER_R - WALL, 0.5));
        }

        // hollow interior
        translate([WALL, WALL, WALL])
            rrect_prism(W - 2*WALL,
                        H - 2*WALL,
                        TOP_H,
                        max(CORNER_R - WALL, 0.5));

        // OLED window: the screen face is positioned in the upper part of
        // the front, above the button grid.
        oled_cy = H - OLED_OFF_Y - OLED_H/2;
        translate([(W - OLED_W)/2, oled_cy - OLED_H/2, -0.1])
            cube([OLED_W, OLED_H, TOP_H + 0.2]);

        // Button bezel holes — 2 rows x 3 cols.
        btn_cy = oled_cy - OLED_H/2 + BTN_GRID_OFFY;
        for (col = [-1 : 1], row = [0 : 1]) {
            translate([W/2 + col * BTN_PITCH_X,
                       btn_cy - row * BTN_PITCH_Y,
                       -0.1])
                cylinder(h = TOP_H + 0.2, d = BTN_D + 0.5);
        }

        // Status LED pinhole, lower-right corner of the face
        translate([W - 5, 4, -0.1])
            cylinder(h = TOP_H + 0.2, d = 1.6);
    }
}

// ====== Button caps =====================================================
// 6 caps with a thin shaft that pokes through the bezel and presses the
// 5x5 mm tactile switch on the PCB. Print all 6 in one job.
module button_cap() {
    cap_d  = BTN_D;          // outer
    cap_h  = 1.4;            // cap dome thickness
    shaft_d= BTN_D - 1.0;
    shaft_h= TOP_H - cap_h - 0.1;
    union() {
        cylinder(h = cap_h, d = cap_d);
        translate([0, 0, cap_h - 0.01])
            cylinder(h = shaft_h, d = shaft_d);
        // a small lip under the bezel to keep the cap captive
        translate([0, 0, cap_h + shaft_h - 0.6])
            cylinder(h = 0.6, d1 = shaft_d, d2 = shaft_d + 1.0);
    }
}

module button_caps() {
    for (i = [0 : 5])
        translate([i * (BTN_D + 3), 0, 0]) button_cap();
}

// ====== Default render ==================================================
// Comment/uncomment what you want to render to STL.
bottom_shell();
translate([0, H + 8, 0]) top_shell();
translate([0, -10, 0]) button_caps();
