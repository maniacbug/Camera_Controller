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
#include "windows.h"

//
// Which camera setup is this controlling?
//
// Sky Camera?  do #define SKY_CAMERA, and #undef VIDEO_CAMERA
// Video Camera?  fo #define VIDEO_CAMERA and #under SKY_CAMERA

#undef SKY_CAMERA
#undef VIDEO_CAMERA
#define SKY_GH2_CAMERA
#undef TEST_CAMERA

#ifdef SKY_CAMERA

const bool use_piezo = true; // whether we need to wait for the sound (true) or should just skip that step (false)

// Focus pin
const bool use_focus = true;
const int focus_delay = 500; // Msec to hold focus high before shooting

// Size of camera pulses
const int num_camera_pulses = 1;
const unsigned long camera_pulse_width = 1000L*60L*20L; // 20 minutes
const unsigned long camera_pulse_gap = 0;
#endif

#ifdef SKY_GH2_CAMERA

const bool use_piezo = false; // whether we need to wait for the sound (true) or should just skip that step (false)

// Focus pin
const bool use_focus = true;
const int focus_delay = 1000; // Msec to hold focus high before shooting

// Size of camera pulses
const int num_camera_pulses = 2;
const unsigned long camera_pulse_width = 500;
const unsigned long camera_pulse_gap = 1000L*60L*20L; // don't pulse again for 20 minutes!
#endif

#ifdef VIDEO_CAMERA

const bool use_piezo = false; // whether we need to wait for the sound (true) or should just skip that step (false)

// Focus pin
const bool use_focus = false;
const int focus_delay = 500; // Msec to hold focus high before shooting

// Size of camera pulses
const int num_camera_pulses = 2;
const unsigned long camera_pulse_width = 500;
const unsigned long camera_pulse_gap = 1000L*60L*20L; // don't pulse again for 20 minutes!

#endif

#ifdef TEST_CAMERA

const bool use_piezo = true; // whether we need to wait for the sound (true) or should just skip that step (false)

// Focus pin
const bool use_focus = true;
const int focus_delay = 2000; // Msec to hold focus high before shooting

// Size of camera pulses
const int num_camera_pulses = 10;
const unsigned long camera_pulse_width = 250;
const unsigned long camera_pulse_gap = 1000;

#endif

#ifdef FAKE_PIEZO
const int fake_piezo_on_at = 20; // seconds from start of sketch
const int fake_piezo_off_at = 30; // seconds from start of sketch
#endif

const int piezo_threshold = 300; // 0-1024, how loud the sound has to be before the cameras fire
const int piezo_samples = 500; // Many samples to take and average out every time we test the piezo

// How long to start listening prior to the 'plane' time, in seconds
const uint32_t listen_before_plane_time = 2L * 60L; // 2 minutes

// How long to test the camera pulse when test switch is closed
const uint32_t camera_pulse_test_width = 5000UL;

// How long to wait between checking whether we're in a launch period, in msec
const int window_test_period = 2000;

// Launch windows

// Currently defined in PDE
extern window_c* windows;
extern int num_windows;

#endif // __CONFIG_H__

/*

******** Successful test with FAKE_PIEZO, TEST_WINDOWS, and SERIAL_DEBUG looks like this:

Camera_Controller_3
SERIAL_DEBUG enabled
Current time: Mar 22 2011 21:20:54
02: open Mar 22 2011 21:21:44 plane Mar 22 2011 21:21:49 close Mar 22 2011 21:21:54
01: open Mar 22 2011 21:21:04 plane Mar 22 2011 21:21:19 close Mar 22 2011 21:21:34
00: open Mar 22 2011 21:20:56 plane Mar 22 2011 21:20:57 close Mar 22 2011 21:20:59
Mar 22 2011 21:20:57: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:20:57: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:20:59: Test OFF Window CLOSED Cameras WAITING
Mar 22 2011 21:21:05: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:21:05: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:21:14: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:21:21: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:21:21: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:21:27: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:21:34: Test OFF Window CLOSED Cameras WAITING
Mar 22 2011 21:21:45: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:21:45: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:21:54: Test OFF Window CLOSED Cameras WAITING

Super tricky guy sez...  DS3234 has two alarms.  Could use those little guys to control the fake piezo!

******** Successful test with only TEST_WINDOWS and SERIAL_DEBUG, and never tripping the piezo:

Camera_Controller_3
SERIAL_DEBUG enabled
Current time: Mar 22 2011 21:36:02
02: open Mar 22 2011 21:36:52 plane Mar 22 2011 21:36:57 close Mar 22 2011 21:37:02
01: open Mar 22 2011 21:36:12 plane Mar 22 2011 21:36:27 close Mar 22 2011 21:36:42
00: open Mar 22 2011 21:36:04 plane Mar 22 2011 21:36:05 close Mar 22 2011 21:36:07
Mar 22 2011 21:36:05: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:36:05: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:36:07: Test OFF Window CLOSED Cameras WAITING
Mar 22 2011 21:36:13: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:36:13: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:36:42: Test OFF Window CLOSED Cameras WAITING
Mar 22 2011 21:36:53: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:36:53: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:37:02: Test OFF Window CLOSED Cameras WAITING

******** Successful test with only TEST_WINDOWS and SERIAL_DEBUG, and holding the piezo the entire time

Camera_Controller_3
SERIAL_DEBUG enabled
Current time: Mar 22 2011 21:37:55
02: open Mar 22 2011 21:38:45 plane Mar 22 2011 21:38:50 close Mar 22 2011 21:38:55
01: open Mar 22 2011 21:38:05 plane Mar 22 2011 21:38:20 close Mar 22 2011 21:38:35
00: open Mar 22 2011 21:37:57 plane Mar 22 2011 21:37:58 close Mar 22 2011 21:38:00
Mar 22 2011 21:37:58: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:37:58: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:37:58: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:04: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:04: Test OFF Window CLOSED Cameras WAITING
Mar 22 2011 21:38:06: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:06: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:06: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:12: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:12: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:18: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:18: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:24: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:24: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:31: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:31: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:37: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:37: Test OFF Window CLOSED Cameras WAITING
Mar 22 2011 21:38:46: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:46: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:46: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:52: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:52: Test OFF Window OPEN Cameras FIRING
Mar 22 2011 21:38:58: Test OFF Window OPEN Cameras WAITING
Mar 22 2011 21:38:58: Test OFF Window CLOSED Cameras WAITING

*/

// vim:ci:sw=4 sts=4
