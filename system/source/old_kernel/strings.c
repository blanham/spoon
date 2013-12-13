#include <types.h>



void *memset(void *dest, int c, int n)
{
  char *new_dest = dest;
  int i = 0;
  
  while (i++ != n )
  {
    *new_dest = c;
     new_dest++;
  }

  return dest;
}



void* 	memcpy(void* /* restrict */s1, const void* /* restrict */s2, int n)
{
  char *cdest;
  char *csrc;
  uint32 *ldest = (uint32*)s1;
  uint32 *lsrc  = (uint32*)s2;

  while ( n >= 4 )
  {
      *ldest++ = *lsrc++;
	  n -= 4;
  }

  cdest = (char*)ldest;
  csrc  = (char*)lsrc;
  
  while ( n-- > 0 )
      *cdest++ = *csrc++;
  
  return s1;
}


char* 	strcpy(char* s, const char* ct)
{
	int i = 0;
	do { s[i] = ct[i]; } 
		while (ct[i++] != '\0');
	return s;
}

char*	strncpy(char* s, const char* ct, int n)
{
	int i;

	for ( i = 0; i < n; i++ )
	{
		s[i] = ct[i]; 
		if ( ct[i] == '\0' ) break;
	}
	
	while ( i < n ) s[i++] = '\0'; 
	   
	return s;
}




int memcmp( const char *a, const char *b, int len )
{
  char *src = (char*)a;
  char *dst = (char*)b;
  
  while ( len-- > 0 )
  {
	if ( *src < *dst )  return -1;
	if ( *src > *dst )  return  1;
	src++;
	dst++;
  }

  return 0;
}




int32 strcmp( const char* a, const char* b)
{
  char *src;
  char *dst;
  
  src = (char*)a;
  dst = (char*)b;


  while ((*src != 0) && (*dst != 0))
  {
    if ( *src < *dst ) return -1;
    if ( *src > *dst ) return 1;
    if ((*src == 0) ||  (*dst == 0))  break;
    src++;
    dst++; 
  }

  if ( *src == *dst ) return 0;
  if ( *src < *dst ) return -1;
  if ( *src > *dst ) return 1;
 
  return 0;
}



int strncmp( const char* a, const char* b, int n)
{
  char *src;
  char *dst;
  int count = 0;
  int boom = 0;
  
  src = (char*)a;
  dst = (char*)b;

  while ((*src != 0) && (*dst != 0))
  {
    if ( *src != *dst ) count++;
    if ((*src == 0) ||  (*dst == 0))  break;
    src++;
    dst++; 
    boom ++;
    if (boom == n) return count;
  }

  if ( *src != *dst ) count++;

 
  return count;
}


int strlen(const char* a)
{
  int i = 0;
  while (a[i] != '\0') i++;
  return i;
}







