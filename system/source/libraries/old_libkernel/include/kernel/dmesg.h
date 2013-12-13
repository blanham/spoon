#ifndef _KERNEL_DMESG_H
#define _KERNEL_DMESG_H


#ifdef __cplusplus
extern "C" {
#endif


int smk_get_dmesg( void *data, int len );
int smk_get_dmesg_size();


#ifdef __cplusplus
}
#endif

#endif

