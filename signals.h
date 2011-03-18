/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file signals.h
 *
 * Declares the individual components of program logic.  Essentially these are the
 * "signals" thrown or received by the various hardware components.
 */

#ifndef __SIGNALS_H__
#define __SIGNALS_H__

/**
 * Status events
 *
 * Various states that pieces of the system can get into
 */
typedef enum { no_activity = 0, window_is_closed, window_is_open, cameras_are_waiting, cameras_are_firing } status_e;

/**
 * Test switch on?
 *
 * @return true if the test switch is engaged
 */
extern boolean test_switch_on(void);

/**
 * Await the opening of the window
 *
 * Block here until the next window opens
 */
extern void await_window_open(void);

/**
 * Is the window on now?
 *
 * @return true if the window is currently open
 */
extern boolean window_open(void);

/**
 * Is the sound on?
 *
 * @return true if the sound is being heard continuously
 */
extern boolean sound_is_on(void);

/**
 * Set camera pins
 *
 * @param state The specific value (LOW or HIGH) to set all the camera pins to
 */
extern void set_camera_pins(int state);

/**
 * Set LED status
 *
 * Display the program's status on the LED's.  Note that the status LED's retain
 * their value, so not all the status needs to be send every time.  Just send the
 * things that have changed.
 *
 * @param status The status event to display
 */
extern void set_status(status_e status);

/**
 * Set LED status
 *
 * Convenience function to set TWO status events at the same time
 *
 * @param status1 One status event to set
 * @param status2 Another one to set
 */
extern void set_status(status_e status1, status_e status2);

/**
 * Start listening
 *
 * Start listening for the piezo.  Call this before any calls to is_sound_on()
 */
extern void start_listening(void);

#endif // __SIGNALS_H__

// vim:ci:sw=4 sts=4 ft=cpp
