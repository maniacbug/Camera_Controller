# RTC-driven Camera Controller

## What does it do?

This sketch drives a time- and noise-sensitive camera-controlling system.

It maintains a set of time windows during which the system is in operation.
It monitors a real-time clock for precision and persistence.  During an open
window, it listens for sustained loud noise on a piezo sensor to trigger the
cameras.  When sufficient noise is heard, it triggers a configurable
series of pulses to any number of cameras.

## Installation 

Download the latest archive into your sketch folder.  You'll also need certain
libraries To get them, open your sketch folder in a terminal window and type 
these commands:

mkdir -p libraries
cd libraries
git clone git://github.com/maniacbug/RTClib.git 
git clone git://github.com/maniacbug/MemoryFree.git
wget http://www.arduino.cc/playground/uploads/Main/MsTimer2.zip && unzip MsTimer2.zip

The last library is optional, and only needed if you ever #define FAKE_PIEZO. _.
That is used for testing the operation of code that relies on the Piezo when there
is none.

After installation, be sure to set the time on the clock!  See 'Diagnostic
Operation' below for details.

## Hardware Description: hardware.h 

The hardware.h file describes the layout of all the pins on the board.

## Behaviour Configuration: config.h

The config.h file controls how the software behaves.  Currently there is one #ifdef
section for each kind of camera.  At the top, ensure the right camera is #ifdef'd
and the others are #undef'd.

## Normal Operation 

To run in normal operation, ensure the test switch is OPEN, and power on the 
Arduino.  The three status lights will come on for a second or so, and then 
turn off.  The unit is now operating normally.

## EEPROM Logging

A continuous log of operation is kept in the EEPROM.  This is useful for 
understanding what happened in the field.  Every time the unit is powered on,
the entire log is printed to the Serial connection.  If users encounter any
problems when using the unit, they should always plug the unit back into
the computer, capture the logs, and send them for debugging.

## Diagnostic Operation

To run in disagnostic operation, CLOSE the test switch and power the unit on.
The three status lights will stay on.  In this mode, the unit will accept
input on the Serial connection to further configure it.  These commands are
available:

110401D		Sets the date to April 1st, 2011
184559T		Sets the time to 18:45:59.  Time is in 24-hour time.
C		Clears the log buffer
E		Plays the log buffer back again

When you're done, open the test switch, and the unit will proceed into normal
operation.

## Debugging Operation: debug.h

Many debugging constants are included in debug.h.  These should not normally be 
needed, but are included to troubleshoot various problems.
