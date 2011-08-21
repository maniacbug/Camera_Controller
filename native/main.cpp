#include <WProgram.h>
#include "hal.h"
#include <stdio.h>

int main(void)
{
	setvbuf(stdin,NULL,_IONBF,0);
	init();

	setup();
  
	bool done = false;
	do
	{
		done = hal_loop();
	}
	while (!done);

	return 0;
}

