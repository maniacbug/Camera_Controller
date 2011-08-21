
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

void hal_test_sw_begin( void )
{
    pinMode(test_sw_pin,INPUT);
    digitalWrite(test_sw_pin,HIGH);
}   

void hal_status_leds_begin( void )
{
    int i;
    i = num_status_led_pins;
    while (i--)
    {
        pinMode(status_led_pin[i],OUTPUT);
        digitalWrite(status_led_pin[i],led_on_value);
    }
}
void hal_camera_begin( void )
{
    i = num_camera_pins;
    while (i--)
    {
        pinMode(camera_pin[i],OUTPUT);
        digitalWrite(camera_pin[i],LOW);
    }
    pinMode(focus_pin,OUTPUT);
    digitalWrite(focus_pin,LOW);
}

void hal_rtc_begin( void )
{
    Wire.begin();
    RTC.begin();
}
void hal_serial_begin( void )
{
    Serial.begin(57600);
    Printf_setup();
}
void hal_camera_set(HIGH,camera_pulse_width)
{
     set_camera_pins(HIGH);
     delay(camera_pulse_width);
}
void hal_focus_set(HIGH,focus_delay)
{
                digitalWrite(focus_pin,HIGH);
                delay(focus_delay);
}
bool hal_test_sw_closed( void )
{
    return ( digitalRead(test_sw_pin) == switch_closed_value );
}

bool hal_inchar_ready( void )
{
  return Serial.available();
}

int hal_inchar_read(void)
{
  return Serial.read();
}
long hal_read_piezo(void)
{
    long reading = 0;
    int n = piezo_samples;
    while (n--)
        reading += analogRead(piezo_pin);
    return reading / piezo_samples;
}
void hal_status_led_set(int num, int state)
{
    digitalWrite(num,state);
}
void hal_wait_seconds( wait_length )
{
	delay( wait_length * 1000L );
}
