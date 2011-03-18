/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file config.h
 *
 * This file contains all variables needed to configure the operation of this particular
 * device.  It should not be necessary to make changes anywhere else in the software to control
 * how the device operates.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "hardware.h"

const boolean use_piezo = true; // whether we need to wait for the sound (true) or should just skip that step (false)
const int piezo_threshold = 512; // 0-1024, how loud the sound has to be before the cameras fire
const int piezo_pulse_width = 500; // How long the sound has to be constantly above the threshhold to count, in msec

// How long to wait between checking whether we're in a launch period, in msec
const int window_test_period = 200;

// Size of camera pulses
const int num_camera_pulses = 10;
const int camera_pulse_width = 100;
const int camera_pulse_gap = 100;

// Launch windows
// ... are defined in signals.cpp right now

#endif // __CONFIG_H__

/*
How to test:

1. Launch/reset
2. Look for window open status @ 2sec
3. Press & hold fake piezo
4. Look for camera firing status after 0.5sec
5. Release fake piezo
5. Look for 10 fake camera pulses @ 100msec to continue
6. Look for window and firing status off after camera done
7. Look for window open status @ 10sec
8. Repeat piezo test as many times as possible, looking for firing status off and window status on after each time
9. Look for final window status off
10. Press and hold test button
11. Look for test mode and window status led
12. Repeat piezo test a few times
13. Release test button
14. Look for all status off
*/

// vim:ci:sw=4 sts=4
