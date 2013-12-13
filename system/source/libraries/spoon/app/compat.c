#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>

// ---------------------------------
void *  operator new (uint32 size) 
{
  return malloc(size);
}

void *  operator new[] (uint32 size) 
{
  return malloc(size);
}

void  operator delete (void *p)
{
  free( p );
}

void  operator delete[] (void *p)
{
  free(p);
}


extern "C" 
void _Unwind_Resume()
{
}

extern "C" 
void __gxx_personality_v0()
{
}


extern "C" 
void __cxa_pure_virtual ()
{
//This routine will only be called if the user calls a non-overridden pure virtual function, which has undefined behavior according to the C++ Standard. Therefore, this ABI does not specify its behavior, but it is expected that it will terminate the program, possibly with an error message. 
  printf("%s\n","Call to a pure virtual function!!");
  smk_exit(-1);
}

//extern "C" 
//void *__dso_handle __attribute__ ((_hidden_));
 
void *__dso_handle = &__dso_handle;

extern "C" 
int __cxa_atexit( void (*func)(void*), void* arg, void *dso )
{
  return 0;
}

//extern "C";



