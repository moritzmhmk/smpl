# smpl
minimal grbl alternative for DLP 3D printers

## Why not grbl for DLP?

grbl is a great piece of software for running CNC milling machines (or laser cutters).

It is however not designed for DLP printers.

For a DLP printer it makes sense to have home switches that are not limit switches (e.g. for shutters or tilt mechanisms) or home switches in addition to limit switches (e.g. on the z-axis).

Having a combined limit/home switch on the z-axis would require it to move further than in normal operation which is impossible with most designs.

### TL;DR
With grbl a switch stops the machine. With smpl you can have seperate home switches that are just for homing.

## supported subset of G code

smpl only supports a tiny subset of g code (grbl, Marlin, etc. support a way larger subset) that should be just enough to get a DLP printer running.

* G0 (set motion mode to rapid)
* ~~G1 (set motion mode to linear)~~ planned feature
* G4 P3 (dwell for 3ms)
* ~~G20 (set units to inch)~~ planned feature
* G21 (set units to mm)
* G28 Z (home Z axis (when no axis is specified home all axes))
* G90 (set distance mode to absolute)
* G91 (set distance mode to incremental)
* G92 A12 (set position of axis (without movement i.e. set the offset))
* M17 (enable steppers)
* M18 (disable steppers)
* M114 (print current positions)
* M119 (print current limit/home switch states)

