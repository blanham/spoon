#ifndef _LIBKERNEL_CONSOLE_H
#define _LIBKERNEL_CONSOLE_H


#ifdef __cplusplus
extern "C" {
#endif

#define 	ALL_CONSOLES		0x10000000

#define		VC_MODE_BUFFER		0
#define		VC_MODE_MESSAGE		1

#define		VC_PUTC			0
#define		VC_GOTOXY		1
#define		VC_CLEAR		2

#include <types.h>

void smk_init_console();

int smk_new_console( int pages );
int smk_stage_console( void* data, int pages );
int smk_delete_console( int id );


int32 smk_set_console( int32 console );
int32 smk_get_console( int pid );
char *smk_get_console_location();

int smk_switch_console( int pid, int console );

int smk_set_active_pid( int console, int pid );
int smk_active_pid( int console );


// --- PREPARING AND SETTING UP.........

int smk_create_console( int pages, int mode, int pid, int port, int active_pid );




// -------------------------------------

void smk_setfgbg( int fg, int bg );

void smk_conio_puts( const char* n );
void smk_conio_putc( unsigned char n );

void smk_conio_gotoxy( uint8 x, uint8 y);
void smk_conio_getxy( uint8 *x, uint8 *y);
void smk_conio_clear();



#ifdef __cplusplus
}
#endif

#endif
