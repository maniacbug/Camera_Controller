#include <WProgram.h>
#include <RTClib.h>

void hal_test_sw_begin( void );
void hal_status_leds_begin( void );
void hal_camera_begin( void );
void hal_rtc_begin( void );
void hal_serial_begin( void );
void hal_camera_set(int,int);
void hal_focus_set(int,int);
bool hal_test_sw_closed( void );
bool hal_inchar_ready( void );
bool hal_inchar_read( void );
long hal_read_piezo(void);
void hal_status_led_set(int num, int state);
void hal_wait_seconds( int wait_length );
DateTime hal_rtc_now(void);
void hal_rtc_set(DateTime&);
bool hal_loop(void);
