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
#include <Wire.h>
#include <RTClib.h>
#include <MemoryFree.h>
#include <EEPROM.h>
#include "debug.h"
#include "hardware.h"
#include "signals.h"
#include "config.h"
#include "logger.h"

/*************************************************************/

#ifdef FAKE_PIEZO
#include <MsTimer2.h>
#endif

/*************************************************************/

#ifdef TEST_WINDOWS_SHORT
#include "windows.h"

window_c short_windows[] =
{
    window_c( 2,5 ),
    window_c( 10,40 ),
    window_c( 50,60 )
};
const int num_short_windows = sizeof(short_windows)/sizeof(window_c);

window_c* windows = short_windows;
int num_windows = num_short_windows;

#elif defined(TEST_WINDOWS_LONG)

window_c long_windows[] =
{
    //          DATE     OPEN        PLANE       TIME
    window_c (  4,20,11, 5,30, 0,AM, 6,40,00,AM, 9, 0, 0,AM ),
    window_c (  4,20,11, 9, 0, 0,PM, 9,40,00,PM,10, 0, 0,PM ),
    window_c (  4,21,11, 5,30, 0,AM, 6,40,00,AM, 9, 0, 0,AM ),
    window_c (  4,21,11, 9, 0, 0,PM, 9,40,00,PM,10, 0, 0,PM ),
    window_c (  4,22,11, 5,30, 0,AM, 6,40,00,AM, 9, 0, 0,AM ),
    window_c (  4,22,11, 9, 0, 0,PM, 9,40,00,PM,10, 0, 0,PM ),
};
const int num_long_windows = sizeof(long_windows)/sizeof(window_c);

window_c* windows = long_windows;
int num_windows = num_long_windows;

#else

#include "sts134_windows.h"
window_c* windows = sts134_windows;
int num_windows = num_sts134_windows;

#endif

/*************************************************************/

// Enable 'printf'

int Serial_putc( char c, FILE *t)
{
    Serial.write( c );
}

void Printf_setup(void)
{
    fdevopen( &Serial_putc, 0);
}

/*************************************************************/

RTC_DS1307 RTC;

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
        digitalWrite(status_led_pin[i],led_on_value);
    }

    // Setup camera pins
    i = num_camera_pins;
    while (i--)
    {
        pinMode(camera_pin[i],OUTPUT);
        digitalWrite(camera_pin[i],LOW);
    }
    pinMode(focus_pin,OUTPUT);
    digitalWrite(focus_pin,LOW);

    // Setup RTC
    Wire.begin();
    RTC.begin();

    Serial.begin(57600);
    Printf_setup();
    printf_P(PSTR("Camera_Controller_3\r\nSERIAL_DEBUG enabled\r\n"));

    // Always playback the old logs
    log_playback();

    // Start a new logging session
    log_begin();

    // Test switch must be OPEN before starting.  If it's CLOSED on startup, that means we want to send configuration
    // to the unit via serial port
    if ( test_switch_on() )
        listen_for_serial_configuration();

    // Log current configuration
    uint16_t config_list[25];
    uint16_t* config_ptr = config_list;
    *config_ptr++ = 1; // config data version #
    *config_ptr++ = freeMemory();
    *config_ptr++ = (use_piezo?B1:0) | (use_focus?B10:0);
    *config_ptr++ = piezo_threshold;
    *config_ptr++ = piezo_samples;
    *config_ptr++ = num_camera_pulses;
    *config_ptr++ = camera_pulse_width & 0xffff;
    *config_ptr++ = camera_pulse_width >> 16;
    *config_ptr++ = camera_pulse_gap & 0xffff;
    *config_ptr++ = camera_pulse_gap >> 16;
    *config_ptr++ = focus_delay;
    *config_ptr++ = camera_pulse_test_width;
    *config_ptr++ = num_windows;
    *config_ptr++ = listen_before_plane_time;
    log_config(config_list,config_ptr-config_list);

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
    printf_P(PSTR("Windows:\n\r"));

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

void camera_pulses(void)
{
    int i = num_camera_pulses;
    while (i--)
    {
        set_camera_pins(HIGH);
        delay(camera_pulse_width);
        set_camera_pins(LOW);
        delay(camera_pulse_gap);
    }
}

void test_pulses(void)
{
    set_camera_pins(HIGH);
    delay(camera_pulse_test_width);
    set_camera_pins(LOW);
}

void loop(void)
{
    await_window_open();

    set_status(window_is_open);
    start_listening();
    while ( window_open() || test_switch_on() )
    {
        if ( sound_is_on() || ! use_piezo )
        {
            set_status(cameras_are_firing);

            if (use_focus)
            {
                digitalWrite(focus_pin,HIGH);
                delay(focus_delay);
            }
            
            if ( test_switch_on() )
                test_pulses();
            else
                camera_pulses();

            if (use_focus)
            {
                digitalWrite(focus_pin,LOW);
            }

            set_status(cameras_are_waiting);
            start_listening();
        }
    }
    set_status(window_is_closed);
}

// vim:ci:sw=4 sts=4 ft=cpp
