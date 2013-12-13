#ifndef _NET_SERVER_PACKET_H
#define _NET_SERVER_PACKET_H

#include <types.h>
#include <stdlib.h>
#include <string.h>
#include "frames.h"

#include "host.h"

class Packet
{

   protected:
      void *packet_data;
      unsigned int length;
     
     
   private:
      struct ethernet_header *eth;


   public:
     Packet( void *data, unsigned int len )
       : packet_data(data), length(len), 
         eth((struct ethernet_header*)data)
       { 
       }
       
     Packet( Packet &pkt )
       : length( pkt.length )
       {
         packet_data = malloc( pkt.length );
	 memcpy( packet_data, pkt.packet_data, pkt.length );
	 eth = (struct ethernet_header*)packet_data;
       }
       
    ~Packet()
       { free( packet_data ); }

     virtual char *src_mac()
     		{ return eth->src_mac; }
     virtual char *dst_mac()
     		{ return eth->dst_mac; }
     uint32 protocol_id()
     		{ return ntohs(eth->protocol_id); }

     virtual void* payload()
      {
        return ( (void*)((uint32)packet_data + 
	                 sizeof(struct ethernet_header) ) );
      }


     bool IsRARP()
     	{ return (protocol_id() == ntohs(ETHERTYPE_REVARP) ); } 
     bool IsARP()
     	{ return (protocol_id() == ntohs(ETHERTYPE_ARP)); } 
     bool IsIP()
     	{ return (protocol_id() == ntohs(ETHERTYPE_IP)); } 


     unsigned int Length() { return length; }
     void* Raw() { return packet_data; }



	virtual void set_src_mac( char* mac ) 
		{ memcpy( eth->src_mac, mac, 6 ); }
	virtual void set_dst_mac( char* mac )
		{ memcpy( eth->dst_mac, mac, 6 ); }
	virtual void set_protocol_id( uint32 id )
		{ eth->protocol_id = htons(id); }
		
};


class ARP_Packet : public Packet
{

   private:
      struct arp_header *arp;

   public:
      ARP_Packet( Packet &pkt )
       : Packet( pkt ),  
         arp( (struct arp_header*) pkt.payload() )
      {
      }
       
	unsigned int hardware_type() 
	  { return ntohs(arp->hardware_type); }
	unsigned int protocol_type() 
	  { return ntohs(arp->protocol_type); }
	unsigned int hardware_addr_len() 
	  { return arp->hardware_addr_length; }
	unsigned int protocol_addr_len() 
	  { return arp->protocol_addr_length; }
	unsigned int arp_operation() 
	  { return ntohs(arp->arp_operation); }

	void set_hardware_type( uint16 p ) 
	  { arp->hardware_type = htons(p); }
	void set_protocol_type( uint16 p ) 
	  { arp->protocol_type = htons(p); }
	void set_hardware_addr_len( uint8 p) 
	  { arp->hardware_addr_length = p; }
	void set_protocol_addr_len( uint8 p ) 
	  { arp->protocol_addr_length = p; }
	void set_arp_operation( uint16 p ) 
	  { arp->arp_operation = htons(p); }


	char *src_mac() { return arp->sender_mac; }
	char *dst_mac() { return arp->dest_mac; }

	virtual void set_src_mac( char* mac ) 
		{ memcpy( arp->sender_mac, mac, 6 ); 
		  Packet::set_src_mac( mac );
		}
	virtual void set_dst_mac( char* mac )
		{ memcpy( arp->dest_mac, mac, 6 ); 
		  Packet::set_dst_mac( mac );
		}

	uint32 src_ip() { 
	   return (uint32)( (arp->sender_ip[0]  << 24) |
	                    (arp->sender_ip[1]  << 16) |
			    (arp->sender_ip[2]  << 8 ) |
			    (arp->sender_ip[3]       )    );
	}

	uint32 dst_ip() { 
	   return (uint32)( (arp->dest_ip[0]  << 24) |
	                    (arp->dest_ip[1]  << 16) |
			    (arp->dest_ip[2]  << 8 ) |
			    (arp->dest_ip[3]       )    );
	}

	void set_src_ip( uint32 ip) {
		arp->sender_ip[0] = (ip >> 24) & 0xFF;
		arp->sender_ip[1] = (ip >> 16) & 0xFF;
		arp->sender_ip[2] = (ip >> 8 ) & 0xFF;
		arp->sender_ip[3] = (ip      ) & 0xFF;
	}
	
	void set_dst_ip(unsigned int ip) {
		arp->dest_ip[0] = (ip >> 24) & 0xFF;
		arp->dest_ip[1] = (ip >> 16) & 0xFF;
		arp->dest_ip[2] = (ip >> 8 ) & 0xFF;
		arp->dest_ip[3] = (ip      ) & 0xFF;
	}


     virtual void* payload()
      {
        return ( (void*)((uint32)packet_data + 
	                 sizeof(struct ethernet_header) +
			 sizeof(struct arp_header) ) );
      }


      void make_arp_reply( char *mac )
      {
         uint32 tmp;

	 set_dst_mac( src_mac() );
	 set_src_mac( mac );

	 tmp = src_ip();
	 set_src_ip( dst_ip() );
	 set_dst_ip( tmp );

	 arp->arp_operation = htons( ARPOP_REPLY ); 
      }


      void make_rarp_reply( char *mac, uint32 ip )
      {
	
	// put the sender's address in the 
	// destination, because we're sending it back.
	set_dst_mac(src_mac());

	//the source is us.
	set_src_mac(mac);

	// put sender's ip in destination
	set_dst_ip(src_ip());

	//the source is us.
	set_src_ip(ip);

	// change arp_operation to reply
	 arp->arp_operation = htons( ARPOP_RREPLY ); 
      }


	bool isEthernetArp()	
	   { return arp->hardware_type==htons(ARPHRD_ETHER); }
	bool isIPArp()			
	   { return arp->protocol_type==htons(ARPPROT_IP); }

	bool isArpRequest()		
	   { return arp->arp_operation==htons(ARPOP_REQUEST); }
	bool isArpReply()		
	   { return arp->arp_operation==htons(ARPOP_REPLY); }

	bool isRarpRequest()	
	   { return arp->arp_operation==htons(ARPOP_RREQUEST); }
	bool isRarpReply()		
	   { return arp->arp_operation==htons(ARPOP_RREPLY); }


};


class IP_Packet : public Packet
{
   private:
      struct ip_header* iphdr;
      void *data;
      void *options_block;

   public:
      IP_Packet( Packet &pkt )
       : Packet( pkt ),  
         iphdr( (struct ip_header*) pkt.payload() ),
	 options_block( (void*)((uint32)iphdr + 20 ) ),
	 data( (void*)((uint32)iphdr + (iphdr->hlen << 2)) )
      {
      }
       
	unsigned int version()	{ return iphdr->version; 	}
	unsigned int hlen()	{ return iphdr->hlen;		}
	unsigned int olen()	{ return iphdr->hlen - 5;  }
	unsigned int tos()	{ return iphdr->tos;		}
	unsigned int total_length() 	{ return ntohs(iphdr->total_length); 	 }
	unsigned int identification()	{ return ntohs(iphdr->identification); }
	unsigned int flags()	  		{ return iphdr->flags;	}
	unsigned int fragmentation_offset()	{ return ntohs((unsigned short)iphdr->fragmentation_offset); }
	unsigned int time_to_live()		{ return iphdr->time_to_live; 	}
	unsigned int protocol()			{ return iphdr->protocol;		}
	unsigned int checksum()			{ return iphdr->header_checksum; }
	unsigned int src_address()		{ return iphdr->source_address;  }
	unsigned int dst_address()		{ return iphdr->destination_address; }
	
	void * payload() { return data; } 
	void * options() { return options_block; }

       

	// 

	void set_src_address(unsigned int new_source) {
		iphdr->source_address = new_source;
	}
	
	void set_dst_address(unsigned int new_dest) {
		iphdr->destination_address = new_dest;
	}
	
	void set_total_length(uint16 length) {
	 	iphdr->total_length = htons(length);
	}
	
	void set_checksum(uint16 _checksum)
	 { iphdr->header_checksum = _checksum; }

//	uint16 calc_header_checksum();

	bool isIP4()  { return iphdr->version == 4; }
	bool isTCP()  { return iphdr->protocol == IPPROTO_TCP; }
	bool isUDP()  { return iphdr->protocol == IPPROTO_UDP; }
	bool isICMP() { return iphdr->protocol == IPPROTO_ICMP; }


};


#endif


