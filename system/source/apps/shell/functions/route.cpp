#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../flist.h"
#include "route.h"

//#include <spoon/network/NetDefs.h>
//#include <spoon/network/NetConfig.h>

struct function_info info_route = 
{
   "routing table manipulation",
   "route [add/del INFO]\n"
   "This command can be used to manipulate or display the routine\n"
   "table. Command usage:\n"
   "    route                               - print routing table\n"
   "    route add IP NETMASK GATWAY DEVICE  - add a route\n"
   "    route del IP                        - delete a route\n"
   "\n"
   " - The default route is specified by an IP *and* NETMASK of 0.0.0.0\n"
   " - DEVICE is a device name such as \"eth0\" or \"eth1\".\n"
   " - Use ifconfig to list the device names.\n"
   ,
   route_main
};

/*
uint32 route_string_toip( char *ip)
{
  uint32 answer;
  int offset;
  int pos;
  uint32 working;
  unsigned char c;

  offset = 24;
  answer = 0;
  working = 0;
  pos = 0;

   while ( offset >= 0 )
   {
     c = ip[pos++];
     
     switch (c)
     {
       case '0':
       case '1':
       case '2':
       case '3':
       case '4':
       case '5':
       case '6':
       case '7':
       case '8':
       case '9':
      		c = c - '0';
		working = working * 10 + c;
       		break;

       case 0:
       case '.':
      		answer = answer | ( working << offset );
		offset -= 8;
		working = 0;
       		break;
     }

   }


  return answer;
}

char *route_ip_tostring(uint32 ip)
{
  int offset;
  int pos;
  int k;
  static char str[32]; 
  unsigned char c;

   offset = 24;
   pos = 0;
   while (offset >= 0 )
   {
   	c = (ip >> offset) & 0xFF;
	k = 0;
	
	switch (c/100)
	{
	  case 2: str[pos++] = '2'; k = 1; break;
	  case 1: str[pos++] = '1'; k = 1; break;
	}

	c = c % 100;

	switch (c/10)
	{
	  case 9: str[pos++] = '9'; k = 1; break;
	  case 8: str[pos++] = '8'; k = 1; break;
	  case 7: str[pos++] = '7'; k = 1; break;
	  case 6: str[pos++] = '6'; k = 1; break;
	  case 5: str[pos++] = '5'; k = 1; break;
	  case 4: str[pos++] = '4'; k = 1; break;
	  case 3: str[pos++] = '3'; k = 1; break;
	  case 2: str[pos++] = '2'; k = 1; break;
	  case 1: str[pos++] = '1'; k = 1; break;
	  case 0: if ( k == 1 ) { str[pos++] = '0'; k = 1;} break;
	}

	c = c % 10;

	switch (c)
	{
	  case 9: str[pos++] = '9'; k = 1; break;
	  case 8: str[pos++] = '8'; k = 1; break;
	  case 7: str[pos++] = '7'; k = 1; break;
	  case 6: str[pos++] = '6'; k = 1; break;
	  case 5: str[pos++] = '5'; k = 1; break;
	  case 4: str[pos++] = '4'; k = 1; break;
	  case 3: str[pos++] = '3'; k = 1; break;
	  case 2: str[pos++] = '2'; k = 1; break;
	  case 1: str[pos++] = '1'; k = 1; break;
	  case 0: str[pos++] = '0'; k = 1; break;
	}
	
	offset -= 8;
	if ( offset >= 0 ) str[pos++] = '.';
   }

  str[pos++] = 0;

  return str;
}

int route_print()
{
  NetConfig *net = new NetConfig();
  	     net->Refresh();

    printf("%s\n","net_server IP routing information");
    printf("%20s %20s %20s %10s\n",
    		"IP", "NETMASK", "GATEWAY", "DEVICE" );

    for ( int i = 0; i < net->CountRoutes(); i++ )
     {
       struct net_route* route = net->RouteAt( i );
       printf("%20s ",  route_ip_tostring( route->ip ) );
       printf("%20s ",  route_ip_tostring( route->netmask ) );
       printf("%20s ",  route_ip_tostring( route->gateway ) );
       printf("%10s\n", route->device );
     } 


  delete net;
  return -1;
}

int route_del( char *ip )
{
  NetConfig *net = new NetConfig();

	uint32 rip = route_string_toip( ip );

	net->DeleteRoute( rip );

  delete net;
  return 0;
}

int route_add( char *ip, char *netmask, char *gateway, char *device )
{
  uint32 nip;
  uint32 nm;
  uint32 ng;

    nip = route_string_toip( ip );
    nm = route_string_toip( netmask );
    ng = route_string_toip( gateway );


	NetConfig *net = new NetConfig();
	  net->AddRoute( nip, nm, ng, device );
	delete net;


  return 0;
}
*/

int route_main( int argc, char **argv )
{
/*
  switch (argc)
  {
    case 1: // PRINT
    		return route_print();	
    case 3: // ROUTE DEL IP
    		if ( strcmp( argv[1], "del" ) != 0 ) return -1;
    		return route_del( argv[2] ) ;	
    case 6: // ROUTE ADD IP NETMASK GATEWAY DEVICE
    		if ( strcmp( argv[1], "add" ) != 0 ) return -1;
    		return route_add( argv[2], argv[3], argv[4], argv[5] ) ;	
  }
*/

 printf("%s\n","route is temporarily disabled until I get the net_server running again.");
  return 0;
}


