/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file hardware.h
 *
 * Describes the hardware interface.
 */

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <WProgram.h>

/**
 * Mode test switch
 *
 * Enters test mode when tied to ground.
 */

const int test_sw_pin = 9;
const int switch_closed_value = LOW;

/**
 * Status led's
 *
 * For providing information about the current state of the device
 */

const int status_led_pin[] = { 8, 10, 12 };  // on for test mode, listening and firing
const int num_status_led_pins = sizeof(status_led_pin)/sizeof(int);
const int led_on_value = LOW;
const int led_off_value = HIGH;

/**
 * 2-channel camera controller
 *
 * For firing the cameras.  An unlimited number of camera pins can be used.  Just
 * remember to configure pulses for them in config.h
 */

const int camera_pin[] = { 13 };
const int num_camera_pins = sizeof(camera_pin)/sizeof(int);

/**
 * 1-pin analog piezo sensor
 *
 * For hearing sound.
 */

const int piezo_pin = 1; // analog

/**
 * Real-time clock
 *
 * For maintaining the actual time which we can use to accurately open and close launch windows
 */

// Not here yet

#endif //__HARDWARE_H__

// vim:ci:sw=4 sts=4
