#ifndef _PartitionNode_H
#define _PartitionNode_H

#include <types.h>
#include <spoon/storage/DeviceNode.h>

#include "ataio.h"


struct partition_descriptor
{
    uint8 bootable;
    uint8 begin_cylinder;
    uint8 begin_head;
    uint8 begin_sector;
    uint8 type;
    uint8 end_cylinder;
    uint8 end_head;
    uint8 end_sector;
    uint32 start_sector;
    uint32 size;
};




#define DELAY400NS  { pio_inbyte( CB_ASTAT ); pio_inbyte( CB_ASTAT );  \
                      pio_inbyte( CB_ASTAT ); pio_inbyte( CB_ASTAT ); }

class PartitionNode : public DeviceNode
{
		
   public:
      PartitionNode( int64 start, int64 end,
                     char *node, unsigned int base, int device )
      : DeviceNode( node ), _base(base), _device(device),
		_start(start), 
		_end(end),
        pio_memory_seg(0), pio_xfer_width(16),
		reg_atapi_delay_flag(0), int_use_intr_flag(0),
		tmr_time_out(20L)
       {
         pio_set_iobase_addr( base, base + 0x200 );
	   	 reg_config_info[0] = REG_CONFIG_TYPE_NONE;
	   	 reg_config_info[1] = REG_CONFIG_TYPE_NONE;
       }
       
     ~PartitionNode()
      {
      	Quit();
      }


       virtual void Install()
        {
	  smk_request_io( 0x60 );
	  reg_reset( 0, _device );
	  DeviceNode::Install();
	}
	
       virtual void Uninstall()
        {
	  DeviceNode::Uninstall();
        }

       
       virtual int read( int64 pos, void *buffer, int len );
       virtual int write( int64 pos, void *buffer, int len );

	
	bool Present();


   private:
        void pio_set_iobase_addr( unsigned int base1, unsigned int base2 );

	unsigned char pio_inbyte( unsigned int addr );
	void pio_outbyte( unsigned int addr, unsigned char data );
	unsigned int pio_inword( unsigned int addr );
	void pio_outword( unsigned int addr, unsigned int data );


	void pio_rep_inbyte( unsigned int addrDataReg, void* buffer,
		                     long byteCnt );
	void pio_rep_outbyte( unsigned int addrDataReg, void* buffer,
		                      long byteCnt );

	void pio_rep_indword( unsigned int addrDataReg, void* buffer,
		                      unsigned int dwordCnt );


	void pio_rep_outdword( unsigned int addrDataReg, void* buffer,
		                       unsigned int dwordCnt );


	void pio_rep_inword( unsigned int addrDataReg, void* buffer,
		                     unsigned int wordCnt );


	void pio_rep_outword( unsigned int addrDataReg, void* buffer,
		                      unsigned int wordCnt );


   private:
	int reg_reset( int skipFlag, int devRtrn );

   private:
	int reg_pio_data_in_lba( int dev, int cmd,
                         int fr, int sc,
                         long lba,
			 void* buffer,
                         int numSect, int multiCnt );
	int reg_pio_data_in( int dev, int cmd,
                     int fr, int sc,
                     unsigned int cyl, int head, int sect,
	             void* buffer,
                     int numSect, int multiCnt );
	int reg_pio_data_out_lba( int dev, int cmd,
                         int fr, int sc,
                         long lba,
			 void* buffer,
                         int numSect, int multiCnt );
	int reg_pio_data_out( int dev, int cmd,
                     int fr, int sc,
                     unsigned int cyl, int head, int sect,
	             void* buffer,
                     int numSect, int multiCnt );




   private:
	void sub_zero_return_data( void );
	void sub_atapi_delay( int dev );
	void sub_xfer_delay( void );
	void sub_trace_command( void );
	int  sub_select( int dev );
	void reg_wait_poll( int we, int pe );

   private:
	int tmr_chk_timeout( void );
	void tmr_set_timeout( void );
	unsigned int tmr_read_bios_timer( void );

   private:
      unsigned int _base;
      int _device;
      int64 _start;
      int64 _end;

	struct REG_CMD_INFO reg_cmd_info;
	unsigned int pio_memory_seg;
	unsigned int pio_base_addr1;
	unsigned int pio_base_addr2;
	unsigned int pio_reg_addrs[10];
	int reg_config_info[2];
      
        unsigned char pio_last_write[10];
	unsigned char pio_last_read[10];

	int pio_xfer_width;
	int reg_atapi_delay_flag;

	int int_use_intr_flag;


     private:
	unsigned int tmr_time_out;      // max command execution time in seconds
	unsigned int timeOut;   	      // command timeout time - see the
 
};

#endif



