/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <EEPROM.h>
#include <WProgram.h>
#include <avr/pgmspace.h>
#include "hardware.h"
#include "debug.h"
#include "logger.h"

/**
 * @file logger.cpp
 *
 * Dual-purpose logger.  Logs to EEPROM and Serial, and then plays back EEPROM
 * logs for later inspection
 */

const uint16_t num_status_bits = 3;
const uint16_t timestamp_mask = 0xffff << num_status_bits;
const uint16_t status_mask = ~timestamp_mask;
const uint16_t max_timestamp = ( timestamp_mask >> num_status_bits ) - 1;
const uint16_t log_op_seenextword = 0;
const uint16_t log_op_fulltime = 1;
const uint16_t log_op_config = 2;
const uint16_t log_op_done = 3;
const uint16_t log_op_begin = 4;
const uint16_t log_op_settime = 5;

char log_status_string[] PROGMEM = "%04u %s: Test %s Window %s Cameras %s\n\r";
char log_op_fulltime_string[] PROGMEM = "%04u %s: Current time\n\r";
//char log_op_config_string[] PROGMEM = "Configuration\n\r\tMemory: %i\r\n\tpiezo: %i thr=%i pulse=%i\r\ncamera pulses: %i wid=%lu gap=%lu\n\r";
char log_op_config_string[] PROGMEM = "%04u Configuration\n\r";
char log_op_done_string[] PROGMEM = "%04u OK\n\r";
char log_op_begin_string[] PROGMEM = "%04u New run\n\r";
char log_op_settime_string[] PROGMEM = "%04u Setting time\n\r";

char log_playback_begin[] PROGMEM = "*** BEGIN LOG PLAYBACK ***\n\r";
char log_playback_end[] PROGMEM = "*** END LOG PLAYBACK ***\n\r";

uint16_t current_eeprom_address = 0;
uint32_t last_log_entry_time = 0;
char timeprintbuffer[25];

void log_end(void);
void log_write(uint16_t data);
void log_write_byte(uint8_t data);
uint16_t log_read(void);
void log_write_32(uint32_t data);
uint32_t log_read_32(void);

// Log the current status
void log_status(uint8_t status)
{
    status &= status_mask;
    DateTime now = RTC.now();
    uint32_t log_entry_time = now.unixtime();

    // if too much time has passed to represent the time since last log
    // in the allotted # of bits, then we need to first log a "fulltime"
    uint32_t timestamp = log_entry_time - last_log_entry_time;
    if ( timestamp > max_timestamp )
    {
        log_current_time();
        timestamp = 0;
    }

    last_log_entry_time = log_entry_time;

    printf_P(log_status_string,
             current_eeprom_address,
             now.toString(timeprintbuffer,25),
             (status & B100)?"HIGH":"LOW",
             (status & B10)?"HIGH":"LOW",
             (status & B1)?"HIGH":"LOW"
            );

    log_write( ( timestamp << num_status_bits ) | status);
    log_end();
}

void log_set_time(void)
{
    printf_P(log_op_settime_string,current_eeprom_address);
    log_write(timestamp_mask | log_op_settime);
    log_current_time();
}

// Log the current time
void log_current_time(void)
{
    DateTime now = RTC.now();
    last_log_entry_time = now.unixtime();

    printf_P(log_op_fulltime_string,current_eeprom_address,now.toString(timeprintbuffer,25));

    // A 'current time' log includes a 'fulltime' opcode...
    log_write(timestamp_mask | log_op_fulltime);

    // ...and the time in seconds.
    log_write_32(last_log_entry_time);

    // Always write a log_end at the end everytime, in case this ends
    // up being the last time
    log_end();
}

// Log the supplied configuration
void log_config(uint8_t* data, int len)
{
    printf_P(log_op_config_string,current_eeprom_address);

    log_write(timestamp_mask | log_op_config);
#if 0
    // Config data is just saved as single bytes with a qty preceeding it
    log_write_byte(len);
    while (len--)
        log_write_byte(*data++);
#endif
    log_end();
}

void log_end(void)
{
    log_write(timestamp_mask | log_op_done);
    current_eeprom_address -= 2;
}

void log_begin(void)
{
    log_write(timestamp_mask | log_op_begin);
    log_current_time();
}

void log_clear(void)
{
    current_eeprom_address = 0;
    printf_P(PSTR("%04i Log cleared\n\r"),current_eeprom_address);
    log_begin();
}

void log_playback(void)
{
    current_eeprom_address = 0;
    last_log_entry_time = 0;

    // Read the initial value.  If the first value is NOT a 'begin', then we don't
    // even need to play back because this is not a valid log stream.
    uint16_t value = log_read();
    boolean done = ( value != (timestamp_mask | log_op_begin) );
    while (!done)
    {
        // If the timestamp bits are full, then this is a command, not a status log
        if ( ( value & timestamp_mask ) == timestamp_mask )
        {
            uint8_t op = value & status_mask;
            switch (op)
            {
            case log_op_fulltime:
                last_log_entry_time = log_read_32();
                printf_P(log_op_fulltime_string,current_eeprom_address-6,DateTime(last_log_entry_time).toString(timeprintbuffer,25));
                break;
            case log_op_settime:
                printf_P(log_op_settime_string,current_eeprom_address-2);
                break;
            case log_op_config:
                printf_P(log_op_config_string,current_eeprom_address-2);
                break;
            case log_op_begin:
                printf_P(log_playback_begin);
                printf_P(log_op_begin_string,current_eeprom_address-2);
                break;
            case log_op_done:
                printf_P(log_op_done_string,current_eeprom_address-2);
                printf_P(log_playback_end);

                // rewind in front of the 'done' opcode, AND an extra 2 to offset the coming
                // log_read()
                // The idea is to leave the current pointer right here, so we can continue writing here.
                current_eeprom_address -= 2;
                done = true;
                break;
            }
        }
        else
        {
            last_log_entry_time += ( ( value & timestamp_mask ) >> num_status_bits );
            uint8_t status = value & status_mask;
            printf_P(log_status_string,current_eeprom_address-2,
                     DateTime(last_log_entry_time).toString(timeprintbuffer,25),
                     (status & B100)?"HIGH":"LOW",
                     (status & B10)?"HIGH":"LOW",
                     (status & B1)?"HIGH":"LOW"
                    );
        }
        value = log_read();
    }
    current_eeprom_address -= 2;
}

void log_write_32(uint32_t data)
{
    //printf_P(PSTR("log_write_32(): %04u %04lu\n\r"),current_eeprom_address,data);

    log_write(data & 0xffff); // LSW
    log_write(data >> 16); // MSW
}

void log_write(uint16_t data)
{
    //printf_P(PSTR("log_write(): %04u %04u\n\r"),current_eeprom_address,data);

    log_write_byte(data & 0xff); // LSB
    log_write_byte(data >> 8); // MSB
}

void log_write_byte(uint8_t data)
{
    EEPROM.write(current_eeprom_address++,data);
}

uint16_t log_read(void)
{
    uint16_t read_address = current_eeprom_address;
    uint16_t result = EEPROM.read(current_eeprom_address++);
    result |= EEPROM.read(current_eeprom_address++) << 8;

    //printf_P(PSTR("log_read(): %04u %04u\n\r"),read_address,result);

    return result;
}

uint32_t log_read_32(void)
{
    uint16_t read_address = current_eeprom_address;

    uint32_t result = log_read();
    result |= (uint32_t)log_read() << 16;

    //printf_P(PSTR("log_read_32(): %04u %04lu\n\r"),read_address,result);

    return result;
}

