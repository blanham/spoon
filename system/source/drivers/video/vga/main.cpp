#include <kernel.h>
#include <stdio.h>
#include "tauron.h"

int main(int argc, char* argv[])
{
	smk_request_iopriv();

	smk_disable_interrupts();
	SetVideoMode( MODE13H );
	Clear13H(0);
	ModeTest();
	smk_enable_interrupts();
	smk_sleep(10);
	SetVideoMode( MODE03H );	

	while (1==1) {}
}


