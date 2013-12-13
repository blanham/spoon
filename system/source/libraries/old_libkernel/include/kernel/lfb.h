#ifndef _LIBKERNEL_LFB_H
#define _LIBKERNEL_LFB_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif


int   smk_get_lfb_info( uint32 *width,
                    uint32 *height,
                    uint32 *depth,
                    uint32 *type,
                    void **ptr
		    );

int   smk_set_lfb_info( uint32 width,
                    uint32 height,
                    uint32 depth,
                    uint32 type,
		    uint32 ptr);

void *smk_request_lfb();


#ifdef __cplusplus
}
#endif

#endif

