#ifndef _KERNEL_STRINGS_H
#define _KERNEL_STRINGS_H

#include "types.h"

char* strcpy(char* s, const char* ct);
char* strncpy(char* s, const char* ct, int n);
int strcmp( const char*, const char* );
int strncmp( const char* a, const char* b, int n);

void* memcpy(void* /* restrict */s1, const void* /* restrict */s2, int n);
void* memset(void *dest, int c, int n);
int memcmp( const char *a, const char *b, int len );

int strlen(const char* a);



#endif





