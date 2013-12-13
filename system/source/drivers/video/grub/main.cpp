#include <stdio.h>

#include "GrubDriver.h"


int main(int argc, char *argv[] )
{
	printf("%s\n", "grub - fakes a real driver and provides access to the LFB that grub set up.");
	
	GrubDriver *grub = new GrubDriver();
				grub->Resume();
				grub->Wait();
	
	return 0;
}


