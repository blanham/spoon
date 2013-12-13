#include <types.h>
#include <kernel.h>



/**  \defgroup PCI  PCI Interfacing  
 *
 */


/** @{ */

int	smk_probe_pci( uint8 bus, uint8 unit, uint8 function, struct pci_cfg* cfg )
{
	return _sysenter( (SYS_PCI|SYS_TWO) , bus, unit, function, (uint32)cfg, 0 );
}



int smk_search_pci( uint16 vendor_id, uint16 device_id, struct pci_cfg* cfg )
{
  return _sysenter( ( SYS_PCI | SYS_ONE ), vendor_id, device_id, ( uint32 )cfg, 0, 0 );
}


/** @} */


