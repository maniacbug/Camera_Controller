/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file signals.cpp
 *
 * Contains the individual components of program logic.  Essentially these are the
 * "signals" thrown or received by the various hardware components.
 */

#include <WProgram.h>
#include <limits.h>
#include "hardware.h"
#include "config.h"
#include "signals.h"
#include "debug.h"

#ifdef FAKE_CLOCK
typedef unsigned long time_t;
const time_t time_t_max = ULONG_MAX;

time_t time_now(void)
{
    return millis();
}

struct window
{
    time_t open;
    time_t close;
};

window windows[] = { { 1000,5000 }, { 10000,25000 } };
const int num_windows = sizeof(windows)/sizeof(window);
#endif

boolean test_switch_on(void)
{
    return ( digitalRead(test_sw_pin) == switch_closed_value );
}

void await_window_open(void)
{
    boolean done = false;

    // Keep doing this until we actually get in a window
    while (!done)
    {
        // If the test mode switch is fired, the window is forced open
        done = test_switch_on();
        
        time_t now = time_now();
        time_t time_to_next_open = time_t_max;
        
        // Loop through the windows, see if we're in a window or if now, 
        // how much longer must we wait        
        int i = num_windows;
        while (i-- && !done)
        {
            if ( now > windows[i].open && now < windows[i].close )
            {
                done = true;
                time_to_next_open = 0;
            }
            else if ( now < windows[i].open )
                time_to_next_open = min(windows[i].open - now,time_to_next_open);
        }
        
        // wait for a little while, either until the window is due to open or
        // the longest amount of time has passed before we should look again
        unsigned long wait_length = min(time_to_next_open,window_test_period);
        delay( wait_length );
    }
}

boolean window_open(void)
{
    // If the test mode switch is fired, the window is forced open
    boolean result = test_switch_on();

    // loop through all the windows to see if we're open now.
    time_t now = time_now();
    int i = num_windows;
    while (i-- && !result)
    {
        if ( now > windows[i].open && now < windows[i].close )
        {
            result = true;
            break;
        }
    }

    return result;
}

boolean piezo_on = false;
boolean piezo_previous = false;
unsigned long piezo_edge_time = 0;

void start_listening(void)
{
    piezo_previous = false;
    piezo_on = false;
}

// Debounce code from http://www.ladyada.net/learn/sensor/tilt.html

boolean sound_is_on(void)
{
    boolean reading = analogRead(piezo_pin) > piezo_threshold;

    // If the switch changed, due to bounce or pressing...
    if (reading != piezo_previous)
    {
        // reset the debouncing timer
        piezo_edge_time = millis();
    }

    if ((millis() - piezo_edge_time) > piezo_pulse_width)
    {
        // whatever the switch is at, its been there for a long time
        // so lets settle on it!
        piezo_on = reading;
    }
    // Save the last reading so we keep a running tally
    piezo_previous = reading;

    return piezo_on;
}

void set_camera_pins(int state)
{
    int i = num_camera_pins;
    while (i--)
        digitalWrite(camera_pin[i],state);
}

void set_status(status_e status)
{
    static boolean window_open;
    static boolean cameras_firing;

    switch (status)
    {
    case window_is_closed:
        window_open = false;
        break;
    case window_is_open:
        window_open = true;
        break;
    case cameras_are_waiting:
        cameras_firing = false;
        break;
    case cameras_are_firing:
        cameras_firing = true;
        break;
    }
    digitalWrite(status_led_pin[0],test_switch_on()?led_on_value:led_off_value);
    digitalWrite(status_led_pin[1],window_open?led_on_value:led_off_value);
    digitalWrite(status_led_pin[2],cameras_firing?led_on_value:led_off_value);
}

void set_status(status_e status1, status_e status2)
{
    set_status(status1);
    set_status(status2);
}

// vim:ci:sw=4 sts=4 ft=cpp
