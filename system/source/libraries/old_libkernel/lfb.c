#include <kernel.h>



/**  \defgroup LFB  Linear Frame Buffer  


The kernel is also the keeper of the video LFB ( linear frame buffer ). It is
supposed to know the location, width, height, depth of the video buffer at
all times and provide it to the userland applications who need it and have permission
to access it.

<P>

It requests a 800x600x32 LFB using the multiboot header and GRUB is supposed to
provide it. However, GRUB's vesa implementation is incomplete so I've done some
modifications myself.  You can get the modifications from the "instructions" 
section for the <A HREF='/spoon/grub/grub.html'>GRUB setup</A>.


<P>
Userland applications can modify the kernel's LFB information if they have permission.
This is going to be handy for new video card drivers which set up their own LFB
and then need to pass the information along to the kernel to use.


  
 */

/** @{ */

int   smk_get_lfb_info( uint32 *width,
                    uint32 *height,
                    uint32 *depth,
                    uint32 *type,
                    void **ptr
		    )
{
	return _sysenter( (SYS_LFB|SYS_ONE),
			(uint32)width,
			(uint32)height,
			(uint32)depth,
			(uint32)type,
			(uint32)ptr );
}

int   smk_set_lfb_info( uint32 width,
                    uint32 height,
                    uint32 depth,
                    uint32 type,
		    		uint32 ptr )
{
	return _sysenter( (SYS_LFB|SYS_TWO),
			(uint32)width,
			(uint32)height,
			(uint32)depth,
			(uint32)type,
			(uint32)ptr );
}

void *smk_request_lfb()
{
	return (void*)_sysenter( (SYS_LFB|SYS_THREE),
			       0,0,0,0,0 );
}


/** @} */

