#include <types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include "RTL8139Driver.h"
#include "cards.h"

//#include "frames.h"
#include "host.h"
//#include "Packet.h"

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
#define MYRT_INTS ( RT_INT_RX_OK | RT_INT_TX_OK | RT_INT_TX_ERR )

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


#define	ETHERTYPE_IP		0x0800		/* IP */
#define	ETHERTYPE_ARP		0x0806		/* Address resolution */
#define	ETHERTYPE_REVARP	0x8035		/* Reverse ARP */

#define	ARPOP_REQUEST	1		/* ARP request.  */
#define	ARPOP_REPLY	2		/* ARP reply.  */
#define	ARPOP_RREQUEST	3		/* RARP request.  */
#define	ARPOP_RREPLY	4		/* RARP reply.  */
#define	ARPOP_InREQUEST	8		/* InARP request.  */
#define	ARPOP_InREPLY	9		/* InARP reply.  */
#define	ARPOP_NAK	10		/* (ATM)ARP NAK.  */


#define ARPHRD_ETHER 	1		/* Ethernet 10/100Mbps.  */

enum
{
    IPPROTO_IP = 0,	   /* Dummy protocol for TCP.  */
#define IPPROTO_IP		IPPROTO_IP
    IPPROTO_HOPOPTS = 0,   /* IPv6 Hop-by-Hop options.  */
#define IPPROTO_HOPOPTS		IPPROTO_HOPOPTS
    IPPROTO_ICMP = 1,	   /* Internet Control Message Protocol.  */
#define IPPROTO_ICMP		IPPROTO_ICMP
    IPPROTO_IGMP = 2,	   /* Internet Group Management Protocol. */
#define IPPROTO_IGMP		IPPROTO_IGMP
    IPPROTO_IPIP = 4,	   /* IPIP tunnels (older KA9Q tunnels use 94).  */
#define IPPROTO_IPIP		IPPROTO_IPIP
    IPPROTO_TCP = 6,	   /* Transmission Control Protocol.  */
#define IPPROTO_TCP		IPPROTO_TCP
    IPPROTO_EGP = 8,	   /* Exterior Gateway Protocol.  */
#define IPPROTO_EGP		IPPROTO_EGP
    IPPROTO_PUP = 12,	   /* PUP protocol.  */
#define IPPROTO_PUP		IPPROTO_PUP
    IPPROTO_UDP = 17,	   /* User Datagram Protocol.  */
#define IPPROTO_UDP		IPPROTO_UDP
    IPPROTO_IDP = 22,	   /* XNS IDP protocol.  */
#define IPPROTO_IDP		IPPROTO_IDP
    IPPROTO_TP = 29,	   /* SO Transport Protocol Class 4.  */
#define IPPROTO_TP		IPPROTO_TP
    IPPROTO_IPV6 = 41,     /* IPv6 header.  */
#define IPPROTO_IPV6		IPPROTO_IPV6
    IPPROTO_ROUTING = 43,  /* IPv6 routing header.  */
#define IPPROTO_ROUTING		IPPROTO_ROUTING
    IPPROTO_FRAGMENT = 44, /* IPv6 fragmentation header.  */
#define IPPROTO_FRAGMENT	IPPROTO_FRAGMENT
    IPPROTO_RSVP = 46,	   /* Reservation Protocol.  */
#define IPPROTO_RSVP		IPPROTO_RSVP
    IPPROTO_GRE = 47,	   /* General Routing Encapsulation.  */
#define IPPROTO_GRE		IPPROTO_GRE
    IPPROTO_ESP = 50,      /* encapsulating security payload.  */
#define IPPROTO_ESP		IPPROTO_ESP
    IPPROTO_AH = 51,       /* authentication header.  */
#define IPPROTO_AH		IPPROTO_AH
    IPPROTO_ICMPV6 = 58,   /* ICMPv6.  */
#define IPPROTO_ICMPV6		IPPROTO_ICMPV6
    IPPROTO_NONE = 59,     /* IPv6 no next header.  */
#define IPPROTO_NONE		IPPROTO_NONE
    IPPROTO_DSTOPTS = 60,  /* IPv6 destination options.  */
#define IPPROTO_DSTOPTS		IPPROTO_DSTOPTS
    IPPROTO_MTP = 92,	   /* Multicast Transport Protocol.  */
#define IPPROTO_MTP		IPPROTO_MTP
    IPPROTO_ENCAP = 98,	   /* Encapsulation Header.  */
#define IPPROTO_ENCAP		IPPROTO_ENCAP
    IPPROTO_PIM = 103,	   /* Protocol Independent Multicast.  */
#define IPPROTO_PIM		IPPROTO_PIM
    IPPROTO_COMP = 108,	   /* Compression Header Protocol.  */
#define IPPROTO_COMP		IPPROTO_COMP
    IPPROTO_SCTP = 132,	   /* Stream Control Transmission Protocol.  */
#define IPPROTO_SCTP		IPPROTO_SCTP
    IPPROTO_RAW = 255,	   /* Raw IP packets.  */
#define IPPROTO_RAW		IPPROTO_RAW
    IPPROTO_MAX
};

struct ethhdr 
{
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
};

struct arphdr
  {
    unsigned short int ar_hrd;		/* Format of hardware address.  */
    unsigned short int ar_pro;		/* Format of protocol address.  */
    unsigned char ar_hln;		/* Length of hardware address.  */
    unsigned char ar_pln;		/* Length of protocol address.  */
    unsigned short int ar_op;		/* ARP opcode (command).  */

    unsigned char __ar_sha[ETH_ALEN];	/* Sender hardware address.  */
    unsigned char __ar_sip[4];		/* Sender IP address.  */
    unsigned char __ar_tha[ETH_ALEN];	/* Target hardware address.  */
    unsigned char __ar_tip[4];		/* Target IP address.  */
  };

struct iphdr
  {
    unsigned int ihl:4;
    unsigned int version:4;
    uint8 tos;
    uint16 tot_len;
    uint16 id;
    uint16 frag_off;
    uint8 ttl;
    uint8 protocol;
    uint16 check;
    uint32 saddr;
    uint32 daddr;
  };



// -------------------------------------


void print_ip( unsigned char ip[4] )
{
  printf("%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3] );
}

void print_mac( unsigned char mac[ETH_ALEN] )
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

void RTL8139Driver::ARP_Reply( unsigned char mac[6], 
			       unsigned char tip[4],
		               unsigned char ip[4] )
{
   char *buffer;
   struct ethhdr *eth;
   struct arphdr *arp;

   buffer = (char*)malloc(sizeof(struct ethhdr) + sizeof(struct arphdr));

   eth = (struct ethhdr*)buffer;
   arp = (struct arphdr*)(buffer + sizeof(struct ethhdr));


	memcpy( eth->h_dest, mac, 6 );
	memcpy( eth->h_source, rtl->mac_addr, 6 );
	eth->h_proto = htons( ETHERTYPE_ARP ); 

	arp->ar_op = htons( ARPOP_REPLY );

	memcpy( arp->__ar_sha, rtl->mac_addr, 6 );
	memcpy( arp->__ar_tha, mac, 6 );
	memcpy( arp->__ar_tip, tip, 4 );
	memcpy( arp->__ar_sip, ip, 4 );

	arp->ar_hrd = htons(1);
	arp->ar_pro = htons(2048);
	arp->ar_hln = ETH_ALEN;
	arp->ar_pln = 4;

    SendBuffer( buffer, sizeof( struct ethhdr) + sizeof( struct arphdr ) );

    free( buffer );
}

// -------------------------------------
// -------------------------------------

void RTL8139Driver::do_arp( char *buffer )
{
   struct arphdr *arp;
   unsigned char ip[4];

   ip[0] = 192;
   ip[1] = 168;
   ip[2] = 0;
   ip[3] = 50;

   arp = (struct arphdr*)(buffer + sizeof(struct ethhdr));

   switch ( htons(arp->ar_op) )
   {
	case ARPOP_REQUEST:  
		printf("%s\n","ARP REQUEST"); 
		printf("%s","who has " );
		print_ip( arp->__ar_tip );
		printf("%s"," tell " );
		print_ip( arp->__ar_sip );
		printf("%s","\n");

		ARP_Reply( arp->__ar_sha,
			   arp->__ar_sip,
			   arp->__ar_tip );
			   
		break;
		
	case ARPOP_REPLY:  printf("%s\n","ARP REPLY"); break;
	case ARPOP_RREQUEST:  printf("%s\n","ARP RREQUEST"); break;
	case ARPOP_RREPLY:  printf("%s\n","ARP RREPLY"); break;
	case ARPOP_InREQUEST:  printf("%s\n","ARP InREQUEST"); break;
	case ARPOP_InREPLY:  printf("%s\n","ARP InREPLY"); break;
	case ARPOP_NAK:  printf("%s\n","ARP NAK"); break;
	default:
	  printf("%s\n","UNKNOWN ARP OPCODE");
	  break;
   }
}

// ----------------------------------------

struct icmphdr
{
  uint8 type;		/* message type */
  uint8 code;		/* type sub-code */
  uint16 checksum;
  union
  {
    struct
    {
      uint16	id;
      uint16	sequence;
    } echo;			/* echo datagram */
    uint32	gateway;	/* gateway address */
    struct
    {
      uint16	__unused;
      uint16	mtu;
    } frag;			/* path mtu discovery */
  } un;
};

#define ICMP_ECHOREPLY		0	/* Echo Reply			*/
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable	*/
#define ICMP_SOURCE_QUENCH	4	/* Source Quench		*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO		8	/* Echo Request			*/
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded		*/
#define ICMP_PARAMETERPROB	12	/* Parameter Problem		*/
#define ICMP_TIMESTAMP		13	/* Timestamp Request		*/
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply		*/
#define ICMP_INFO_REQUEST	15	/* Information Request		*/
#define ICMP_INFO_REPLY		16	/* Information Reply		*/
#define ICMP_ADDRESS		17	/* Address Mask Request		*/
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply		*/

void RTL8139Driver::IP_ICMP( char *buffer )
{
   struct ethhdr *eth;
   struct iphdr *ip;
   struct icmphdr *icmp;
   char *data;

   int i;
   int size;

   printf("%s\n","IP_ICMP");

   eth   = (struct ethhdr*) buffer;
   ip    = (struct iphdr*)( buffer + sizeof( struct ethhdr ) );
   icmp  = (struct icmphdr*)( ip + sizeof( struct iphdr ) );
   data  = (char*)( icmp + sizeof( struct icmphdr ) );
   
   size = ip->tot_len - (sizeof(struct iphdr) + sizeof(struct icmphdr));
   
	printf("%s(%i) %i %i: ","ICMP",
	                        size,
				icmp->type, icmp->code );

//   if ( icmp->type == ICMP_ECHO )
//   {

     if (  size > 8 ) size = 8;

     for ( i = 0; 
           i <  size ;
  	   i++ ) 
     {
       printf("%c", data[i] );
     }
//   }

   printf("%s","\n");

}

void RTL8139Driver::IP_TCP( char *buffer )
{
	printf("%s\n","TCP packet");
}

void RTL8139Driver::IP_UDP( char *buffer )
{
	printf("%s\n","UDP packet");
}

// ----------------------------------------

void RTL8139Driver::do_ip( char *buffer )
{
   struct ethhdr *eth;
   struct iphdr *ip;

   eth = (struct ethhdr*) buffer;
   ip  = (struct iphdr*)( buffer + sizeof( struct ethhdr ) );

   printf("%s\n","do_ip");

   switch (ip->protocol)
   {
       case IPPROTO_ICMP:
                IP_ICMP( buffer );
       		break;
		
       case IPPROTO_TCP:
                IP_TCP( buffer );
       		break;

       case IPPROTO_UDP:
                IP_UDP( buffer );
       		break;
		
       default:
	   printf("%s%i: %i %i ",
	   	  "UNKNOWN IP",
		  ip->version,
		  ip->ttl,
		  ip->protocol );
 
	   print_ip( (unsigned char*) &(ip->saddr) );
	   printf("%s"," -> " );
	   print_ip( (unsigned char*) &(ip->daddr) );
	   printf("%s","\n" );
         break;
   }

   /*
    unsigned int ihl:4;
    unsigned int version:4;
    uint8 tos;
    uint16 tot_len;
    uint16 id;
    uint16 frag_off;
    uint8 ttl;
    uint8 protocol;
    uint16 check;
    uint32 saddr;
    uint32 daddr;
 */


}

void RTL8139Driver::do_packet(  char *buffer )
{
   struct ethhdr *packet;

   packet = (struct ethhdr*) buffer;

   switch ( htons(packet->h_proto) )
   {
      case ETHERTYPE_IP:
         do_ip( buffer );
	 break;
      case ETHERTYPE_ARP:
         do_arp( buffer );
	 break;
      default:
	 printf("%s %i %p:%p:%p:%p:%p:%p\n",
	         "UNKNOWN ",
		 packet->h_proto,
		 packet->h_source[0],
		 packet->h_source[1],
		 packet->h_source[2],
		 packet->h_source[3],
		 packet->h_source[4],
		 packet->h_source[5] );
	 break;	 
   }

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

	do_packet( buf );


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
//			printf("buf 0x%x, head 0x%x, tail 0x%x\n", 
//				RTL_READ_32(rtl, RT_RXBUF), RTL_READ_16(rtl, RT_RXBUFHEAD), RTL_READ_16(rtl, RT_RXBUFTAIL));
//			RTL_WRITE_32(rtl, RT_RXMISSED, 0);
//			RTL_WRITE_16(rtl, RT_RXBUFTAIL, TAILREG_TO_TAIL(RTL_READ_16(rtl, RT_RXBUFHEAD)));
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



