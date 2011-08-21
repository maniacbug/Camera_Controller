#define PSTR(x) (x)
#define printf_P printf
#define HIGH 1
#define LOW 0
#include <stdio.h>
#include <inttypes.h>
#define PROGMEM
#define min(a,b) (((a)<(b))?(a):(b))
#define prog_char char
unsigned long millis(void);
void setup(void);
void loop(void);
void init(void);
