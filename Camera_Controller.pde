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

#include <SPI.h>
#include <RTClib.h>
#include <MemoryFree.h>
#include "debug.h"
#include "hardware.h"
#include "signals.h"
#include "config.h"

/*************************************************************/

#ifdef FAKE_PIEZO
#include <MsTimer2.h>
#endif

/*************************************************************/

#ifdef TEST_WINDOWS
#include "windows.h"
window_c test_windows[] =
{
    window_c( 2,5 ),
    window_c( 10,40 ),
    window_c( 50,60 )
};
const int num_test_windows = sizeof(test_windows)/sizeof(window_c);

window_c* windows = test_windows;
int num_windows = num_test_windows;
#else
#include "sts134_windows.h"
#endif

/*************************************************************/

#ifdef SERIAL_DEBUG
#include "WProgram.h"
int Serial_putc( char c, FILE *t)
{
    Serial.write( c );
}

void Printf_setup(void)
{
    fdevopen( &Serial_putc, 0);
}
#endif

/*************************************************************/

RTC_DS3234 RTC(8);

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

    // Setup RTC
    SPI.begin();
    RTC.begin();

#ifdef SERIAL_DEBUG
    Serial.begin(9600);
    Printf_setup();
    printf_P(PSTR("Camera_Controller_3\r\nSERIAL_DEBUG enabled\r\n"));
    printf_P(PSTR("Free memory: %i\r\n"),freeMemory());
    printf_P(PSTR("\r\nConfiguration:\r\npiezo: %i thr=%i pulse=%i\r\ncamera pulses: %i wid=%lu gap=%lu\n\r"),
             use_piezo, piezo_threshold, piezo_pulse_width, num_camera_pulses,camera_pulse_width, camera_pulse_gap );

    // Test switch must be OPEN before starting.  If it's CLOSED on startup, that means we want to send configuration
    // to the unit via serial port
    if ( test_switch_on() )
    {
        listen_for_serial_configuration();
    }
#endif

#ifdef TEST_WINDOWS
    // We need to offset all the windows by the program start time,
    // while we're testing.
    DateTime program_start_time = RTC.now();
    i = num_windows;
    while(i--)
        windows[i].add(program_start_time);
#endif

#ifdef SERIAL_DEBUG
    char buf[25];
    printf_P(PSTR("\r\nCurrent time: %s\n\rWindows:\n\r"),program_start_time.toString(buf,25));

    i = num_windows;
    while(i--)
    {
        printf_P(PSTR("%02i: open %s "),i,windows[i].open.toString(buf,25));
        printf_P(PSTR("plane %s "),windows[i].plane.toString(buf,25));
        printf_P(PSTR("close %s\r\n"),windows[i].close.toString(buf,25));
    }
    printf_P(PSTR("OK\r\n"));
#endif

    // Do any setup the signals need.
    signals_begin();
}

/*************************************************************/

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
            start_listening();
        }
    }
    set_status(window_is_closed);
}

/*************************************************************/

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
