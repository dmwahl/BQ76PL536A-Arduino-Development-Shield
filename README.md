=====================================
BQ76PL536A Arduino Development Shield
=====================================

Overview
=====================================
The goal of this project is to create an open source battery management system based on the TI BQ76PL536A Battery Monitor/Protection IC.

Hardware
=====================================
Initially the hardware is a single BQ76PL536A IC on a Arduino shield, for 3-6 series cells per board. Future versions will include multiple ICs and expand to allow up to 192 cells in series.

Current features:
1. Convenient Arduino shield form factor
2. Headers for chaining multiple shields together
3. Balancing resistors on board
4. External temp sensor header

Future features:
1. External balance headers to support higher charging/balancing current
2. PWM Charger control
3. Expandable up to 192 cells in series (~700V)
4. Electrically isolated communications
5. Data logging to microSD card
6. Current shutoff to protect cells from damage due to over-discharge
7. "Gas gauge" functionality

All hardware schematics and BOMs will be released as open source.

Software
=====================================
All software will be released as open source in the form of an Arduino library and example sketches.


Contact: david at evsaresexy dot com
