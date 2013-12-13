#ifndef _LIBKERNEL_PCI_H
#define _LIBKERNEL_PCI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel/inttypes.h>

struct pci_cfg
{
  uint16_t vendor_id;
  uint16_t device_id;

  uint16_t command;
  uint16_t status;

  uint8_t revision_id;
  uint8_t interface;
  uint8_t sub_class;
  uint8_t base_class;

  uint8_t cache_line_size;
  uint8_t latency_timer;
  uint8_t header_type;
  uint8_t bist;

  uint8_t bus;
  uint8_t dev;
  uint8_t func;
  uint8_t irq;

  uint32_t base[ 6 ];
  uint32_t size[ 6 ];
  uint8_t type[ 6 ];
  uint32_t rom_base;
  uint32_t rom_size;

  uint16_t subsys_vendor;
  uint16_t subsys_device;

  uint8_t current_state;
};



int	smk_probe_pci( uint8_t bus, uint8_t unit, uint8_t function, struct pci_cfg* cfg );

int smk_search_pci( uint16_t vendor_id, 
					uint16_t device_id, 
					int num,
					struct pci_cfg* cfg );

#ifdef __cplusplus
}
#endif

#endif
