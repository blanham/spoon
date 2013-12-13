#ifndef _KERNEL_ALLOC_H
#define _KERNEL_ALLOC_H


void* malloc(int size);
void  free(void *ptr);
void* calloc(int nobj, int size);
void* realloc(void *p, int size);


#endif
		
