#include <types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include "RTL8139Driver.h"
#include "cards.h"

//#include "host.h"
//#include "Packet.h"
//#include "frames.h"

#include "rtl8139_dev.h"
//#include "rtl8139_priv.h"

#define RTL_WRITE_8(rtl, reg, dat) \
	outb( (rtl)->io_port + (reg), dat )
#define RTL_WRITE_16(rtl, reg, dat) \
	outw( (rtl)->io_port + (reg), dat )
#define RTL_WRITE_32(rtl, reg, dat) \
	outl( (rtl)->io_port + (reg), dat )

#define RTL_READ_8(rtl, reg) \
	inb((rtl)->io_port + (reg))
#define RTL_READ_16(rtl, reg) \
	inw((rtl)->io_port + (reg))
#define RTL_READ_32(rtl, reg) \
	inl((rtl)->io_port + (reg))

#define TAILREG_TO_TAIL(in) \
		(uint16)(((uint32)(in) + 16) % 0x10000)
#define TAIL_TO_TAILREG(in) \
		(uint16)((uint16)(in) - 16)
				   


#define MYRT_INTS (RT_INT_PCIERR | RT_INT_RX_ERR | RT_INT_RX_OK | RT_INT_TX_ERR | RT_INT_TX_OK | RT_INT_RXBUF_OVERFLOW)
//#define MYRT_INTS ( RT_INT_RX_OK | RT_INT_TX_OK | RT_INT_TX_ERR )

rtl8139 rtl_real;
rtl8139 *rtl;
int go;


// ***************************************




RTL8139Driver::RTL8139Driver( uint32 base, int irq )
:	Driver( "RTL8139Driver" )
{

	rtl = &rtl_real;
	go = 0;

	request_io( 0x5 );

//	RequestIRQ( 15 );

	this->base = base;
	this->irq = irq;

	Init();
}


RTL8139Driver::~RTL8139Driver()
{
}


void RTL8139Driver::Init()
{
	uint32 temp;


	rtl->irq = this->irq;
	rtl->io_port = this->base;

	printf("%s%p%s%i\n","Initialising realtek driver with a base of ",base," and irq ", irq );


	printf("%s\n","Sending reset.");

// 	time = system_time();
	RTL_WRITE_8(rtl, RT_CHIPCMD, RT_CMD_RESET);
	RTL_READ_8(rtl, RT_CHIPCMD);
	sleep(1);
//	while((RTL_READ_8(rtl, RT_CHIPCMD) & RT_CMD_RESET)) {
//		if(system_time() - time > 1000000) {
//			err = -1;
//			goto err1;
//		}
//	}

	printf("%s\n","It has reset.");

	RequestIRQ( rtl->irq );

	printf("%s\n","Reading MAC address");

	// read the mac address
	rtl->mac_addr[0] = RTL_READ_8(rtl, RT_IDR0);
	rtl->mac_addr[1] = RTL_READ_8(rtl, RT_IDR0 + 1);
	rtl->mac_addr[2] = RTL_READ_8(rtl, RT_IDR0 + 2);
	rtl->mac_addr[3] = RTL_READ_8(rtl, RT_IDR0 + 3);
  	rtl->mac_addr[4] = RTL_READ_8(rtl, RT_IDR0 + 4);
  	rtl->mac_addr[5] = RTL_READ_8(rtl, RT_IDR0 + 5);

  	printf("rtl8139: mac addr %p:%p:%p:%p:%p:%p\n", 
  		rtl->mac_addr[0], rtl->mac_addr[1], rtl->mac_addr[2],
  		rtl->mac_addr[3], rtl->mac_addr[4], rtl->mac_addr[5]);


	// enable writing to the config registers
	RTL_WRITE_8(rtl, RT_CFG9346, 0xc0);

	// reset config 1
	RTL_WRITE_8(rtl, RT_CONFIG1, 0);

	// Enable receive and transmit functions
	RTL_WRITE_8(rtl, RT_CHIPCMD, RT_CMD_RX_ENABLE | RT_CMD_TX_ENABLE);

	// Set Rx FIFO threashold to 1K, Rx size to 64k+16, 1024 byte DMA burst
	RTL_WRITE_32(rtl, RT_RXCONFIG, 0x0000de00);
	
	// Set Tx 1024 byte DMA burst
	RTL_WRITE_32(rtl, RT_TXCONFIG, 0x03000600);

	// Turn off lan-wake and set the driver-loaded bit
	RTL_WRITE_8(rtl, RT_CONFIG1, (RTL_READ_8(rtl, RT_CONFIG1) & ~0x30) | 0x20);
	
	// Enable FIFO auto-clear
	RTL_WRITE_8(rtl, RT_CONFIG4, RTL_READ_8(rtl, RT_CONFIG4) | 0x80);

	// go back to normal mode
	RTL_WRITE_8(rtl, RT_CFG9346, 0);


	rtl->rxbuf = mem_alloc( (64 * 1024 + 16) / 4096 + 1 );
	rtl->txbuf = mem_alloc( (8  * 1024     ) / 4096 + 1 );
	rtl->txbn  = 0;


// create a rx and tx buf
//rtl->rxbuf_region = vm_create_anonymous_region(vm_get_kernel_aspace_id(), "rtl8139_rxbuf", (void **)&rtl->rxbuf,
//REGION_ADDR_ANY_ADDRESS, 64*1024 + 16, REGION_WIRING_WIRED_CONTIG, LOCK_KERNEL|LOCK_RW);
//rtl->txbuf_region = vm_create_anonymous_region(vm_get_kernel_aspace_id(), "rtl8139_txbuf", (void **)&rtl->txbuf,
//REGION_ADDR_ANY_ADDRESS, 8*1024, REGION_WIRING_WIRED, LOCK_KERNEL|LOCK_RW);
							

	// Setup RX buffers
//	vm_get_page_mapping(vm_get_kernel_aspace(), rtl->rxbuf, &temp);
	temp = (uint32) mem_location( rtl->rxbuf );
	RTL_WRITE_32(rtl, RT_RXBUF, temp);

	// Setup TX buffers
//	vm_get_page_mapping(vm_get_kernel_aspace(), rtl->txbuf, &temp);
	temp = (uint32) mem_location( rtl->txbuf );
	RTL_WRITE_32(rtl, RT_TXADDR0, temp);
	RTL_WRITE_32(rtl, RT_TXADDR1, temp + 2*1024);
//	vm_get_page_mapping(vm_get_kernel_aspace(), rtl->txbuf + 4*1024, &temp);
	temp = (uint32) mem_location( rtl->txbuf ) + 4 * 1024;
	RTL_WRITE_32(rtl, RT_TXADDR2, temp);
	RTL_WRITE_32(rtl, RT_TXADDR3, temp + 2*1024);

	// Reset RXMISSED counter
	RTL_WRITE_32(rtl, RT_RXMISSED, 0);
	
	// Enable receiving broadcast and physical match packets
//	RTL_WRITE_32(rtl, RT_RXCONFIG, RTL_READ_32(rtl, RT_RXCONFIG) | 0x0000000a);
	RTL_WRITE_32(rtl, RT_RXCONFIG, RTL_READ_32(rtl, RT_RXCONFIG) | 0x0000000f);

	// Filter out all multicast packets
	RTL_WRITE_32(rtl, RT_MAR0, 0);
	RTL_WRITE_32(rtl, RT_MAR0 + 4, 0);

	// Disable all multi-interrupts
	RTL_WRITE_16(rtl, RT_MULTIINTR, 0);

	RTL_WRITE_16(rtl, RT_INTRMASK, MYRT_INTS);
//	RTL_WRITE_16(rtl, RT_INTRMASK, 0x807f);
	
	// Enable RX/TX once more
	RTL_WRITE_8(rtl, RT_CHIPCMD, RT_CMD_RX_ENABLE | RT_CMD_TX_ENABLE);

	RTL_WRITE_8(rtl, RT_CFG9346, 0);
}
		

static void rtl8139_stop(rtl8139 *rtl)
{
        // stop the rx and tx and mask all interrupts
        RTL_WRITE_8(rtl, RT_CHIPCMD, RT_CMD_RESET);
        RTL_WRITE_16(rtl, RT_INTRMASK, 0);
}
							      

static void rtl8139_resetrx(rtl8139 *rtl)
{
        rtl8139_stop(rtl);

        // reset the rx pointers
        RTL_WRITE_16(rtl, RT_RXBUFTAIL, TAIL_TO_TAILREG(0));
        RTL_WRITE_16(rtl, RT_RXBUFHEAD, 0);

        // start it back up
        RTL_WRITE_16(rtl, RT_INTRMASK, MYRT_INTS);
        // Enable RX/TX once more
        RTL_WRITE_8(rtl, RT_CHIPCMD, RT_CMD_RX_ENABLE | RT_CMD_TX_ENABLE);
}



typedef struct rx_entry {
        volatile uint16 status;
        volatile uint16 len;
        volatile uint8 data[1];
} rx_entry;
					  


#define ETH_ALEN	6		/* Octets in one ethernet addr	 */


// -------------------------------------


void print_ip( uint32 ip )
{
  printf("%i.%i.%i.%i", (ip >> 24) & 0xFF,
			(ip >> 16) & 0xFF, 
			(ip >>  8) & 0xFF, 
			(ip      ) & 0xFF
		   );
}

void print_mac( unsigned char *mac )
{
	 printf("%p:%p:%p:%p:%p:%p",
		 mac[0],
		 mac[1],
		 mac[2],
		 mac[3],
		 mac[4],
		 mac[5] );
}
// -------------------------------------


void RTL8139Driver::Send_DHCP_Request()
{
   unsigned char   tmp_mac[6];
   uint32 tmp_ip;

   Packet     *p1 = new Packet( 1400 );
   IP_Packet  *p2 = new IP_Packet( *p1 );
   UDP_Packet *p3 = new UDP_Packet( *p2 );
   DHCP_Packet *dhcp = new DHCP_Packet( *p3 );

	// TODO:  can't delete p(n) if we've deleted p(n+1)

	memset( tmp_mac, 0xFF, 6 );

	// ETHER LEVEL 
    dhcp->packet()->set_protocol_id( ETHERTYPE_IP );
    dhcp->packet()->set_src_mac( rtl->mac_addr );
    dhcp->packet()->set_dst_mac( tmp_mac );

	// IP LEVEL

    
	// UDP LEVEL

	// DHCP LEVEL



   SendBuffer( (const char*)dhcp->Raw(), dhcp->Length() );
   delete dhcp;
}

//-----------------------------------------


void RTL8139Driver::do_tcp( TCP_Packet *packet )
{
    if ( packet->IsTCP() ) 
      printf("%s (%i -> %i) ",
                 "TCP", 
		 packet->src_port(),
		 packet->dst_port()
		  );

    if ( (packet->flags() & TCP_SYN) == TCP_SYN ) printf("SYN");
    if ( (packet->flags() & TCP_FIN) == TCP_FIN ) printf("FIN");
    if ( (packet->flags() & TCP_ACK) == TCP_ACK ) printf("ACK");
    if ( (packet->flags() & TCP_PUSH) == TCP_PUSH ) printf("PUSH");
    if ( (packet->flags() & TCP_RST) == TCP_RST ) printf("RST");
    if ( (packet->flags() & TCP_URG) == TCP_URG ) printf("URG");
 
    printf("%s","\n");

	if ( packet->dst_port() == 666 ) Send_DHCP_Request();

  delete packet;		
}

void RTL8139Driver::do_dhcp( DHCP_Packet *packet )
{

   switch ( packet->getMessageType() )
   {
	case DHCP_DISCOVER: printf("%s\n","DHCP: DISCOVER"); break;
	case DHCP_OFFER: printf("%s\n","DHCP: OFFER"); break;
	case DHCP_REQUEST: printf("%s\n","DHCP: REQUEST"); break;
	case DHCP_DECLINE: printf("%s\n","DHCP: DECLINE"); break;
	case DHCP_ACK: printf("%s\n","DHCP: ACK"); break;
	case DHCP_NAK: printf("%s\n","DHCP: NAK"); break;
	case DHCP_RELEASE: printf("%s\n","DHCP: RELEASE"); break;
	case DHCP_INFORM: printf("%s\n","DHCP: INFORM"); break;
	default:
		break;

   }

   delete packet;
}


void RTL8139Driver::do_udp( UDP_Packet *packet )
{

   if ( packet->dst_port() == 67 )
   {
      do_dhcp( new DHCP_Packet( *packet ) );
      delete packet;
      return;
   }

   printf("UDP (%i -> %i) %i\n",
   		packet->src_port(),
		packet->dst_port(),
		packet->udp_length() );
   		
  delete packet;		

}

void RTL8139Driver::do_icmp( ICMP_Packet *packet )
{
   unsigned char old_mac[6];
   uint32 old_ip;

   printf("ICMP " );

   switch ( packet->type() )
   {
	case ICMP_ECHOREPLY:
	      printf("%s","ECHOREPLY");
	      break;
	case ICMP_DEST_UNREACH:
	      printf("%s","DEST_UNREACH");
	      break;
	case ICMP_SOURCE_QUENCH:
	      printf("%s","SOURCE_QUENCH");
	      break;
	case ICMP_REDIRECT:
	      printf("%s","REDIRECT");
	      break;
	      
	case ICMP_ECHO:
	      printf("%s","ECHO");

		packet->set_type( ICMP_ECHOREPLY );

		old_ip = packet->ip4()->src_address();
		packet->ip4()->set_src_address( packet->ip4()->dst_address() );
		packet->ip4()->set_dst_address( old_ip );
		packet->ip4()->set_checksum( packet->ip4()->calc_header_checksum() );

		memcpy( old_mac, packet->packet()->src_mac(), 6 );
		packet->packet()->set_src_mac( packet->packet()->dst_mac() );
		packet->packet()->set_dst_mac( old_mac );

		packet->set_checksum( packet->calc_header_checksum() );
		packet->ip4()->set_checksum( packet->ip4()->calc_header_checksum() );

		printf("\n%s %i %s\n","Sending reply",
				    packet->Length(),
				    "bytes");

		SendBuffer( (const char*)packet->Raw(), packet->Length() );
	      
	      break;
	      
	case ICMP_TIME_EXCEEDED:
	      printf("%s","EXCEEDED");
	      break;
	case ICMP_PARAMETERPROB:
	      printf("%s","PARAMETERPROB");
	      break;
	case ICMP_TIMESTAMP:
	      printf("%s","TIMESTAMP");
	      break;
	case ICMP_TIMESTAMPREPLY:
	      printf("%s","TIMESTAMPREPLY");
	      break;
	case ICMP_INFO_REQUEST:
	      printf("%s","INFO_REQUEST");
	      break;
	case ICMP_INFO_REPLY:
	      printf("%s","INFO_REPLY");
	      break;
	case ICMP_ADDRESS:
	      printf("%s","ADDRESS");
	      break;
	case ICMP_ADDRESSREPLY:
	      printf("%s","ADDRESSREPLY");
	      break;

	default:
		printf("type = %i", packet->type() );
		break;
  }

   		
  printf(" [%i,%i] %s\n",
  	   packet->echo_id(),
	   packet->echo_sequence(), 
	   ".");

  delete packet;

}

void RTL8139Driver::do_ip( Packet *packet )
{
   IP_Packet *ip = new IP_Packet( *packet );

    printf("%s","IP ");
	print_ip( ip->src_address() );
    printf("%s"," " );	
	print_ip( ip->dst_address() );
    printf("%s",": " );

    if ( ip->IsUDP() ) do_udp( new UDP_Packet( *ip ) );
    if ( ip->IsTCP() ) do_tcp( new TCP_Packet( *ip ) );
    if ( ip->IsICMP() ) do_icmp( new ICMP_Packet( *ip ) );

   delete ip;
}

void RTL8139Driver::do_arp( Packet *packet )
{
  ARP_Packet *arp = new ARP_Packet( *packet );

    printf("%s", "ARP: who has " );
    print_ip( arp->dst_ip() );
    printf("%s", ", tell ");
    print_ip( arp->src_ip() );
    printf("%s\n",".");

    arp->make_arp_reply( rtl->mac_addr );

    SendBuffer( (char*)(packet->Raw()), packet->Length() );

   delete arp;
}


// -------------------------------------

void RTL8139Driver::do_packet(  char *buffer, int length )
{
   Packet *packet = new Packet( buffer, length );

//   printf("%s","PACKET: ");
//   print_mac( packet->src_mac() );
//   printf("%s","\n    --> ");
//   print_mac( packet->dst_mac() );
//   printf("%s\n",".");


   switch ( packet->protocol_id()  )
   {
      case ETHERTYPE_IP:
         do_ip( packet );
	 break;
      case ETHERTYPE_ARP:
         do_arp( packet );
	 break;
      default:
	 break;	 
   }

  delete packet;
  
}

int RTL8139Driver::SendBuffer( const char *ptr, int len )
{
	int i;
	int txbn;
	int state;

restart:

	/* wait for clear-to-send */
	if(!(RTL_READ_32(rtl, RT_TXSTATUS0 + rtl->txbn*4) & RT_TX_HOST_OWNS)) {
		printf("%s\n","rtl8139_xmit: no txbuf free.");
		goto restart;
	}	

	memcpy((void*)((uint32)rtl->txbuf + rtl->txbn * 0x800), ptr, len);
	if(len < 64) 
		len = 64;

	RTL_WRITE_32(rtl, RT_TXSTATUS0 + (rtl->txbn)*4, len | 0x80000);
	if(++rtl->txbn >= 4)
		rtl->txbn = 0;

}


void RTL8139Driver::IntRX()
{
        rx_entry *entry;
        uint32 tail;
        uint16 len;
        int rc;

	char buf[16384];
	int buf_len = 16384;
					

	if(!(RTL_READ_8(rtl, RT_CHIPCMD) & RT_CMD_RX_BUF_EMPTY)) {
		// Should release semaphore here or something..
	}


	disable_interrupts();

	tail = TAILREG_TO_TAIL(RTL_READ_16(rtl, RT_RXBUFTAIL));

	if ( tail == RTL_READ_16(rtl, RT_RXBUFHEAD)) 
	{
		enable_interrupts();
		printf("%s\n","tail == RTL and going back");
		return;
	}

        if(RTL_READ_8(rtl, RT_CHIPCMD) & RT_CMD_RX_BUF_EMPTY) 
	{
		enable_interrupts();
		printf("%s\n","CMD & EMPTY... returning");
		return;
        }
						      

        entry = (rx_entry *)((uint8 *)rtl->rxbuf + tail);

        if(entry->len == 0xfff0) 
	{
		enable_interrupts();
		printf("%s\n","unfinished buffer?");
		return;
        }
						      
	// figure the len that we need to copy
        len = entry->len - 4; // minus the crc

#define ETHERNET_HEADER_SIZE (6+6+2)
#define ETHERNET_MAX_SIZE (ETHERNET_HEADER_SIZE+1500)
#define ETHERNET_MIN_SIZE (ETHERNET_HEADER_SIZE+46)


        if((entry->status & RT_RX_STATUS_OK) == 0 || len > ETHERNET_MAX_SIZE) 
	{
                rtl8139_resetrx(rtl);
		enable_interrupts();
		printf("%s\n","ok and too big");
		return;
        }
								  

        // copy the buffer
        if(len > buf_len) {
                printf("%s%i%s%i%s\n","rtl8139_rx: packet too large for buffer (len ", len, ", buf_len ", (long)buf_len,")\n");
                RTL_WRITE_16(rtl, RT_RXBUFTAIL, TAILREG_TO_TAIL(RTL_READ_16(rtl, RT_RXBUFHEAD)));
                goto out;
        }
        if(tail + len > 0xffff) {
                printf("%s\n","packet wraps around.");
                memcpy(buf, (const void *)&entry->data[0], 0x10000 - (tail + 4));
                memcpy((uint8 *)buf + 0x10000 - (tail + 4), (const void *)rtl->rxbuf, len - (0x10000 - (tail + 4)));
        } else {
//		printf("%s\n","copy");
                memcpy(buf, (const void *)&entry->data[0], len);
        }
        rc = len;

        // calculate the new tail
        tail = ((tail + entry->len + 4 + 3) & ~3) % 0x10000;
//      dprintf("new tail at 0x%x, tailreg will say 0x%x\n", tail, TAIL_TO_TAILREG(tail));
        RTL_WRITE_16(rtl, RT_RXBUFTAIL, TAIL_TO_TAILREG(tail));

        if(tail != RTL_READ_16(rtl, RT_RXBUFHEAD)) {
                // we're at last one more packet behind
//                release_sem = true;
        }

out:
	enable_interrupts();

	do_packet( buf, len );


//#if 0
/*	{
	        int i;
	        printf("%s%p (%i)\n","RX ",buf, len);
	
	        printf("%s\n","dumping packet:");
	        for(i=0; i<len; i++) {
	                if(i%8 == 0)
	                        printf("\n");
	                printf("0x%p ", buf[i]);
	        }
	        printf("\n");
	}
*/
//#endif


}

void RTL8139Driver::IntTX( uint16 status )
{
	uint32 txstat;
	int i;

	// transmit ok
//	printf("%s\n","Transmit success");

	for(i=0; i<4; i++) {
//		if(grtl->last_txbn == grtl->txbn)
//			break;
		txstat = RTL_READ_32(rtl, RT_TXSTATUS0 + rtl->txbn*4);
		if(txstat & RT_TX_HOST_OWNS) {
//			printf("%s\n","host now owns the buffer\n");
		} else {
//			printf("%s\n","host no own\n");
			break;
		}
//		if(++grtl->last_txbn >= 4)
//			grtl->last_txbn = 0;
	}
}



int32 RTL8139Driver::IRQ( int32 irq )
{
	int rc;

	if ( go  ==  0 )
	{
		go = 1;
	  	request_io(0x60);
	}

//	printf("%s(%i)%s\n","IRQ",irq," for realtek 8139 driver received." );

	
	// Disable interrupts
	RTL_WRITE_16(rtl, RT_INTRMASK, 0);

	for(;;) {

		uint16 status = RTL_READ_16(rtl, RT_INTRSTATUS);
//		printf("%s%p\n","INTRSTATUS = ",status);
		if(status)
			RTL_WRITE_16(rtl, RT_INTRSTATUS, status);
		else
			break;

		if(status & RT_INT_TX_OK)
		{
			IntTX(status);
		}
		if ( status & RT_INT_TX_ERR) 
		{
			printf("%s\n","interrupt tx ERR");
		}
		if(status & RT_INT_RX_ERR)
		{
			printf("%s%s\n","interrupt rx ERR");
		}
		if ( status & RT_INT_RX_OK) 
		{
//			printf("%s\n","interrupt rx OK");
			IntRX();
		}
		
		if(status & RT_INT_RXBUF_OVERFLOW) {
			printf("%s","RX buffer overflow!\n");
			printf("buf 0x%p, head 0x%p, tail 0x%p\n", 
				RTL_READ_32(rtl, RT_RXBUF), RTL_READ_16(rtl, RT_RXBUFHEAD), RTL_READ_16(rtl, RT_RXBUFTAIL));
			RTL_WRITE_32(rtl, RT_RXMISSED, 0);
			RTL_WRITE_16(rtl, RT_RXBUFTAIL, TAILREG_TO_TAIL(RTL_READ_16(rtl, RT_RXBUFHEAD)));
		}
		if(status & RT_INT_RXFIFO_OVERFLOW) {
			printf("%s","RX fifo overflow!\n");
		}
		if(status & RT_INT_RXFIFO_UNDERRUN) {
			printf("%s","RX fifo underrun\n");
		}

		
	}

	// reenable interrupts
	RTL_WRITE_16(rtl, RT_INTRMASK, MYRT_INTS);

//	printf("%s\n","IRQ done.");

	return 0;
}



