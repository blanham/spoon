#ifndef KERNEL_MODULES_H
#define KERNEL_MODULES_H

#include <multiboot.h>

int reserve_modules( multiboot_info_t *mb_info );
int init_modules();

int start_first_module();

#endif

