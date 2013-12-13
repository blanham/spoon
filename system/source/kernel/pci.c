#include <pci.h>
#include <assert.h>
#include <alloc.h>
#include <inttypes.h>
#include <misc.h>

#include "io.h"

static struct pci_list_t *pci_list = NULL;


// ******************************************************************

uint32_t pciRead( int bus, int dev, int func, int reg, int uint8_ts )
{
  uint16_t base;
  confadd_t c;

 	 c.enable = 1;
	 c.rsvd = 0;
	 c.bus = bus;
 	 c.dev = dev;
 	 c.func = func;
 	 c.reg = reg & 0xFC;

  long *n = (long*)((void*)&c);

  outl( 0xCF8, *n );
  base = 0xCFC + ( reg & 0x03 );

  switch ( uint8_ts )
  {
    case 1: return inb( base );
    case 2: return inw( base );
    case 4: return inl( base );
  }

  return 0;
}

void pciWrite( int bus, int dev, int func, int reg, uint32_t v, int uint8_ts )
{
  uint16_t base;
  confadd_t c;
  
  	c.enable = 1;
  	c.rsvd = 0;
  	c.bus = bus;
  	c.dev = dev;
  	c.func = func;
  	c.reg = reg & 0xFC;

  long *n = (long*)((void*)&c);
  
  outl( 0xCF8, *n );
  base = 0xCFC + ( reg & 0x03 );

  switch ( uint8_ts )
  {
    case 1: outb( base, ( uint8_t )v ); break;
    case 2: outw( base, ( uint16_t )v ); break;
    case 4: outl( base, v ); break;
  }
}

// *******************************************************************

uint8_t pci_read_config_byte( int bus, int dev, int func, int reg )
{
  return ( pciRead( bus, dev, func, reg, sizeof( uint8_t ) ) );
}

uint16_t pci_read_config_word( int bus, int dev, int func, int reg )
{
  return ( pciRead( bus, dev, func, reg, sizeof( uint16_t ) ) );
}

uint32_t pci_read_config_dword( int bus, int dev, int func, int reg )
{
  return ( pciRead( bus, dev, func, reg, sizeof( uint32_t ) ) );
}

void pci_write_config_byte( int bus, int dev, int func, int reg, uint8_t val )
{
  pciWrite( bus, dev, func, reg, val, sizeof( uint8_t ) );
}

void pci_write_config_word( int bus, int dev, int func, int reg, uint16_t val )
{
  pciWrite( bus, dev, func, reg, val, sizeof( uint16_t ) );
}

void pci_write_config_dword( int bus, int dev, int func, int reg, uint32_t val )
{
  pciWrite( bus, dev, func, reg, val, sizeof( uint32_t ) );
}

// *******************************************************************

void pci_read_irq( struct pci_cfg *cfg )
{
  uint8_t irq;

  irq = pci_read_config_byte( cfg->bus, 
				  			  cfg->dev, 
							  cfg->func, 
							  PCI_INTERRUPT_PIN );
  
  if ( irq != 0 ) 
    irq = pci_read_config_byte( cfg->bus, 
								cfg->dev, 
								cfg->func, 
								PCI_INTERRUPT_LINE );

  cfg->irq = irq;
}

uint32_t pci_size( uint32_t base, unsigned long mask )
{
  uint32_t size = mask & base;
  size = size & ~( size - 1 );
  return ( size - 1 );
}

void pci_read_bases( struct pci_cfg *cfg, int tot_bases, int rom )
{
  int i;
  uint32_t l, sz, reg;

  memset( cfg->base, 0, sizeof( cfg->base ) );
  memset( cfg->size, 0, sizeof( cfg->size ) );
  memset( cfg->type, 0, sizeof( cfg->type ) );

  for( i = 0; i < tot_bases; i++ )
  {
    reg = PCI_BASE_ADDRESS_0 + ( i << 2 );
	
    l = pci_read_config_dword( cfg->bus, cfg->dev, cfg->func, reg );
    pci_write_config_dword( cfg->bus, cfg->dev, cfg->func, reg, ~0 );
	
    sz = pci_read_config_dword( cfg->bus, cfg->dev, cfg->func, reg );
    pci_write_config_dword( cfg->bus, cfg->dev, cfg->func, reg, l );

    if ( (!sz) || (sz == 0xFFFFFFFF) )  continue; 
    if ( l == 0xFFFFFFFF )  l = 0; 

    if ( ( l & PCI_BASE_ADDRESS_SPACE ) == PCI_BASE_ADDRESS_SPACE_MEMORY )
    {
      cfg->base[ i ] = l & PCI_BASE_ADDRESS_MEM_MASK;
      cfg->size[ i ] = pci_size( sz, PCI_BASE_ADDRESS_MEM_MASK );
      cfg->type[ i ] = PCI_IO_RESOURCE_MEM;
    }
    else
    {
      cfg->base[ i ] = l & PCI_BASE_ADDRESS_IO_MASK;
      cfg->size[ i ] = pci_size( sz, PCI_BASE_ADDRESS_IO_MASK );
      cfg->type[ i ] = PCI_IO_RESOURCE_IO;
    }
  }

  if ( rom != 0 )
  {
    cfg->rom_base = 0;
    cfg->rom_size = 0;

    l = pci_read_config_dword(cfg->bus, cfg->dev, cfg->func, rom);
    pci_write_config_dword(cfg->bus, cfg->dev, cfg->func, rom, ~PCI_ROM_ADDRESS_ENABLE);
	
    sz = pci_read_config_dword(cfg->bus, cfg->dev, cfg->func, rom);
    pci_write_config_dword(cfg->bus, cfg->dev, cfg->func, rom, l);
	
    if ( l == 0xFFFFFFFF ) { l = 0; }
    if ( (sz) && (sz != 0xFFFFFFFF) )
    {
      cfg->rom_base = l & PCI_ROM_ADDRESS_MASK;
      sz = pci_size( sz, PCI_ROM_ADDRESS_MASK );
      cfg->rom_size = cfg->rom_size + ( unsigned long )sz;
    }
  }
}

int pci_probe( int bus, int dev, int func, struct pci_cfg *cfg )
{
  uint32_t *temp = ( uint32_t* )cfg;
  int i;

  for( i = 0; i < 4; i++ )
  {
    temp[ i ] = pci_read_config_dword( bus, dev, func, ( i << 2 ) );
  }

  if ( cfg->vendor_id == 0xFFFF ) { return -1; }

  cfg->bus = bus;
  cfg->dev = dev;
  cfg->func = func;

  cfg->current_state = 4;

  switch ( cfg->header_type & 0x7F )
  {
    case PCI_HEADER_TYPE_NORMAL:
      pci_read_irq( cfg );
      pci_read_bases( cfg, 6, PCI_ROM_ADDRESS );
      cfg->subsys_vendor = pci_read_config_word( bus, dev, func, PCI_SUBSYSTEM_VENDOR_ID );
      cfg->subsys_device = pci_read_config_word( bus, dev, func, PCI_SUBSYSTEM_ID );
      break;

    case PCI_HEADER_TYPE_BRIDGE:
      pci_read_bases( cfg, 2, PCI_ROM_ADDRESS_1 );
      break;

    case PCI_HEADER_TYPE_CARDBUS:
      pci_read_irq( cfg );
      pci_read_bases( cfg, 1, 0 );
      cfg->subsys_vendor = pci_read_config_word( bus, dev, func, PCI_CB_SUBSYSTEM_VENDOR_ID );
      cfg->subsys_device = pci_read_config_word( bus, dev, func, PCI_CB_SUBSYSTEM_ID );
      break;

  }

  return 0;
}


int pci_find_cfg( uint16_t vendor_id, 
				  uint16_t device_id, int num, 
				  struct pci_cfg *cfg )
{
  struct pci_cfg *act_cfg;
  struct pci_list_t *tmp = pci_list;
  int count = 0;

  while ( tmp != NULL )
  {
    act_cfg = tmp->cfg;
    if ( (act_cfg->vendor_id == vendor_id) && (act_cfg->device_id == device_id) )
		if ( count++ == num )
	    {
	      memcpy( cfg, act_cfg, sizeof( struct pci_cfg ) );
	      return 0;
	    }

    tmp = tmp->next;
  }

  return -1;
}

int pci_find_probe( int bus, int dev, int func, struct pci_cfg *cfg )
{
  struct pci_cfg *act_cfg;
  struct pci_list_t *tmp = pci_list;

  while ( tmp != NULL )
  {
    act_cfg = tmp->cfg;
    if ( (act_cfg->bus == bus) && 
			(act_cfg->dev == dev) && 
				(act_cfg->func == func) )
    {
      memcpy( cfg, act_cfg, sizeof( struct pci_cfg ) );
      return 0;
    }

    tmp = tmp->next;
  }

  return -1;
}




void pci_add_list( struct pci_list_t *new_item )
{

  if ( pci_list == NULL ) pci_list = new_item;
  else
  {
	struct pci_list_t *tmp = pci_list;
	while ( tmp->next != NULL ) tmp = tmp->next;

    tmp->next = new_item;
  }
}

void pci_scan_all( )
{
  uint16_t bus, dev, func;
  struct pci_cfg *pcfg;
  struct pci_list_t *list_item;


  for ( bus = 0; bus < 4; bus++ )
    for ( dev = 0; dev < 32; dev++ )
      for ( func = 0; func < 8; func++ )
        if ( pci_probe( bus, dev, func, pcfg ) == 0 )
        {
          if ( (pcfg->vendor_id == 0) && (pcfg->device_id == 0) ) continue;

		  pcfg = (struct pci_cfg*)malloc( sizeof( struct pci_cfg ) );
		  assert( pcfg != NULL );

          list_item = (struct pci_list_t*)malloc( sizeof( struct pci_list_t ) );
		  assert( list_item != NULL );

          list_item->cfg = pcfg;
          list_item->next = NULL;

          pci_add_list( list_item ); // add it to the list
        }

}



void init_pci()
{
	assert( pci_list == NULL );
	pci_scan_all();
}



