/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __WINDOWS_H__
#define __WINDOWS_H__

#include <WProgram.h>
#include <RTClib.h>

enum { AM = 0, PM = 12 };

typedef int window[15];

struct window_data
{
    int date[3];
    int open[4];
    int plane[4];
    int close[4];
};

// Note that I can save 300 bytes by optimizing window_c to contain only static values and put them in progmem.
// All I need is to get a unixtime when it's needed.
struct window_c
{
    DateTime open;
    DateTime plane;
    DateTime close;

    // regular constructor, based on real data
    window_c( int m,int d,int y,int oh,int om, int os, int oa, int ph, int pm ,int ps, int pa, int ch,int cm, int cs, int ca ):
        open( y,m,d,oh+oa-((oh==12)?12:0),om,os ),
        plane( y,m,d,ph+pa-((ph==12)?12:0),pm,ps ),
        close( y,m,d,ch+ca-((ch==12)?12:0),cm,cs )
    {}

    // test constructor, send in seconds of offset from the beginning of the sketch
    window_c( int os, int cs ):
        open( os ),
        plane( (os+cs)/2 ),
        close( cs )
    {}

    // add time to the window
    void add( const DateTime& more_time )
    {
        uint32_t more_u = more_time.unixtime();
        open += more_u;
        plane += more_u;
        close += more_u;
    }
};

#endif // __WINDOWS_H__

// vim:ci:sw=4 sts=4 ft=cpp
