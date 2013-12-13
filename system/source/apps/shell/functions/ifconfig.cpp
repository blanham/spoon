#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../flist.h"
#include "ifconfig.h"

//#include <spoon/network/NetDefs.h>
//#include <spoon/network/NetConfig.h>

struct function_info info_ifconfig = 
{
   "ip address information and manipulation",
   "ifconfig [add/del INFO]/[devices]\n"
   "This is the command which manipulates network interfaces (as in\n"
   "ip addresses and not network cards). If called without any\n"
   "parameters, it will list the interfaces.\n"
   "\n"
   "Command usage:\n"
   "\n"
   "   ifconfig                                   - list interfaces\n"
   "   ifconfig add IP NETMASK BROADCAST DEVICE   - add interface\n"
   "   ifconfig del IP                            - del interface\n"
   "   ifconfig devices                           - list devices\n"
   ,
   ifconfig_main
};

/*
uint32 ifconfig_string_toip( char *ip)
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

char *ifconfig_ip_tostring(uint32 ip)
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


int ifconfig_list()
{

  NetConfig *net = new NetConfig();
  	     net->Refresh();

    printf("%20s %20s %20s %10s\n","IP","NETMASK","BROADCAST","DEVICE");

    for ( int i = 0; i < net->CountInterfaces(); i++ )
     {
       struct net_interface* interface = net->InterfaceAt( i );

	 printf("%20s " , ifconfig_ip_tostring( interface->ip ) );
	 printf("%20s " , ifconfig_ip_tostring( interface->netmask ) );
	 printf("%20s " , ifconfig_ip_tostring( interface->broadcast ) );
         printf("%10s\n" , interface->device );
     }


  delete net;

  return 0; 
}




int ifconfig_devices()
{

  NetConfig *net = new NetConfig();
  	     net->Refresh();

    printf("%10s %20s %40s\n","DEVICE","DRIVER","DESCRIPTION" );

    for ( int i = 0; i < net->CountDevices(); i++ )
     {
       struct net_device* device = net->DeviceAt( i );

         printf("%10s " , device->name );
	 printf("%20s " , "" );
	 printf("%40s\n", "" );
     }


  delete net;

  return 0; 
}


int ifconfig_add( char *ip, char *netmask, char *broadcast, char *device )
{
  uint32 _ip;
  uint32 _netmask;
  uint32 _broadcast;

  NetConfig *net = new NetConfig();
 	     net->Refresh();
    
    _ip = ifconfig_string_toip( ip );
    _netmask = ifconfig_string_toip( netmask );
    _broadcast = ifconfig_string_toip( broadcast );

	net->AddInterface(  _ip, _netmask, _broadcast, device );

	
  delete net;
  return 0;
}

int ifconfig_del( char *ip )
{
  uint32 _ip;
  
  NetConfig *net = new NetConfig();
 	     net->Refresh();
 
    _ip = ifconfig_string_toip( ip );

    net->DeleteInterface( _ip );
 

  delete net;
  return 0;
}

*/
int ifconfig_main( int argc, char **argv )
{
/*
	switch (argc)
	 {
	  case 1:  // IFCONFIG
		  return ifconfig_list();

	  case 2:  // IFCONFIG DEVICES
	  	  if ( strcmp( argv[1], "devices" ) != 0) return -1;
		  return ifconfig_devices(); 

	  case 3:  // IFCONFIG DEL IP
	  	  if ( strcmp( argv[1], "del" ) != 0 ) return -1;
		  return ifconfig_del( argv[2] );
	   
	  case 6:  // IFCONFIG ADD IP NETMASK BROADCAST DEVICE
	  	  if ( strcmp( argv[1], "add" ) != 0 ) return -1;
		  return ifconfig_add( argv[2], argv[3], argv[4], argv[5] );
	 }
*/
 printf("%s\n","ifconfig is temporarily disabled until I get the net_server running again.");
 return 0;
}


