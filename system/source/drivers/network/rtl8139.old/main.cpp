#include <types.h>
#include <stdio.h>
#include <kernel.h>
#include "RTL8139Driver.h"


void run( uint32 base, int irq )
{
	RTL8139Driver *heck = new RTL8139Driver( base, irq);
		heck->Run();
	delete heck;
}




int main( int argc, char **argv[] )
{
	struct pci_cfg cfg;


	if ( search_pci(  0x1259, 0x2503, &cfg ) == 0 )  run( cfg.base[0], cfg.irq );
	if ( search_pci(  0x1743, 0x8139, &cfg ) == 0 )  run( cfg.base[0], cfg.irq );
	if ( search_pci(  0x14EA, 0xAB06, &cfg ) == 0 )  run( cfg.base[0], cfg.irq );
	if ( search_pci(  0x1039, 0x8139, &cfg ) == 0 )  run( cfg.base[0], cfg.irq );
	if ( search_pci(  0x1065, 0x8139, &cfg ) == 0 )  run( cfg.base[0], cfg.irq );
	if ( search_pci(  0x10EC, 0x8139, &cfg ) == 0 )  run( cfg.base[0], cfg.irq );
	if ( search_pci(  0x10EC, 0x8139, &cfg ) == 0 )  run( cfg.base[0], cfg.irq );

/*
	0x1259	0x2503	network/rtl8139
	0x1743	0x8139	network/rtl8139
	0x14EA	0xAB06	network/rtl8139
	0x1039	0x8139	network/rtl8139
	0x1065	0x8139	network/rtl8139
	0x10EC	0x8138	network/rtl8139
	0x10EC	0x8139	network/rtl8139
*/

	printf("%s\n","Maybe we didn't find an 8139 realtek network card, but the driver isn't running...");

	
	while (1)
	{
		go_dormant();
		release_timeslice();
	}
	
	
	return 0;
}



