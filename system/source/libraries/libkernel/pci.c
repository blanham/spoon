#include <kernel.h>



/**  \defgroup PCI  PCI Interfacing  
 *
 */


/** @{ */

int	smk_probe_pci( uint8_t bus, 
					uint8_t unit, 
					uint8_t function, 
					struct pci_cfg* cfg )
{
	return _sysenter( (SYS_PCI|SYS_TWO) , 
						bus, unit, function, (uint32_t)cfg, 0 );
}



int smk_search_pci( uint16_t vendor_id, 
					uint16_t device_id, 
					int num,
					struct pci_cfg* cfg )
{
  return _sysenter( ( SYS_PCI | SYS_ONE ), 
				  	vendor_id, device_id, num, ( uint32_t )cfg, 0 );
}


/** @} */


