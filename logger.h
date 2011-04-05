/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <inttypes.h>

void log_status(uint8_t status);
void log_current_time(void);
void log_config(uint16_t* data, int len);
void log_set_time(void);

void log_playback(void);
void log_begin(void);
void log_clear(void);

#endif // __LOGGER_H__
