/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @mainpage RTC-driven Camera Controller
 *
 * This sketch drives a time- and noise-sensitive camera-controlling system.
 *
 * It maintains a set of time windows during which the system is in operation.
 * It monitors a real-time clock for precision and persistence.  During an open
 * window, it listens for sustained loud noise on a piezo sensor to trigger the
 * cameras.  When sufficient noise is heard, it triggers a configurable
 * series of pulses to any number of cameras.
 */

/**
 * @file Camera_Controller.pde
 *
 * This is the main file of the sketch.  It contains only the setup and loop logic.
 */

#include "debug.h"
#include "hardware.h"
#include "signals.h"
#include "config.h"

void setup(void)
{
    // Setup switch
    pinMode(test_sw_pin,INPUT);
    digitalWrite(test_sw_pin,HIGH);

    // Setup status LED's
    int i;
    i = num_status_led_pins;
    while (i--)
    {
        pinMode(status_led_pin[i],OUTPUT);
        digitalWrite(status_led_pin[i],led_off_value);
    }

    // Setup camera pins
    i = num_camera_pins;
    while (i--)
    {
        pinMode(camera_pin[i],OUTPUT);
        digitalWrite(camera_pin[i],LOW);
    }

#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    Printf_setup();
    printf("Camera_Controller_3\r\nSERIAL_DEBUG enabled\r\n");
#endif
}

void loop(void)
{
    await_window_open();

    set_status(window_is_open, cameras_are_waiting);
    start_listening();
    while ( window_open() || test_switch_on() )
    {
        if ( sound_is_on() || ! use_piezo )
        {
            set_status(cameras_are_firing);

            int i = num_camera_pulses;
            while (i--)
            {
                set_camera_pins(HIGH);
                delay(camera_pulse_width);
                set_camera_pins(LOW);
                delay(camera_pulse_gap);
            }
            set_status(cameras_are_waiting);
        }
    }
    set_status(window_is_closed);
}

/**
 * Alternate loop() to test the hardware test rig
 */
void rig_test(void)
{
    static uint8_t counter;
    static int pin2_state;

    digitalWrite(status_led_pin[0],digitalRead(test_sw_pin));
    digitalWrite(status_led_pin[1],analogRead(piezo_pin)<512);
    digitalWrite(status_led_pin[2],pin2_state);
    digitalWrite(camera_pin[0],pin2_state^HIGH);

    if ( ! counter++ )
        pin2_state ^= HIGH;

    delay(5);
}

// vim:ci:sw=4 sts=4 ft=cpp
