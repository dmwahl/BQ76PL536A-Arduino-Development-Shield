=====================================
BQ76PL536A Arduino Development Shield
=====================================

Overview
=====================================
The goal of this project is to create an open source battery management system based on the TI BQ76PL536A Battery Monitor/Protection IC. The initial hardware revision is in the form of an Arduino shield to facilitate software development.

Hardware
=====================================
Initially the hardware is a single BQ76PL536A IC on a Arduino shield, for 3-6 series cells per board. Future versions will include multiple ICs and expand to allow up to 192 cells in series.

Current features:
* Convenient Arduino shield form factor
* Headers for chaining multiple shields together
* Balancing resistors on board
* External temp sensor header

Future features:
* External balance headers to support higher charging/balancing current
* PWM Charger control
* Expandable up to 192 cells in series (~700V)
* Electrically isolated communications
* Data logging to microSD card
* Current shutoff to protect cells from damage due to over-discharge
* "Gas gauge" functionality

All hardware schematics and BOMs will be released as open source.

Software
=====================================
All software will be released as open source in the form of an Arduino library and example sketches.


Contact: david at evsaresexy dot com or http://www.evsaresexy.com
