#include <types.h>
#include <spoon/drivers/Driver.h>

#include <spoon/network/Packet.h>

#ifndef _RTL8139Driver_H
#define _RTL8139Driver_H



class RTL8139Driver 	:	public Driver
{
	public:
		RTL8139Driver( uint32 base, int irq );
		~RTL8139Driver();


		void Init();
		virtual int32 IRQ( int32 irq );


		void IP_ICMP( char *buffer );
		void IP_TCP( char *buffer );
		void IP_UDP( char *buffer );

		void Send_DHCP_Request();

		void do_dhcp( DHCP_Packet *packet );
		void do_icmp( ICMP_Packet *packet );
		void do_udp( UDP_Packet *packet );
		void do_tcp( TCP_Packet *packet );
		void do_arp( Packet *p );
		void do_ip( Packet *p );
		void do_packet(  char *buffer, int length );
		void ARP_Reply( unsigned char mac[6],
				unsigned char tip[4],
				unsigned char sip[4] );


		virtual int SendBuffer( const char *ptr, int len );

		void IntRX();
		void IntTX( uint16 status );

	private:
		uint32 base;
		uint32 irq;

};

#endif

