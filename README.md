# Ethernet Tester
Arduino Sketch to for an Ethernet cable tester.

# About
The hardware is composed of an Arduino Leonardo, with a DIY shield that controls 8 GPIO output pins.

Each pin is set high consecutively, one at a time, so that each wire in a 4-pair Ethernet cable can be tested for continuity.
On the other side of the cable, a receiving device made of resistors and LEDs display which wire is being tested.
If an LED does not light up during the sequence, it means there's a break in the wire. If more than one LED are lit up at any point, it means that there's a short between two wires.

The sketch also handles 3 buttons, for automatic sequence cycling or for manual cycling (with previous / next buttons) to troubleshoot faulty cables in detail.
