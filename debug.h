/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file debug.h
 *
 * Contains debug declarations that can/should be used everywhere.
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#undef SERIAL_DEBUG
#undef STEP_TIME
#define FAKE_CLOCK

#ifdef SERIAL_DEBUG
#define IF_SERIAL_DEBUG(x) (x)
#else
#define IF_SERIAL_DEBUG(x)
#endif

#endif // __DEBUG_H__

// vim:ci:sw=4 sts=4 ft=cpp
