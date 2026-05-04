# Retail box

Small straight tuck-end carton for the MP3 player + cable.

## Files

| File           | Purpose                                                    |
| -------------- | ---------------------------------------------------------- |
| `dieline.svg`  | Cut + fold template, no graphics. Send to print shop.      |
| `artwork.svg`  | Print-ready artwork over the same dieline (2-colour).      |
| `insert.scad`  | Optional 3D-printed tray that holds device + cable in place. |

## Box specs

| Attribute            | Value                              |
| -------------------- | ---------------------------------- |
| Style                | Straight tuck-end                  |
| Internal dimensions  | **75 × 55 × 20 mm** (W × H × D)    |
| Flat dieline         | 200 × 91 mm                        |
| Material             | 350 gsm coated cardstock (C2S)     |
| Alternative          | 300 gsm Kraft (rougher / eco look) |
| Tuck flap            | 18 mm with 4 mm thumb notch (top)  |
| Dust flaps           | 8 mm                               |
| Glue tab             | 10 mm                              |
| Finish               | matte lamination + spot UV on logo (optional) |

## Print artwork

Two-colour design — matte black background with single accent orange
(`#FF6B35`) — keeps cost down and survives both digital and offset print.

| Panel        | Content                                                       |
| ------------ | ------------------------------------------------------------- |
| Front        | "mp3 player" wordmark, tagline, waveform, small device illustration |
| Back         | What's inside list, specs, EAN-13 barcode placeholder         |
| Side 1       | "OPEN HARDWARE · SINCE 2026" rotated -90°                     |
| Side 2       | "MADE FOR HUMANS WHO STILL CARRY MP3s" rotated +90°           |
| Top tuck     | accent-orange "mp3 player" mark                               |
| Bottom tuck  | model number, country of manufacture                          |

> Replace the placeholder barcode in `artwork.svg` with your real EAN-13
> before going to press. Most label printers (or
> [barcode.tec-it.com](https://barcode.tec-it.com)) export EAN-13 SVG.

## Bleed and safe area

| Zone           | Margin from cut |
| -------------- | --------------- |
| Bleed (colour) | + 3 mm outside  |
| Safe (text)    | − 4 mm inside   |

The artwork already extends colour to all flap edges; add a 3 mm bleed
border before exporting print PDF.

## Polish print suppliers (qty 200)

| Supplier              | URL                  | Approx. price (PLN, qty 200) | Notes                         |
| --------------------- | -------------------- | --------------------------- | ----------------------------- |
| **Packhelp**          | packhelp.pl          | ~700–800 (≈ 4 PLN/box)      | Online quote, 7-day turnaround|
| **Pakke.pl**          | pakke.pl             | ~600–750                    | Polish, fast EU delivery       |
| **Drukomat**          | drukomat.pl          | ~650–800                    | Custom dieline accepted        |
| **Pudełka.com.pl**    | pudelka.com.pl       | ~600–900                    | Lowest MOQ, Kraft strong       |

Sub-200 quantities push the per-unit cost up sharply because plate setup
amortizes badly. If you only need ~10 boxes, ask a local print shop for
**plain Kraft tuck-end + sticker label** instead.

## Ordering checklist

1. Open `artwork.svg`, hide / delete the `GuideLayer` group.
2. Add 3 mm bleed (extend colour fills past the cut path).
3. Replace the placeholder EAN with the real one.
4. Convert all text to outlines (so the press doesn't need fonts).
5. Export as **PDF/X-1a or PDF/X-4**, CMYK, no spot colours unless you
   negotiated them with the printer.
6. Send `dieline.svg` separately as the cut template (printers want this
   as a reference layer, **not** part of the print artwork).
7. Specify: 350 gsm C2S, matte lamination, qty 200, no varnish unless
   spot-UV is in the budget.

## 3D-printed insert tray (optional)

The `insert.scad` tray drops into the bottom half of the carton and
fixes the device + a coiled 30 cm USB-C cable so they don't rattle. ~6 g
of PLA, ~3 minutes to print, fits any FDM printer with a 100 × 100 mm
bed. If you'd rather skip it, swap in a die-cut 1 mm EVA foam pad — any
Polish foam supplier will laser-cut to the same outline for ~1 PLN/each
in qty 200.
