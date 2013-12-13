#include <types.h>
#include <spoon/drivers/Driver.h>

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

		void do_arp( char *buffer );
		void do_ip( char *buffer );
		void do_packet(  char *buffer );
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

