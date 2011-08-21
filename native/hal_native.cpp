#include <WProgram.h>
#include <RTClib.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>

unsigned long rtc_offset_seconds = - ( 7L * 60L * 60L ) ;

bool test_switch_closed =  true;
bool piezo_activated = false;
bool program_done = false;

/*************************************************************/
bool hal_internal_select(void)
{
   fd_set rfds;
   struct timeval tv;
   int retval;

   /* Watch stdin (fd 0) to see when it has input. */
   FD_ZERO(&rfds);
   FD_SET(0, &rfds);

   /* Wait up to 10 msec */
   tv.tv_sec = 0;
   tv.tv_usec = 500;

   retval = select(1, &rfds, NULL, NULL, &tv);
   /* Don't rely on the value of tv now! */

   if (retval == -1)
       perror("select()");

   return retval > 0;
}

void hal_internal_keyhandler(void)
{
  if ( hal_internal_select() )
  {
    char c = getchar();
    // SPACE: Toggle test switch
    if ( c == ' ' )
    {
      test_switch_closed = ! test_switch_closed;
      printf("HAL: Space key test switch now %s\n",test_switch_closed?"closed":"open");
    }
    else if ( c == '.' )
    {
      piezo_activated = ! piezo_activated;
      printf("HAL: Period key piezo now %s\n",piezo_activated?"active":"silent");
    }
    else if ( c == 'q' )
    {
      program_done = true;
      printf("HAL: q key QUIT\n");
    }
  }
}


// Enable 'printf'

void Printf_setup(void)
{
//    fdevopen( &Serial_putc, 0);
}

void hal_test_sw_begin( void )
{
//    pinMode(test_sw_pin,INPUT);
//    digitalWrite(test_sw_pin,HIGH);
}   

void hal_status_leds_begin( void )
{
#if 0
    int i;
    i = num_status_led_pins;
    while (i--)
    {
        pinMode(status_led_pin[i],OUTPUT);
        digitalWrite(status_led_pin[i],led_on_value);
    }
#endif
}
void hal_camera_begin( void )
{
#if 0
    i = num_camera_pins;
    while (i--)
    {
        pinMode(camera_pin[i],OUTPUT);
        digitalWrite(camera_pin[i],LOW);
    }
    pinMode(focus_pin,OUTPUT);
    digitalWrite(focus_pin,LOW);
#endif
}

void hal_rtc_begin( void )
{
#if 0
    Wire.begin();
    RTC.begin();
#endif
}
void hal_serial_begin( void )
{
#if 0
    Serial.begin(57600);
    Printf_setup();
#endif
}
void hal_camera_set(int level,int duration)
{
#if 0
     set_camera_pins(HIGH);
     delay(camera_pulse_width);
#endif
  usleep(duration);
}
void hal_focus_set(int level, int duration)
{
#if 0
                digitalWrite(focus_pin,HIGH);
                delay(focus_delay);
#endif
  usleep(duration);
}
bool hal_test_sw_closed( void )
{
#if 0
    return ( digitalRead(test_sw_pin) == switch_closed_value );
#endif
  hal_internal_keyhandler();
  return test_switch_closed;
}

bool hal_inchar_ready( void )
{
#if 0
  return Serial.available();
#endif
}

int hal_inchar_read(void)
{
#if 0
  return Serial.read();
#endif
}
long hal_read_piezo(void)
{
#if 0
    long reading = 0;
    int n = piezo_samples;
    while (n--)
        reading += analogRead(piezo_pin);
    return reading / piezo_samples;
#endif
  hal_internal_keyhandler();
  return piezo_activated ? 1024 : 0 ;
}
void hal_status_led_set(int num, int state)
{
#if 0
    digitalWrite(num,state);
#endif
}
void hal_wait_seconds( int wait_length )
{
#if 0
	delay( wait_length * 1000L );
#endif
  hal_internal_keyhandler();
  //printf("HAL: wait_seconds / getchar() = %02x\n",getchar());
}

unsigned long millis(void)
{
}

void init(void)
{
  printf("\nHAL is simulating Camera Controller Board V1\n");
}

DateTime hal_rtc_now(void)
{
  return DateTime(time(NULL) + rtc_offset_seconds);
}

void hal_rtc_set(DateTime&)
{
}

bool hal_loop(void)
{
  loop();
  hal_internal_keyhandler();

  return program_done;
}

// vim:cin:ai:sw=2 sts=2 ft=cpp
