#ifndef _NET_SERVER_HOST_H
#define _NET_SERVER_HOST_H

#include <types.h>

inline uint32  htonl( uint32 a )
{
  return (  ((a & 0xFF000000) >> 24) |
	    ((a & 0x00FF0000) >>  8) |
	    ((a & 0x0000FF00) <<  8) |
	    ((a & 0x000000FF) << 24) );
}

inline uint16  htons( uint16 a )
{
  return (  ((a & 0xFF00) >>  8) |
	    ((a & 0x00FF) <<  8)  );
}

inline uint32  ntohl( uint32 a )
{
 	return htonl( a );
}

inline uint16  ntohs( uint16 a )
{
	return htons(a);
}

#endif

