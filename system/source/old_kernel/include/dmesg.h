#ifndef _KERNEL_DMESG_H
#define _KERNEL_DMESG_H


int dmesg( char *format, ... );
int dmesg_xy( int x, int y, char *format, ... );


int get_dmesg( void *buffer, int len );
int get_dmesg_size();


#endif

