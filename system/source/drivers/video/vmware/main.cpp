#include <stdio.h>
#include <kernel.h>


#define PCI_VENDOR_ID_VMWARE 			0x15AD
#define PCI_DEVICE_ID_VMWARE_SVGA2		0x0405
#define PCI_DEVICE_ID_VMWARE_SVGA		0x0710

#include "VMWareDriver.h"
#include "svga_reg.h"


// **********************************************************

int launch_drivers()
{
	uint8 bus;
	uint8 unit;
	uint8 function;

	for(bus = 0; bus < 255; bus++) 
	{
		for(unit = 0; unit < 32; unit++) 
		{
			for(function = 0; function < 8; function++) 
			{
				struct pci_cfg my_cfg;

				if ( smk_probe_pci( bus, unit, function, &my_cfg ) != 0 ) break;

				if ( (my_cfg.vendor_id != PCI_VENDOR_ID_VMWARE ) ) continue;

				if ( ( my_cfg.device_id != PCI_DEVICE_ID_VMWARE_SVGA2 ) &&
					 ( my_cfg.device_id != PCI_DEVICE_ID_VMWARE_SVGA ) )
				{
					if(!(my_cfg.header_type & 0x80)) break;
					continue;
				}

				printf("%s%p\n","VMWare device found: ", my_cfg.device_id );
				
				int index;
				int value;


				if (  my_cfg.device_id == PCI_DEVICE_ID_VMWARE_SVGA )
				{
					index = SVGA_LEGACY_BASE_PORT + 4 * SVGA_INDEX_PORT;
					value = SVGA_LEGACY_BASE_PORT + 4 * SVGA_VALUE_PORT;
				}
				else
				{
					index = my_cfg.base[0] + SVGA_INDEX_PORT;
					value = my_cfg.base[0] + SVGA_VALUE_PORT;
				}
				
				for ( int i = 0 ; i < 6; i++ )
					printf("%s(%x,%x)\n", "base", my_cfg.base[i], my_cfg.size[i] ); 
				
				
				(new VMWareDriver( index, value ) )->Resume();

				if(!(my_cfg.header_type & 0x80)) break;
			}
		}
	}

	return -1;
}


// **********************************************************

int main(int argc, char *argv[] )
{
	printf("%s\n", "vmware - video driver.");

	smk_request_iopriv();
	
	launch_drivers();
	

	/*
	  VMWareDriver *vmware = new VMWareDriver( PCI_DEVICE_ID_VMWARE_SVGA2 );
	  				vmware->Resume();


	  int rc;
	  
	  if ( wait_thread( vmware->tid(), &rc ) != 0 )
		printf("%s: %i\n","problem waiting for thread", vmware->tid() );
	  else
	  	printf("%s%i\n","vmware video driver returned with: " , rc );
	  */

	while (1==1)
	{
		smk_sleep(60);
	}
					
	return 0;
}


