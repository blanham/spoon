#ifndef _LIBKERNEL_EXEC_H
#define _LIBKERNEL_EXEC_H

#ifdef __cplusplus
extern "C" {
#endif

int32 smk_mem_exec( const char *name, uint32 start, uint32 end, const char *command_line );


#ifdef __cplusplus
}
#endif

#endif

