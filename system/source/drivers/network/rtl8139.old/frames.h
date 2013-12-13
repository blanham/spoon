#ifndef _NET_SERVER_FRAMES_H
#define _NET_SERVER_FRAMES_H

#include <types.h>

#define ETHERTYPE_PUP           0x0200         
#define ETHERTYPE_IP            0x0800        
#define ETHERTYPE_ARP           0x0806       
#define ETHERTYPE_REVARP        0x8035      


struct ethernet_header {
    char	dst_mac[6],
        	src_mac[6];
    unsigned protocol_id:16;
} __attribute__ ((__packed__)); 
	


#define ARPOP_REQUEST   1
#define ARPOP_REPLY     2
#define ARPOP_RREQUEST  3
#define ARPOP_RREPLY    4
#define ARPOP_InREQUEST 8
#define ARPOP_InREPLY   9
#define ARPOP_NAK       10


#define ARPHRD_ETHER	1
#define ARPPROT_IP	0x800

struct arp_header {
  unsigned	hardware_type:16,
		protocol_type:16,
 	        hardware_addr_length:8, 
		protocol_addr_length:8,
		arp_operation:16;
		
  char 		sender_mac[6];
  char		sender_ip[4];
  char		dest_mac[6];
  char		dest_ip[4];
} __attribute__ ((__packed__));


struct ip_header {
  unsigned	 hlen:4,
  		 version:4,
 		 tos:8,
		 total_length:16,
		 identification:16,
		 flags:3,
		 fragmentation_offset:13,
		 time_to_live:8,
		 protocol:8,
		 header_checksum:16,
		 source_address:32,
		 destination_address:32;
} __attribute__ ((__packed__));
 

#define    IPPROTO_ICMP 	1
#define    IPPROTO_TCP 		6
#define    IPPROTO_UDP 		17



struct tcp_header {
unsigned	source_port:16,		// 0 (2)
		destination_port:16,	// 2 (2)
		sequence_number:32,	// 4 (4)
		ack_number:32,		// 8 (4)
		resv:4,			// 12.0 (0.5)
		data_offset:4,		// 12.5 (0.5) number of 32-bit words in header.
		flags:8,		// 13 (1)
		window_size:16,		// 14 (2)
		checksum:16,		// 16 (2)
		urgent_pointer:16;	// 18 (2)
} __attribute__ ((__packed__));



struct udp_header {
unsigned	source_port:16,
		destination_port:16,
		udp_length:16,
		checksum:16;
} __attribute__ ((__packed__));


struct dhcp_header {
	uint8	op,
		htype,
		hlen,
		hops;			// 4

	uint32	xid;			// 4
	uint16	secs;
	uint16  flags;			// 4
	uint32  ciaddr;			// 4
	uint32	yiaddr;			// 4
	uint32	siaddr;			// 4
	uint32	giaddr;			// 4   = 28

	uint8	chaddr[16];
	uint8	sname[64];
	uint8	file[128];
	uint8   options[8]; // 8 is safe as a minimum because cookie+message_type+end = 8
} __attribute__ ((__packed__));


#endif


