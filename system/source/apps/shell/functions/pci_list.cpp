#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../flist.h"
#include "pci_list.h"

#include <pci_list.h>

#include <spoon/app/ConsoleApplication.h>

struct function_info info_pci_list = 
{
   "scan the PCI buses for devices",
   "pci_list\n"
   "This will scan the PCI devices and list them. ",
   pci_list_main
};

char *pci_vendor_name( unsigned int id )
{
	for ( unsigned int i = 0; i < PCI_VENTABLE_LEN; i++ )
	{
		if ( PciVenTable[i].VenId == id ) 
			return PciVenTable[i].VenFull;
	}
	
	return "unknown";
}

 int pci_list_main( int argc, char **argv )
 {
	int count = 0;
		 
	for(int bus = 0; bus < 255; bus++) 
	{
		for(int unit = 0; unit < 32; unit++) 
		{
			for(int function = 0; function < 8; function++) 
			{
				struct pci_cfg my_cfg;

				if ( smk_probe_pci( bus, unit, function, &my_cfg ) != 0 ) break;

					if ( count++ == 5 )
					{
						printf("%s\n","Press any key to continue...");
						((ConsoleApplication*)application)->getch();
						count = 0;
		 			}


				for ( unsigned int i = 0; i < PCI_DEVTABLE_LEN; i++ )
				{
					if ( (PciDevTable[i].VenId == my_cfg.vendor_id) &&
						 (PciDevTable[i].DevId == my_cfg.device_id) )
					{
						printf("(%i:%i:%i) %s\n",
								bus, unit, function,
								pci_vendor_name( my_cfg.vendor_id )
							  );

						printf("     (%s) %s. IRQ %i\n",
								PciDevTable[i].Chip,
								PciDevTable[i].ChipDesc,
								my_cfg.irq );

						printf("     BASE: %p,%p,%p,%p,%p,%p\n",
									my_cfg.base[0],
									my_cfg.base[1],
									my_cfg.base[2],
									my_cfg.base[3],
									my_cfg.base[4],
									my_cfg.base[5] );
					}
				}
				
			}
		}
	}

   
   return 0;
 }
 
