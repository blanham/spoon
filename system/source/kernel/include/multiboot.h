#ifndef _KERNEL_MULTIBOOT_H
#define _KERNEL_MULTIBOOT_H

#define MULTIBOOT_PAGE_ALIGN            (1<<0)
#define MULTIBOOT_MEMORY_INFO           (1<<1)
#define MULTIBOOT_VESA_MODE				(1<<2)



/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002

/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

/* Types. */


/* VBE controller information.  */
struct vbe_controller
{
  unsigned char signature[4];
  unsigned short version;
  unsigned long oem_string;
  unsigned long capabilities;
  unsigned long video_mode;
  unsigned short total_memory;
  unsigned short oem_software_rev;
  unsigned long oem_vendor_name;
  unsigned long oem_product_name;
  unsigned long oem_product_rev;
  unsigned char reserved[222];
  unsigned char oem_data[256];
} __attribute__ ((packed));

/* VBE mode information.  */
struct vbe_mode
{
  unsigned short mode_attributes;
  unsigned char win_a_attributes;
  unsigned char win_b_attributes;
  unsigned short win_granularity;
  unsigned short win_size;
  unsigned short win_a_segment;
  unsigned short win_b_segment;
  unsigned long win_func;
  unsigned short bytes_per_scanline;

  /* >=1.2 */
  unsigned short x_resolution;
  unsigned short y_resolution;
  unsigned char x_char_size;
  unsigned char y_char_size;
  unsigned char number_of_planes;
  unsigned char bits_per_pixel;
  unsigned char number_of_banks;
  unsigned char memory_model;
  unsigned char bank_size;
  unsigned char number_of_image_pages;
  unsigned char reserved0;

  /* direct color */
  unsigned char red_mask_size;
  unsigned char red_field_position;
  unsigned char green_mask_size;
  unsigned char green_field_position;
  unsigned char blue_mask_size;
  unsigned char blue_field_position;
  unsigned char reserved_mask_size;
  unsigned char reserved_field_position;
  unsigned char direct_color_mode_info;

  /* >=2.0 */
  unsigned long phys_base;
  unsigned long reserved1;
  unsigned short reversed2;

  /* >=3.0 */
  unsigned short linear_bytes_per_scanline;
  unsigned char banked_number_of_image_pages;
  unsigned char linear_number_of_image_pages;
  unsigned char linear_red_mask_size;
  unsigned char linear_red_field_position;
  unsigned char linear_green_mask_size;
  unsigned char linear_green_field_position;
  unsigned char linear_blue_mask_size;
  unsigned char linear_blue_field_position;
  unsigned char linear_reserved_mask_size;
  unsigned char linear_reserved_field_position;
  unsigned long max_pixel_clock;

  unsigned char reserved3[189];
  
} __attribute__ ((packed));



/* The Multiboot header. */
typedef struct multiboot_header
{
   unsigned int magic			: 32;
   unsigned int flags			: 32;
   unsigned int checksum		: 32;
   unsigned int header_addr		: 32;
   unsigned int load_addr		: 32;
   unsigned int load_end_addr	: 32;
   unsigned int bss_end_addr	: 32;
   unsigned int entry_addr		: 32;
   unsigned int mode_type		: 32;
   unsigned int width			: 32;
   unsigned int height			: 32;
   unsigned int depth			: 32;
} __attribute__ ((packed)) multiboot_header_t ;

/* The symbol table for a.out. */
typedef struct aout_symbol_table
{
  unsigned int tabsize		: 32;
  unsigned int strsize		: 32;
  unsigned int addr			: 32;
  unsigned int reserved		: 32;
} __attribute__ ((packed)) aout_symbol_table_t;

/* The section header table for ELF. */
typedef struct elf_section_header_table
{
  unsigned int num			: 32;
  unsigned int size			: 32;
  unsigned int addr			: 32;
  unsigned int shndx		: 32;
} __attribute__ ((packed)) elf_section_header_table_t;

/* The Multiboot information. */
typedef struct multiboot_info
{
  unsigned int flags			: 32;
  unsigned int mem_lower		: 32;
  unsigned int mem_upper		: 32;
  unsigned int boot_device		: 32;
  unsigned int cmdline			: 32;
  unsigned int mods_count		: 32;
  unsigned int mods_addr		: 32;
  union
  {
     aout_symbol_table_t aout_sym;
     elf_section_header_table_t elf_sec;
  } u;
  unsigned int mmap_length			: 32;
  unsigned int mmap_addr			: 32;
  unsigned int drives_length		: 32;
  unsigned int drives_addr			: 32;
  unsigned int config_table			: 32;
  unsigned int boot_loader_name		: 32;
  unsigned int apm_table			: 32;
  unsigned int vbe_control_info		: 32;
  unsigned int vbe_mode_info		: 32;
  unsigned int vbe_mode				: 32;
  unsigned int vbe_interface_seg	: 32;
  unsigned int vbe_interface_off	: 32;
  unsigned int vbe_interface_len	: 32;
} __attribute__ ((packed)) multiboot_info_t;

/* The module structure. */
typedef struct module
{
  unsigned int mod_start		: 32;
  unsigned int mod_end			: 32;
  unsigned int string			: 32;
  unsigned int reserved			: 32;
} __attribute__ ((packed)) module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
 *    but no size. */
typedef struct memory_map
{
  unsigned int size				: 32;
  unsigned int base_addr_low	: 32;
  unsigned int base_addr_high	: 32;
  unsigned int length_low		: 32;
  unsigned int length_high		: 32;
  unsigned int type				: 32;
} __attribute__ ((packed)) memory_map_t;


#endif

