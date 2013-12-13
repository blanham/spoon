#!/bin/sh

#  Convert the file "memdisk" to a binary file to be
#  included in the final linked ELF image.
#



objcopy -I binary -O elf32-i386  \
  --redefine-sym _binary___memdisk_start=_DISK  \
  --redefine-sym _binary___memdisk_end=_DISK_END  \
  --redefine-sym _binary___memdisk_size=_DISK_SIZE  \
  ./memdisk ./disk.o

echo -e "\3" | dd bs=1 count=1 seek=18 of=./disk.o conv=notrunc


