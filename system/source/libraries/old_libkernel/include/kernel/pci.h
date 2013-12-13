#ifndef _LIBKERNEL_PCI_H
#define _LIBKERNEL_PCI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct pci_cfg
{
  uint16 vendor_id;
  uint16 device_id;

  uint16 command;
  uint16 status;

  uint8 revision_id;
  uint8 interface;
  uint8 sub_class;
  uint8 base_class;

  uint8 cache_line_size;
  uint8 latency_timer;
  uint8 header_type;
  uint8 bist;

  uint8 bus;
  uint8 dev;
  uint8 func;
  uint8 irq;

  uint32 base[ 6 ];
  uint32 size[ 6 ];
  uint8 type[ 6 ];
  uint32 rom_base;
  uint32 rom_size;

  uint16 subsys_vendor;
  uint16 subsys_device;

  uint8 current_state;
};



int	smk_probe_pci( uint8 bus, uint8 unit, uint8 function, struct pci_cfg* cfg );

int smk_search_pci( uint16 vendor_id, uint16 device_id, struct pci_cfg* cfg );

#ifdef __cplusplus
}
#endif

#endif
