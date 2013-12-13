#ifndef _LIBKERNEL_STRINGS_H
#define _LIBKERNEL_STRINGS_H


extern char *process_state[];
extern char *thread_state[];
extern char *exception_strings[];


void* 	kmemcpy(void* s, const void* ct, int n);
void* 	kmemset(void* s, int c, int n); 
char* 	kstrcpy(char* s, const char* ct);
int 	kstrlen(const char* cs);

#endif

