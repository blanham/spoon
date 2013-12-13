#include <types.h>
#include <kernel.h>





void wait_for_output()
{
       while(inb(0x64) & 0x2);
}


void set_leds( uint32 leds )
{
       wait_for_output();
       outb(0x60, 0xed);
       wait_for_output();
       outb(0x60, leds);
}



uint8 ReadChar()
{

    while (1)
    {
		if ( (inb(0x64) & 0x01) != 0 )
		    return inb(0x60);
    }

    return 0xFF;
}



