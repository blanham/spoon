#ifndef _KERNEL_PCI_H
#define _KERNEL_PCI_H

#include <inttypes.h>

#define PCI_HEADER_TYPE_NORMAL  0
#define PCI_HEADER_TYPE_BRIDGE  1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_STATUS_CAP_LIST 0x10

#define PCI_COMMAND                0x04
#define PCI_STATUS                 0x06
#define PCI_INTERRUPT_LINE         0x3C
#define PCI_INTERRUPT_PIN          0x3D
#define PCI_SUBSYSTEM_VENDOR_ID    0x2C
#define PCI_SUBSYSTEM_ID           0x2E
#define PCI_LATENCY_TIMER          0x0D
#define PCI_CAPABILITY_LIST        0x34
#define PCI_CB_CAPABILITY_LIST     0x14
#define PCI_CACHE_LINE_SIZE        0x0C
#define PCI_CB_SUBSYSTEM_VENDOR_ID 0x40
#define PCI_CB_SUBSYSTEM_ID        0x42
#define PCI_ROM_ADDRESS            0x30
#define PCI_ROM_ADDRESS_1          0x38

#define PCI_PM_PMC  2
#define PCI_PM_CTRL 4

#define PCI_PM_CAP_D1 0x0200
#define PCI_PM_CAP_D2 0x0400

#define PCI_ROM_ADDRESS_ENABLE 0x01
#define PCI_COMMAND_IO         0x01
#define PCI_COMMAND_MEMORY     0x02
#define PCI_COMMAND_MASTER     0x04

#define PCI_BASE_ADDRESS_0            0x10
#define PCI_BASE_ADDRESS_SPACE        0x01
#define PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define PCI_IO_RESOURCE_MEM           0x00
#define PCI_IO_RESOURCE_IO            0x01

#define PCI_CAP_LIST_ID   0
#define PCI_CAP_ID_PM     0x01
#define PCI_CAP_ID_AGP    0x02
#define PCI_CAP_ID_VPD    0x03
#define PCI_CAP_ID_SLOTID 0x04
#define PCI_CAP_ID_MSI    0x05
#define PCI_CAP_ID_CHSWP  0x06
#define PCI_CAP_LIST_NEXT 1
#define PCI_CAP_FLAGS     2
#define PCI_CAP_SIZEOF    4

#define PCI_ROM_ADDRESS_MASK      ( ~0x7FFUL )
#define PCI_BASE_ADDRESS_MEM_MASK ( ~0x0FUL )
#define PCI_BASE_ADDRESS_IO_MASK  ( ~0x03UL )
#define PCI_PM_CTRL_STATE_MASK    ( 0x0003 )

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

typedef struct confadd
{
  uint8_t reg    : 8;
  uint8_t func   : 3;
  uint8_t dev    : 5;
  uint8_t bus    : 8;
  uint8_t rsvd   : 7;
  uint8_t enable : 1;
} __attribute__ ((__packed__)) confadd_t;

struct pci_list_t
{
  struct pci_cfg *cfg;
  struct pci_list_t *next;
} __attribute__ ((__packed__));


void init_pci( );

int pci_find_cfg( uint16_t vendor_id, 
				  uint16_t device_id, 
				  int num,
				  struct pci_cfg *cfg );

int pci_find_probe( 	int bus, 
						int dev, 
						int func, 
						struct pci_cfg *cfg );

#endif

