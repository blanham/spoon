#ifndef _INTTYPES_H
#define _INTTYPES_H


#ifndef NULL
#define NULL (void*)0
#endif

#ifndef _HAVE_SIZE_T
#define _HAVE_SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _HAVE_TIME_T
#define _HAVE_TIME_T
typedef unsigned int time_t;
#endif


#ifndef true
#define true 		(1==1)
#endif

#ifndef false
#define false 		(1==0)
#endif


typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef long long int64_t;
typedef int int32_t;
typedef short int16_t;
typedef char int8_t;


typedef int32_t  intptr_t;
typedef uint32_t uintptr_t;



#endif

