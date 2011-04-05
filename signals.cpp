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
#include <avr/pgmspace.h>
#include <RTClib.h>
#include <limits.h>
#include "hardware.h"
#include "config.h"
#include "signals.h"
#include "debug.h"
#include "windows.h"
#include "logger.h"

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

#ifdef FAKE_PIEZO
#include <MsTimer2.h>

unsigned long fake_piezo_msec_to_on = fake_piezo_on_at * 1000L;
unsigned long fake_piezo_msec_to_off = fake_piezo_off_at * 1000L;
volatile boolean fake_piezo_on = false;

void fake_piezo_isr(void)
{
    // This fires once per msec.  decrement the msec remaining counter, and act
    // if they hit zero.

    if ( ! fake_piezo_msec_to_on-- )
        fake_piezo_on = true;
    if ( ! fake_piezo_msec_to_off--)
    {
        fake_piezo_on = false;
        MsTimer2::stop();
    }
}

#endif

void signals_begin(void)
{
    // Do any startup stuff we need here that is related to signals
#ifdef FAKE_PIEZO
    MsTimer2::set(1,fake_piezo_isr);
    MsTimer2::start();
#endif

    // Initialize the status LED's
    set_status(no_activity);

}

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

        uint32_t now = RTC.now().unixtime();
        uint32_t time_to_next_open = ULONG_MAX;

        // Loop through the windows, see if we're in a window or if now,
        // how much longer must we wait
        int i = num_windows;
        while (i-- && !done)
        {
            if ( now > windows[i].open.unixtime() && now < windows[i].close.unixtime() )
            {
                done = true;
                time_to_next_open = 0;
            }
            else if ( now < windows[i].open.unixtime() )
                time_to_next_open = min(windows[i].open.unixtime() - now,time_to_next_open);
        }

        // wait for a little while, either until the window is due to open or
        // the longest amount of time has passed before we should look again
        unsigned long wait_length = min(time_to_next_open,window_test_period / 1000);
        delay( wait_length * 1000L );
    }
}

boolean window_open(void)
{
    // If the test mode switch is fired, the window is forced open
    boolean result = test_switch_on();

    // loop through all the windows to see if we're open now.
    uint32_t now = RTC.now().unixtime();
    int i = num_windows;
    while (i-- && !result)
    {
        if ( now > windows[i].open.unixtime() && now < windows[i].close.unixtime() )
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
#ifdef FAKE_PIEZO
    return fake_piezo_on;
#endif

#ifdef SERIAL_DEBUG_PIEZO
    static int items = 20;
    int readings = 1000;
    long value = 0;
    while ( readings-- )
        value += analogRead(piezo_pin);
    printf("%i ",value/1000);
    delay(250);
    if ( ! items-- )
    {
        printf("\n\r");
        items = 20;
    }
#endif
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

    uint8_t status_bits = 0;
    if ( test_switch_on() )
        status_bits |= B100;
    if ( window_open )
        status_bits |= B10;
    if ( cameras_firing )
        status_bits |= B1;

    log_status( status_bits );
}

void set_status(status_e status1, status_e status2)
{
    set_status(status1);
    set_status(status2);
}

void listen_for_serial_configuration(void)
{
    printf_P(PSTR("Test switch CLOSED.  Listening for configuration...  Open test switch to continue\r\n"));
    printf_P(PSTR("To set date, send yymmddD\r\nTo set time, send hhmmssT\r\n"));
    uint8_t inbuffer[25];
    uint8_t* curbuf = inbuffer;
    uint8_t* endbuf = inbuffer + 24;
    DateTime now, adjusted;
    char buf[25];
    while ( test_switch_on() )
    {
        while (Serial.available() && curbuf < endbuf)
        {
            int c = Serial.read();
            switch (c)
            {
            case 'T':
                *curbuf = 0;
                curbuf = inbuffer;

                now = RTC.now();
                adjusted = DateTime(
                               now.year(),now.month(),now.day(),
                               10 * (inbuffer[0] - '0') + inbuffer[1] - '0',
                               10 * (inbuffer[2] - '0') + inbuffer[3] - '0',
                               10 * (inbuffer[4] - '0') + inbuffer[5] - '0'
                           );
                RTC.adjust(adjusted);
                log_set_time();

                break;
            case 'D':
                *curbuf = 0;
                curbuf = inbuffer;

                now = RTC.now();
                adjusted = DateTime(
                               10 * (inbuffer[0] - '0') + inbuffer[1] - '0',
                               10 * (inbuffer[2] - '0') + inbuffer[3] - '0',
                               10 * (inbuffer[4] - '0') + inbuffer[5] - '0',
                               now.hour(),now.minute(),now.second()
                           );
                RTC.adjust(adjusted);
                log_set_time();

                break;
            case 'E':
                log_playback();
                break;
            case 'C':
                log_clear();
                break;
            default:
                *curbuf++ = c;
            }
        }
    }
}
// vim:ci:sw=4 sts=4 ft=cpp
