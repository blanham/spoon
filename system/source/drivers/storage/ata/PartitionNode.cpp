#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PartitionNode.h"

int PartitionNode::read( int64 pos, void *buffer, int len )
{
   long lba = (pos / 512) + _start;
   int sectors = len / 512;
   int left    = len % 512;

   int ans;

    void *dest = buffer;

    if ( sectors > 0 )
    {
        smk_disable_interrupts();
	ans = reg_pio_data_in_lba( _device, CMD_READ_SECTORS,
     			    0, sectors,
			   lba,
			   dest,
			   sectors, 0 );
        smk_enable_interrupts();

	if ( ans == 1 ) return -1;
    }
	if ( left == 0 ) return len;
	
	dest = (void*)((uint32)buffer + sectors * 512);
	lba = lba + sectors;
	char temp[512];
	
        smk_disable_interrupts();
	ans = reg_pio_data_in_lba( _device, CMD_READ_SECTORS,
     			    0, 1,
			   lba,
			   temp,
			   1, 0 );
        smk_enable_interrupts();
	if ( ans == 1 ) return -1;

	memcpy( dest, temp, left );
	
   return len;
}

int PartitionNode::write( int64 pos, void *buffer, int len )
{
   long lba = (pos / 512) + _start;
   int sectors = len / 512;
   int    left = len % 512;
   
   int ans;
  
   if ( sectors > 0 )
   {
  	// Write all the complete sectors..
  	smk_disable_interrupts();
	ans = reg_pio_data_out_lba( _device, CMD_READ_SECTORS,
     			    0, sectors,
			   lba,
			   buffer,
			   sectors, 0 );
        smk_enable_interrupts();
	if ( ans == 1 ) return -1;
   }
	if ( left == 0 ) return len;

	// Read in the last sector
	lba += sectors;
	char temp[512];
	
	smk_disable_interrupts();
	ans = reg_pio_data_in_lba( _device, CMD_READ_SECTORS,
     			    0, 1,
			   lba,
			   temp,
			   1, 0 );
	smk_enable_interrupts();
	if ( ans == 1 ) return -1;

	// overwrite any new data in it... 
   	void *dest = (void*)((uint32)buffer + sectors * 512);
	memcpy( temp, dest, left );
 
 	// write it to the disk
  	smk_disable_interrupts();
	ans = reg_pio_data_out_lba( _device, CMD_READ_SECTORS,
     			    0, 1,
			   lba,
			   temp,
			   1, 0 );
	smk_enable_interrupts();
	if ( ans == 1 ) return -1;
   
   return len;
}

// ******************************************************

bool PartitionNode::Present()
{
   int numDev = 0;
   unsigned char sc;
   unsigned char sn;
   unsigned char cl;
   unsigned char ch;
   unsigned char st;
   unsigned char devCtrl;

   // setup register values

   devCtrl = CB_DC_HD15 | ( int_use_intr_flag ? 0 : CB_DC_NIEN );

   unsigned char devSel;

   if ( _device == 0 ) devSel = CB_DH_DEV0;
   		  else devSel = CB_DH_DEV1;

   // set up Device Control register
   pio_outbyte( CB_DC, devCtrl );

   // lets see if there is a device 0

   pio_outbyte( CB_DH, devSel );
   DELAY400NS;
   pio_outbyte( CB_SC, 0x55 );
   pio_outbyte( CB_SN, 0xaa );
   pio_outbyte( CB_SC, 0xaa );
   pio_outbyte( CB_SN, 0x55 );
   pio_outbyte( CB_SC, 0x55 );
   pio_outbyte( CB_SN, 0xaa );
   sc = pio_inbyte( CB_SC );
   sn = pio_inbyte( CB_SN );
   if ( ( sc == 0x55 ) && ( sn == 0xaa ) )
      reg_config_info[_device] = REG_CONFIG_TYPE_UNKN;


   pio_outbyte( CB_DH, devSel );
   DELAY400NS;
   reg_reset( 0, _device );


   // lets check device 0 again, is device 0 really there?
   // is it ATA or ATAPI?

   pio_outbyte( CB_DH, devSel );
   DELAY400NS;
   sc = pio_inbyte( CB_SC );
   sn = pio_inbyte( CB_SN );
   if ( ( sc == 0x01 ) && ( sn == 0x01 ) )
   {
      reg_config_info[_device] = REG_CONFIG_TYPE_UNKN;
      cl = pio_inbyte( CB_CL );
      ch = pio_inbyte( CB_CH );
      st = pio_inbyte( CB_STAT );
      if ( ( cl == 0x14 ) && ( ch == 0xeb ) )
         reg_config_info[_device] = REG_CONFIG_TYPE_ATAPI;
      else
         if ( ( cl == 0x00 ) && ( ch == 0x00 ) && ( st != 0x00 ) )
            reg_config_info[_device] = REG_CONFIG_TYPE_ATA;
   }

   // If possible, select a device that exists, 

      if ( reg_config_info[_device] == REG_CONFIG_TYPE_ATA )
   	{
	      pio_outbyte( CB_DH, devSel );
	      DELAY400NS;
	      numDev ++ ;
	}

   return (numDev==1);
}


// ******************************************************

void PartitionNode::pio_set_iobase_addr( unsigned int base1, unsigned int base2 )
{
   pio_base_addr1 = base1;
   pio_base_addr2 = base2;
   pio_memory_seg = 0;
   pio_reg_addrs[ CB_DATA ] = pio_base_addr1 + 0;  // 0
   pio_reg_addrs[ CB_FR   ] = pio_base_addr1 + 1;  // 1
   pio_reg_addrs[ CB_SC   ] = pio_base_addr1 + 2;  // 2
   pio_reg_addrs[ CB_SN   ] = pio_base_addr1 + 3;  // 3
   pio_reg_addrs[ CB_CL   ] = pio_base_addr1 + 4;  // 4
   pio_reg_addrs[ CB_CH   ] = pio_base_addr1 + 5;  // 5
   pio_reg_addrs[ CB_DH   ] = pio_base_addr1 + 6;  // 6
   pio_reg_addrs[ CB_CMD  ] = pio_base_addr1 + 7;  // 7
   pio_reg_addrs[ CB_DC   ] = pio_base_addr2 + 6;  // 8
   pio_reg_addrs[ CB_DA   ] = pio_base_addr2 + 7;  // 9
}



void PartitionNode::reg_wait_poll( int we, int pe )

{
   unsigned char status;


   // Wait for INT 7x -or- wait for not BUSY -or- wait for time out.

      while ( 1 )
      {
         status = pio_inbyte( CB_ASTAT );       // poll for not busy
         if ( ( status & CB_STAT_BSY ) == 0 )
            break;
         if ( tmr_chk_timeout() )               // time out yet ?
         {
            reg_cmd_info.to = 1;
            reg_cmd_info.ec = pe;
            break;
         }
      }

}





//*************************************************************
//
// These functions do basic IN/OUT of byte and word values.
//
//*************************************************************


unsigned char PartitionNode::pio_inbyte( unsigned int addr )
{
   unsigned int regAddr;
   unsigned char uc;
   unsigned char * ucp;

   regAddr = pio_reg_addrs[ addr ];
   if ( pio_memory_seg )
   {
      ucp = (unsigned char  *) MK_FP( pio_memory_seg, regAddr );
      uc = * ucp;
   }
   else
   {
      uc = (unsigned char) inb( regAddr );
   }
   pio_last_read[ addr ] = uc;
   return uc;
}

//*********************************************************

void PartitionNode::pio_outbyte( unsigned int addr, unsigned char data )
{
   unsigned int regAddr;
   unsigned char * ucp;

   regAddr = pio_reg_addrs[ addr ];
   if ( pio_memory_seg )
   {
      ucp = (unsigned char  *) MK_FP( pio_memory_seg, regAddr );
      * ucp = data;
   }
   else
   {
      outb( regAddr , data);
   }
   pio_last_write[ addr ] = data;
}

//*********************************************************

unsigned int PartitionNode::pio_inword( unsigned int addr )
{
   unsigned int regAddr;
   unsigned int ui;
   unsigned int * uip;

   regAddr = pio_reg_addrs[ addr ];
   if ( pio_memory_seg )
   {
      uip = (unsigned int *) MK_FP( pio_memory_seg, regAddr );
      ui = * uip;
   }
   else
   {
      ui = inw( regAddr );
   }
   return ui;
}

//*********************************************************

void PartitionNode::pio_outword( unsigned int addr, unsigned int data )
{
   unsigned int regAddr;
   unsigned int * uip;

   regAddr = pio_reg_addrs[ addr ];
   if ( pio_memory_seg )
   {
      uip = (unsigned int  *) MK_FP( pio_memory_seg, regAddr );
      * uip = data;
   }
   else
   {
      outw( regAddr , data);
   }
}

//*************************************************************
//
// These functions do REP INS/OUTS (PIO data transfers).
//
//*************************************************************

// Note: pio_rep_inbyte() can be called directly but usually it
// is called by pio_rep_inword() based on the value of the
// pio_xfer_width variables.

void PartitionNode::pio_rep_inbyte( unsigned int addrDataReg,
                     void* buffer,
                     long byteCnt )
{
   unsigned int dataRegAddr;
   unsigned int bCnt;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];

   while ( byteCnt > 0L )
   {

      if ( byteCnt > 16384L )
         bCnt = 16384;
      else
         bCnt = (unsigned int) byteCnt;

      __asm__ __volatile__
      (
        "  push %%eax\n"
	"  push %%ecx\n"
	"  push %%edx\n"
	"  push %%edi\n"

	"  mov %0, %%edi\n"
	"  mov %1, %%ecx\n"
	"  mov %2, %%edx\n"
	  
	"  cld\n"
	  
	"  rep insb\n"

        "  pop   %%edi\n"
        "  pop   %%edx\n"
        "  pop   %%ecx\n"
        "  pop   %%eax\n"
	  
         :
	 : "p" (buffer),
	   "g" (bCnt), "g" (dataRegAddr)
      );



      byteCnt = byteCnt - (long) bCnt;

      pio_inbyte( CB_ASTAT );    // just for debugging

   }
}

//*********************************************************

// Note: pio_rep_outbyte() can be called directly but usually it
// is called by pio_rep_outword() based on the value of the
// pio_xfer_width variables.

void PartitionNode::pio_rep_outbyte( unsigned int addrDataReg,
		      void* buffer,
                      long byteCnt )
{
   unsigned int dataRegAddr;
   unsigned int bCnt;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];

   while ( byteCnt > 0L )
   {

      if ( byteCnt > 16384L )
         bCnt = 16384;
      else
         bCnt = (unsigned int) byteCnt;

      __asm__ __volatile__
      (
        "  push %%eax\n"
	"  push %%ecx\n"
	"  push %%edx\n"
	"  push %%esi\n"

	"  mov %0, %%esi\n"
	"  mov %1, %%ecx\n"
	"  mov %2, %%edx\n"
	  
	"  cld\n"
	  
	"  rep outsb\n"

        "  pop   %%esi\n"
        "  pop   %%edx\n"
        "  pop   %%ecx\n"
        "  pop   %%eax\n"
	  
         :
	 : "p" (buffer),
	   "g" (bCnt), "g" (dataRegAddr)
      );


      byteCnt = byteCnt - (long) bCnt;

      pio_inbyte( CB_ASTAT );    // just for debugging

   }
}

//*********************************************************

// Note: pio_rep_indword() can be called directly but usually it
// is called by pio_rep_inword() based on the value of the
// pio_xfer_width variable.

void PartitionNode::pio_rep_indword( unsigned int addrDataReg,
                      void* buffer,
                      unsigned int dwordCnt )
{
   unsigned int dataRegAddr;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];


      __asm__ __volatile__
      (
        "  push %%eax\n"
	"  push %%ecx\n"
	"  push %%edx\n"
	"  push %%edi\n"

	"  mov %0, %%edi\n"
	"  mov %1, %%ecx\n"
	"  mov %2, %%edx\n"
	  
	"  cld\n"
	  
	"  rep insl\n"

        "  pop   %%edi\n"
        "  pop   %%edx\n"
        "  pop   %%ecx\n"
        "  pop   %%eax\n"
	  
         :
	 : "p" (buffer),
	   "g" (dwordCnt), "g" (dataRegAddr)
      );


}

//*********************************************************

// Note: pio_rep_outdword() can be called directly but usually it
// is called by pio_rep_outword() based on the value of the
// pio_xfer_width variable.

void PartitionNode::pio_rep_outdword( unsigned int addrDataReg,
                       void* buffer,
                       unsigned int dwordCnt )
{
   unsigned int dataRegAddr;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];

      __asm__ __volatile__
      (
        "  push %%eax\n"
	"  push %%ecx\n"
	"  push %%edx\n"
	"  push %%esi\n"

	"  mov %0, %%esi\n"
	"  mov %1, %%ecx\n"
	"  mov %2, %%edx\n"
	  
	"  cld\n"
	  
	"  rep outsl\n"

        "  pop   %%esi\n"
        "  pop   %%edx\n"
        "  pop   %%ecx\n"
        "  pop   %%eax\n"
	  
         :
	 : "p" (buffer),
	   "g" (dwordCnt), "g" (dataRegAddr)
      );

}

//*********************************************************

// Note: pio_rep_inword() is the primary way perform PIO
// Data In transfers. It will handle 8-bit, 16-bit and 32-bit
// I/O based data transfers and 8-bit and 16-bit PCMCIA Memory
// mode transfers.

void PartitionNode::pio_rep_inword( unsigned int addrDataReg,
                     void* buffer,
                     unsigned int wordCnt )

{
   unsigned int dataRegAddr;
   volatile unsigned int * uip1;
   unsigned int  * uip2;
   volatile unsigned char * ucp1;
   unsigned char * ucp2;
   long bCnt;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];

   if ( pio_memory_seg )
   {

      // PCMCIA Memory mode data transfer.

      if ( pio_xfer_width == 8 )
      {
         // PCMCIA Memory mode 8-bit
         bCnt = ( (long) wordCnt ) * 2L;
         ucp1 = (unsigned char *) MK_FP( pio_memory_seg, dataRegAddr );
         ucp2 = (unsigned char *) buffer;
         for ( ; bCnt > 0; bCnt -- )
         {
            * ucp2 = * ucp1;
            ucp2 ++ ;
         }
      }
      else
      {
         // PCMCIA Memory mode 16-bit
         uip1 = (unsigned int  *) MK_FP( pio_memory_seg, dataRegAddr );
         uip2 = (unsigned int  *) buffer;
         for ( ; wordCnt > 0; wordCnt -- )
         {
            * uip2 = * uip1;
            uip2 ++ ;
         }
      }
   }
   else
   {

      // Data transfer using INS instruction.

      if ( pio_xfer_width == 8 )
      {
         // do REP INS
         pio_rep_inbyte( addrDataReg, buffer, ( (long) wordCnt ) * 2L );
         return;
      }

      if ( ( pio_xfer_width == 32 ) && ( ! ( wordCnt & 0x0001 ) ) )
      {
         // do REP INSD
         pio_rep_indword( addrDataReg, buffer, wordCnt / 2 );
         return;
      }

      // do REP INSW

      __asm__ __volatile__
      (
        "  push %%eax\n"
	"  push %%ecx\n"
	"  push %%edx\n"
	"  push %%edi\n"

	"  mov %0, %%edi\n"
	"  mov %1, %%ecx\n"
	"  mov %2, %%edx\n"
	  
	"  cld\n"
	  
	"  rep insw\n"

        "  pop   %%edi\n"
        "  pop   %%edx\n"
        "  pop   %%ecx\n"
        "  pop   %%eax\n"
	  
         :
	 : "p" (buffer),
	   "g" (wordCnt), "g" (dataRegAddr)
      );

   }
}

//*********************************************************

// Note: pio_rep_outword() is the primary way perform PIO
// Data Out transfers. It will handle 8-bit, 16-bit and 32-bit
// I/O based data transfers and 8-bit and 16-bit PCMCIA Memory
// mode transfers.

void PartitionNode::pio_rep_outword( unsigned int addrDataReg,
                      void* buffer,
                      unsigned int wordCnt )

{
   unsigned int dataRegAddr;
   unsigned int * uip1;
   unsigned int * uip2;
   unsigned char * ucp1;
   unsigned char * ucp2;
   long bCnt;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];

   if ( pio_memory_seg )
   {

      // PCMCIA Memory mode data transfer.

      if ( pio_xfer_width == 8 )
      {
         // PCMCIA Memory mode 8-bit
         bCnt = ( (long) wordCnt ) * 2L;
         ucp1 = (unsigned char *) buffer;
         ucp2 = (unsigned char *) MK_FP( pio_memory_seg, dataRegAddr );
         for ( ; bCnt > 0; bCnt -- )
         {
            * ucp2 = * ucp1;
            ucp1 ++ ;
         }
      }
      else
      {
         // PCMCIA Memory mode 16-bit
         uip1 = (unsigned int *) buffer;
         uip2 = (unsigned int *) MK_FP( pio_memory_seg, dataRegAddr );
         for ( ; wordCnt > 0; wordCnt -- )
         {
            * uip2 = * uip1;
            uip1 ++ ;
         }
      }
   }
   else
   {

      // Data transfer using OUTS instruction.

      if ( pio_xfer_width == 8 )
      {
         // do REP OUTS
         pio_rep_outbyte( addrDataReg, buffer, ( (long) wordCnt ) * 2L );
         return;
      }

      if ( ( pio_xfer_width == 32 ) && ( ! ( wordCnt & 0x0001 ) ) )
      {
         // do REP OUTSD
         pio_rep_outdword( addrDataReg, buffer, wordCnt / 2 );
         return;
      }

      // do REP OUTSW

      __asm__ __volatile__
      (
	 " push %%eax\n"
	 " push %%ecx\n"
	 " push %%edx\n"
	 " push %%esi\n"

	 " mov %0, %%esi\n"
	 " mov %1, %%ecx\n"
	 " mov %2, %%edx\n"
	  
	 " cld\n"
	  
	 " rep outsw\n"

         " pop   %%esi\n"
         " pop   %%edx\n"
         " pop   %%ecx\n"
         " pop   %%eax\n"
	  
         :
	 : "p" (buffer),
	   "g" (wordCnt), "g" (dataRegAddr)
      );

   }
}


//*************************************************************
//
// reg_reset() - Execute a Software Reset.
//
// See ATA-2 Section 9.2, ATA-3 Section 9.2, ATA-4 Section 8.3.
//
//*************************************************************

int PartitionNode::reg_reset( int skipFlag, int devRtrn )

{
   unsigned char sc;
   unsigned char sn;
   unsigned char status;
   unsigned char devCtrl;

   // setup register values

   devCtrl = CB_DC_HD15 | ( int_use_intr_flag ? 0 : CB_DC_NIEN );

   // mark start of reset in low level trace


   // Reset error return data.

   sub_zero_return_data();
   reg_cmd_info.flg = TRC_FLAG_SRST;
   reg_cmd_info.ct  = TRC_TYPE_ASR;

   // initialize the command timeout counter

   tmr_set_timeout();


   // Set and then reset the soft reset bit in the Device Control
   // register.  This causes device 0 be selected.

   if ( ! skipFlag )
   {
      pio_outbyte( CB_DC, devCtrl | CB_DC_SRST );
      DELAY400NS;
      pio_outbyte( CB_DC, devCtrl );
      DELAY400NS;
   }


   // If there is a device 0, wait for device 0 to set BSY=0.

   if ( reg_config_info[0] != REG_CONFIG_TYPE_NONE )
   {
      sub_atapi_delay( 0 );
      while ( 1 )
      {
         status = pio_inbyte( CB_STAT );
         if ( ( status & CB_STAT_BSY ) == 0 )
            break;
         if ( tmr_chk_timeout() )
         {
            reg_cmd_info.to = 1;
            reg_cmd_info.ec = 1;
            break;
         }
      }
   }

   // If there is a device 1, wait until device 1 allows
   // register access.

   if ( reg_config_info[1] != REG_CONFIG_TYPE_NONE )
   {
      sub_atapi_delay( 1 );
      while ( 1 )
      {
         pio_outbyte( CB_DH, CB_DH_DEV1 );
         DELAY400NS;
         sc = pio_inbyte( CB_SC );
         sn = pio_inbyte( CB_SN );
         if ( ( sc == 0x01 ) && ( sn == 0x01 ) )
            break;
         if ( tmr_chk_timeout() )
         {
            reg_cmd_info.to = 1;
            reg_cmd_info.ec = 2;
            break;
         }
      }

      // Now check if drive 1 set BSY=0.

      if ( reg_cmd_info.ec == 0 )
      {
         if ( pio_inbyte( CB_STAT ) & CB_STAT_BSY )
         {
            reg_cmd_info.ec = 3;
         }
      }
   }

   // RESET_DONE:

   // We are done but now we must select the device the caller
   // requested before we trace the command.  This will cause
   // the correct data to be returned in reg_cmd_info.

   pio_outbyte( CB_DH, devRtrn ? CB_DH_DEV1 : CB_DH_DEV0 );
   DELAY400NS;
   sub_trace_command();

   // If possible, select a device that exists,
   // try device 0 first.

   if ( reg_config_info[1] != REG_CONFIG_TYPE_NONE )
   {
      pio_outbyte( CB_DH, CB_DH_DEV1 );
      DELAY400NS;
   }
   if ( reg_config_info[0] != REG_CONFIG_TYPE_NONE )
   {
      pio_outbyte( CB_DH, CB_DH_DEV0 );
      DELAY400NS;
   }

   // mark end of reset in low level trace


   // All done.  The return values of this function are described in
   // ATAIO.H.

   if ( reg_cmd_info.ec )
      return 1;
   return 0;
}

//*************************************************************
//
// sub_zero_return_data() -- zero the return data areas.
//
//*************************************************************

void PartitionNode::sub_zero_return_data( void )

{

   reg_cmd_info.flg = TRC_FLAG_EMPTY;
   reg_cmd_info.ct  = TRC_TYPE_NONE;
   reg_cmd_info.cmd = 0;
   reg_cmd_info.fr1 = 0;
   reg_cmd_info.sc1 = 0;
   reg_cmd_info.sn1 = 0;
   reg_cmd_info.cl1 = 0;
   reg_cmd_info.ch1 = 0;
   reg_cmd_info.dh1 = 0;
   reg_cmd_info.dc1 = 0;
   reg_cmd_info.ec  = 0;
   reg_cmd_info.to  = 0;
   reg_cmd_info.st2 = 0;
   reg_cmd_info.as2 = 0;
   reg_cmd_info.er2 = 0;
   reg_cmd_info.sc2 = 0;
   reg_cmd_info.sn2 = 0;
   reg_cmd_info.cl2 = 0;
   reg_cmd_info.ch2 = 0;
   reg_cmd_info.dh2 = 0;
   reg_cmd_info.totalBytesXfer = 0L;
   reg_cmd_info.failbits = 0;
   reg_cmd_info.drqPackets = 0L;
}

//*************************************************************
//
// sub_atapi_delay() - delay for at least two ticks of the bios
//                     timer (or at least 110ms).
//
//*************************************************************

void PartitionNode::sub_atapi_delay( int dev )
{
    smk_ndelay(110);
    return;
}

void PartitionNode::sub_xfer_delay( void )

{
    smk_ndelay(60);
   
//   lw = tmr_read_bios_timer();
//   while ( lw == tmr_read_bios_timer() )
      /* do nothing */ ;
}



//*************************************************************
//
// sub_trace_command() -- trace the end of a command.
//
//*************************************************************

void PartitionNode::sub_trace_command( void )
{
   reg_cmd_info.st2 = pio_inbyte( CB_STAT );
   reg_cmd_info.as2 = pio_inbyte( CB_ASTAT );
   reg_cmd_info.er2 = pio_inbyte( CB_ERR );
   reg_cmd_info.sc2 = pio_inbyte( CB_SC );
   reg_cmd_info.sn2 = pio_inbyte( CB_SN );
   reg_cmd_info.cl2 = pio_inbyte( CB_CL );
   reg_cmd_info.ch2 = pio_inbyte( CB_CH );
   reg_cmd_info.dh2 = pio_inbyte( CB_DH );
}

int PartitionNode::sub_select( int dev )
{
   unsigned char status;

   // PAY ATTENTION HERE
   // The caller may want to issue a command to a device that doesn't
   // exist (for example, Exec Dev Diag), so if we see this,
   // just select that device, skip all status checking and return.
   // We assume the caller knows what they are doing!

   if ( reg_config_info[ dev ] < REG_CONFIG_TYPE_ATA )
   {
      // select the device and return

      pio_outbyte( CB_DH, dev ? CB_DH_DEV1 : CB_DH_DEV0 );
      DELAY400NS;
      return 0;
   }

   // The rest of this is the normal ATA stuff for device selection
   // and we don't expect the caller to be selecting a device that
   // does not exist.
   // We don't know which drive is currently selected but we should
   // wait for it to be not BUSY.  Normally it will be not BUSY
   // unless something is very wrong!

   while ( 1 )
   {
      status = pio_inbyte( CB_STAT );
      if ( ( status & CB_STAT_BSY ) == 0 )
         break;
      if ( tmr_chk_timeout() )
      {
         reg_cmd_info.to = 1;
         reg_cmd_info.ec = 11;
         reg_cmd_info.st2 = status;
         reg_cmd_info.as2 = pio_inbyte( CB_ASTAT );
         reg_cmd_info.er2 = pio_inbyte( CB_ERR );
         reg_cmd_info.sc2 = pio_inbyte( CB_SC );
         reg_cmd_info.sn2 = pio_inbyte( CB_SN );
         reg_cmd_info.cl2 = pio_inbyte( CB_CL );
         reg_cmd_info.ch2 = pio_inbyte( CB_CH );
         reg_cmd_info.dh2 = pio_inbyte( CB_DH );
         return 1;
      }
   }

   // Here we select the drive we really want to work with by
   // putting 0xA0 or 0xB0 in the Drive/Head register (1f6).

   pio_outbyte( CB_DH, dev ? CB_DH_DEV1 : CB_DH_DEV0 );
   DELAY400NS;

   // If the selected device is an ATA device,
   // wait for it to have READY and SEEK COMPLETE
   // status.  Normally the drive should be in this state unless
   // something is very wrong (or initial power up is still in
   // progress).  For any other type of device, just wait for
   // BSY=0 and assume the caller knows what they are doing.

   while ( 1 )
   {
      status = pio_inbyte( CB_STAT );
      if ( reg_config_info[ dev ] == REG_CONFIG_TYPE_ATA )
      {
           if ( ( status & ( CB_STAT_BSY | CB_STAT_RDY | CB_STAT_SKC ) )
                     == ( CB_STAT_RDY | CB_STAT_SKC ) )
         break;
      }
      else
      {
         if ( ( status & CB_STAT_BSY ) == 0 )
            break;
      }
      if ( tmr_chk_timeout() )
      {
         reg_cmd_info.to = 1;
         reg_cmd_info.ec = 12;
         reg_cmd_info.st2 = status;
         reg_cmd_info.as2 = pio_inbyte( CB_ASTAT );
         reg_cmd_info.er2 = pio_inbyte( CB_ERR );
         reg_cmd_info.sc2 = pio_inbyte( CB_SC );
         reg_cmd_info.sn2 = pio_inbyte( CB_SN );
         reg_cmd_info.cl2 = pio_inbyte( CB_CL );
         reg_cmd_info.ch2 = pio_inbyte( CB_CH );
         reg_cmd_info.dh2 = pio_inbyte( CB_DH );
         return 1;
      }
   }

   // All done.  The return values of this function are described in
   // ATAIO.H.

   if ( reg_cmd_info.ec )
      return 1;
   return 0;
}



//*************************************************************
//
// tmr_set_timeout() - function used to set command timeout time
//
// The command time out time is computed as follows:
//
//    timer + ( tmr_time_out * 18 )
//
//**************************************************************

void PartitionNode::tmr_set_timeout( void )

{

   // get value of BIOS timer

   timeOut = tmr_read_bios_timer();

   // add command timeout value

   timeOut = timeOut + ( tmr_time_out * 18L );

   // ignore the lower 4 bits

   timeOut = timeOut & 0xfffffff0;
}

//*************************************************************
//
// tmr_chk_timeout() - function used to check for command timeout.
//
// Gives non-zero return if command has timed out.
//
//**************************************************************

int PartitionNode::tmr_chk_timeout( void )
{
   unsigned int curTime;

   // get current time

   curTime = tmr_read_bios_timer();

   // ignore lower 4 bits

   curTime = curTime & 0xfffffff0;

   // timed out yet ?

   if ( curTime >= timeOut ) return 1;

   return 0;
}

//*************************************************************
//
// tmr_read_bios_timer() - function to read the BIOS timer
//
//**************************************************************

unsigned int PartitionNode::tmr_read_bios_timer( void )
{
   unsigned int seconds;
   unsigned int milli;

        smk_system_time( &seconds, &milli );

   return seconds;
}



// **********************************************

int PartitionNode::reg_pio_data_in_lba( int dev, int cmd,
                         int fr, int sc,
                         long lba,
			 void* buffer,
                         int numSect, int multiCnt )

{
   unsigned int cyl;
   int head, sect;

   sect = (int) ( lba & 0x000000ffL );
   lba = lba >> 8;
   cyl = (int) ( lba & 0x0000ffff );
   lba = lba >> 16;
   head = ( (int) ( lba & 0x0fL ) ) | 0x40;
   return reg_pio_data_in( dev, cmd,
                           fr, sc,
                           cyl, head, sect,
                           buffer,
                           numSect, multiCnt );
}


// -----------------------------------------

int PartitionNode::reg_pio_data_in( int dev, int cmd,
                     int fr, int sc,
                     unsigned int cyl, int head, int sect,
	             void* buffer,
                     int numSect, int multiCnt )

{
   unsigned char devHead;
   unsigned char devCtrl;
   unsigned char cylLow;
   unsigned char cylHigh;
   unsigned char status;
   unsigned int wordCnt;

   // mark start of PDI cmd in low level trace


   // setup register values and adjust parameters

   devCtrl = CB_DC_HD15 | ( int_use_intr_flag ? 0 : CB_DC_NIEN );
   devHead = dev ? CB_DH_DEV1 : CB_DH_DEV0;
   devHead = devHead | ( head & 0x4f );
   cylLow = cyl & 0x00ff;
   cylHigh = ( cyl & 0xff00 ) >> 8;
   // these commands transfer only 1 sector
   if (    ( cmd == CMD_IDENTIFY_DEVICE )
        || ( cmd == CMD_IDENTIFY_DEVICE_PACKET )
        || ( cmd == CMD_READ_BUFFER )
      )
      numSect = 1;
   // only Read Multiple uses multiCnt
   if ( cmd != CMD_READ_MULTIPLE )
      multiCnt = 1;

   // Reset error return data.

   sub_zero_return_data();
   reg_cmd_info.flg = TRC_FLAG_CMD;
   reg_cmd_info.ct  = TRC_TYPE_APDI;
   reg_cmd_info.cmd = cmd;
   reg_cmd_info.fr1 = fr;
   reg_cmd_info.sc1 = sc;
   reg_cmd_info.sn1 = sect;
   reg_cmd_info.cl1 = cylLow;
   reg_cmd_info.ch1 = cylHigh;
   reg_cmd_info.dh1 = devHead;
   reg_cmd_info.dc1 = devCtrl;

   // Set command time out.

   tmr_set_timeout();

   // Select the drive - call the sub_select function.
   // Quit now if this fails.

   if ( sub_select( dev ) )
   {
      sub_trace_command();
      return 1;
   }

   // Set up all the registers except the command register.

   pio_outbyte( CB_DC, devCtrl );
   pio_outbyte( CB_FR, fr );
   pio_outbyte( CB_SC, sc );
   pio_outbyte( CB_SN, sect );
   pio_outbyte( CB_CL, cylLow );
   pio_outbyte( CB_CH, cylHigh );
   pio_outbyte( CB_DH, devHead );

   // For interrupt mode,
   // Take over INT 7x and initialize interrupt controller
   // and reset interrupt flag.

   //int_save_int_vect();

   // Start the command by setting the Command register.  The drive
   // should immediately set BUSY status.

   pio_outbyte( CB_CMD, cmd );

   // Waste some time by reading the alternate status a few times.
   // This gives the drive time to set BUSY in the status register on
   // really fast systems.  If we don't do this, a slow drive on a fast
   // system may not set BUSY fast enough and we would think it had
   // completed the command when it really had not even started the
   // command yet.

   DELAY400NS;

   // Loop to read each sector.

   while ( 1 )
   {
      // READ_LOOP:
      //
      // NOTE NOTE NOTE ...  The primary status register (1f7) MUST NOT be
      // read more than ONCE for each sector transferred!  When the
      // primary status register is read, the drive resets IRQ 14.  The
      // alternate status register (3f6) can be read any number of times.
      // After INT 7x read the the primary status register ONCE
      // and transfer the 256 words (REP INSW).  AS SOON as BOTH the
      // primary status register has been read AND the last of the 256
      // words has been read, the drive is allowed to generate the next
      // IRQ 14 (newer and faster drives could generate the next IRQ 14 in
      // 50 microseconds or less).  If the primary status register is read
      // more than once, there is the possibility of a race between the
      // drive and the software and the next IRQ 14 could be reset before
      // the system interrupt controller sees it.

      // Wait for INT 7x -or- wait for not BUSY -or- wait for time out.

      sub_atapi_delay( dev );
      reg_wait_poll( 34, 35 );

      // Read the primary status register.  In keeping with the rules
      // stated above the primary status register is read only
      // ONCE.

      status = pio_inbyte( CB_STAT );

      // If there was a time out error, go to READ_DONE.

      if ( reg_cmd_info.ec )
      {
//         printf("%s\n","ata: time out error");
         break;   // go to READ_DONE
      }

      // If BSY=0 and DRQ=1, transfer the data,
      // even if we find out there is an error later.

      int reg_slow_xfer_flag = 0;

      if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) == CB_STAT_DRQ )
      {
         // do the slow data transfer thing
//           printf("%s\n","ata: BSY = 0, DRQ = 1");

         if ( reg_slow_xfer_flag )
         {
            if ( numSect <= reg_slow_xfer_flag )
            {
               sub_xfer_delay();
               reg_slow_xfer_flag = 0;
            }
         }

         // increment number of DRQ packets

         reg_cmd_info.drqPackets ++ ;

         // determine the number of sectors to transfer

         wordCnt = multiCnt ? multiCnt : 1;
         if ( (int)wordCnt > numSect )
            wordCnt = numSect;
         wordCnt = wordCnt * 256;

         // Do the REP INSW to read the data for one block.

         reg_cmd_info.totalBytesXfer += ( wordCnt << 1 );
         pio_rep_inword( CB_DATA, buffer, wordCnt );

         DELAY400NS;    // delay so device can get the status updated

         // Note: The drive should have dropped DATA REQUEST by now.  If there
         // are more sectors to transfer, BUSY should be active now (unless
         // there is an error).

         // Decrement the count of sectors to be transferred
         // and increment buffer address.

         numSect = numSect - ( multiCnt ? multiCnt : 1 );
         buffer = (void*)
	          ((unsigned long)buffer + 
		      ( 512 * ( multiCnt ? multiCnt : 1 ) ) );
      }

      // So was there any error condition?

      if ( status & ( CB_STAT_BSY | CB_STAT_DF  | CB_STAT_ERR ) )
      {
//         printf("%s%x\n","ata: there were error conditions -> ", status);
         reg_cmd_info.ec = 31;
         break;   // go to READ_DONE
      }

      // DRQ should have been set -- was it?

      if ( ( status & CB_STAT_DRQ ) == 0 )
      {
//         printf("%s\n","ata: drq was not set. ");
         reg_cmd_info.ec = 32;
         break;   // go to READ_DONE
      }

      // If all of the requested sectors have been transferred, make a
      // few more checks before we exit.

      if ( numSect < 1 )
      {
         // Since the drive has transferred all of the requested sectors
         // without error, the drive should not have BUSY, DEVICE FAULT,
         // DATA REQUEST or ERROR active now.

         sub_atapi_delay( dev );
         status = pio_inbyte( CB_STAT );
         if ( status & ( CB_STAT_BSY | CB_STAT_DF | CB_STAT_DRQ | CB_STAT_ERR ) )
         {
//            printf("%s%x\n","ata: secondary error -> ", status );
            reg_cmd_info.ec = 33;
            break;   // go to READ_DONE
         }

         // All sectors have been read without error, go to READ_DONE.

         break;   // go to READ_DONE

      }

      // This is the end of the read loop.  If we get here, the loop is
      // repeated to read the next sector.  Go back to READ_LOOP.

   }

   // read the output registers and trace the command.

   sub_trace_command();

   // READ_DONE:

   // For interrupt mode, restore the INT 7x vector.

   //int_restore_int_vect();

   // mark end of PDI cmd in low level trace


   // All done.  The return values of this function are described in
   // ATAIO.H.

   if ( reg_cmd_info.ec )
      return 1;
   return 0;
}


//*************************************************************
//
// reg_pio_data_out_lba() - Easy way to execute a PIO Data In command
//                          using an LBA sector address.
//
//*************************************************************

int PartitionNode::reg_pio_data_out_lba( int dev, int cmd,
                          int fr, int sc,
                          long lba,
			  void* buffer,
                          int numSect, int multiCnt )

{
   unsigned int cyl;
   int head, sect;

   sect = (int) ( lba & 0x000000ffL );
   lba = lba >> 8;
   cyl = (int) ( lba & 0x0000ffff );
   lba = lba >> 16;
   head = ( (int) ( lba & 0x0fL ) ) | 0x40;
   return reg_pio_data_out( dev, cmd,
                            fr, sc,
                            cyl, head, sect,
                            buffer,
                            numSect, multiCnt );
}

//*************************************************************
//
// reg_pio_data_out() - Execute a PIO Data Out command.
//
// See ATA-2 Section 9.4, ATA-3 Section 9.4,
// ATA-4 Section 8.7 Figure 11.
//
//*************************************************************

int PartitionNode::reg_pio_data_out( int dev, int cmd,
                      int fr, int sc,
                      unsigned int cyl, int head, int sect,
                      void* buffer,
                      int numSect, int multiCnt )

{
   unsigned char devHead;
   unsigned char devCtrl;
   unsigned char cylLow;
   unsigned char cylHigh;
   unsigned char status;
   int loopFlag = 1;
   unsigned int wordCnt;

   // mark start of PDO cmd in low level trace



   // setup register values and adjust parameters

   devCtrl = CB_DC_HD15 | ( int_use_intr_flag ? 0 : CB_DC_NIEN );
   devHead = dev ? CB_DH_DEV1 : CB_DH_DEV0;
   devHead = devHead | ( head & 0x4f );
   cylLow = cyl & 0x00ff;
   cylHigh = ( cyl & 0xff00 ) >> 8;
   // these commands transfer only 1 sector
   if ( cmd == CMD_WRITE_BUFFER )
      numSect = 1;
   // only Write Multiple and CFA Write Multiple W/O Erase uses multCnt
   if (    ( cmd != CMD_WRITE_MULTIPLE )
        && ( cmd != CMD_CFA_WRITE_MULTIPLE_WO_ERASE )
      )
      multiCnt = 1;

   // Reset error return data.

   sub_zero_return_data();
   reg_cmd_info.flg = TRC_FLAG_CMD;
   reg_cmd_info.ct  = TRC_TYPE_APDO;
   reg_cmd_info.cmd = cmd;
   reg_cmd_info.fr1 = fr;
   reg_cmd_info.sc1 = sc;
   reg_cmd_info.sn1 = sect;
   reg_cmd_info.cl1 = cylLow;
   reg_cmd_info.ch1 = cylHigh;
   reg_cmd_info.dh1 = devHead;
   reg_cmd_info.dc1 = devCtrl;

   // Set command time out.

   tmr_set_timeout();

   // Select the drive - call the sub_select function.
   // Quit now if this fails.

   if ( sub_select( dev ) )
   {
      sub_trace_command();
      return 1;
   }

   // Set up all the registers except the command register.

   pio_outbyte( CB_DC, devCtrl );
   pio_outbyte( CB_FR, fr );
   pio_outbyte( CB_SC, sc );
   pio_outbyte( CB_SN, sect );
   pio_outbyte( CB_CL, cylLow );
   pio_outbyte( CB_CH, cylHigh );
   pio_outbyte( CB_DH, devHead );

   // For interrupt mode,
   // Take over INT 7x and initialize interrupt controller
   // and reset interrupt flag.

//   int_save_int_vect();

   // Start the command by setting the Command register.  The drive
   // should immediately set BUSY status.

   pio_outbyte( CB_CMD, cmd );

   // Waste some time by reading the alternate status a few times.
   // This gives the drive time to set BUSY in the status register on
   // really fast systems.  If we don't do this, a slow drive on a fast
   // system may not set BUSY fast enough and we would think it had
   // completed the command when it really had not even started the
   // command yet.

   DELAY400NS;

   // Wait for not BUSY or time out.
   // Note: No interrupt is generated for the
   // first sector of a write command.  Well...
   // that's not really true we are working with
   // a PCMCIA PC Card ATA device.

   sub_atapi_delay( dev );
   while ( 1 )
   {
      status = pio_inbyte( CB_ASTAT );
      if ( ( status & CB_STAT_BSY ) == 0 )
         break;
      if ( tmr_chk_timeout() )
      {
         reg_cmd_info.to = 1;
         reg_cmd_info.ec = 47;
         loopFlag = 0;
         break;
      }
   }

   // If we are using interrupts and we just got an interrupt, this is
   // wrong.  The drive must not generate an interrupt at this time.

   if ( loopFlag && int_use_intr_flag /*&& int_intr_flag*/ )
   {
      reg_cmd_info.ec = 46;
      loopFlag = 0;
   }

   // This loop writes each sector.

   while ( loopFlag )
   {
      // WRITE_LOOP:
      //
      // NOTE NOTE NOTE ...  The primary status register (1f7) MUST NOT be
      // read more than ONCE for each sector transferred!  When the
      // primary status register is read, the drive resets IRQ 14.  The
      // alternate status register (3f6) can be read any number of times.
      // For correct results, transfer the 256 words (REP OUTSW), wait for
      // INT 7x and then read the primary status register.  AS
      // SOON as BOTH the primary status register has been read AND the
      // last of the 256 words has been written, the drive is allowed to
      // generate the next IRQ 14 (newer and faster drives could generate
      // the next IRQ 14 in 50 microseconds or less).  If the primary
      // status register is read more than once, there is the possibility
      // of a race between the drive and the software and the next IRQ 14
      // could be reset before the system interrupt controller sees it.

      // If BSY=0 and DRQ=1, transfer the data,
      // even if we find out there is an error later.

      int reg_slow_xfer_flag = 0;

      if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) == CB_STAT_DRQ )
      {
         // do the slow data transfer thing

         if ( reg_slow_xfer_flag )
         {
            if ( numSect <= reg_slow_xfer_flag )
            {
               sub_xfer_delay();
               reg_slow_xfer_flag = 0;
            }
         }

         // increment number of DRQ packets

         reg_cmd_info.drqPackets ++ ;

         // determine the number of sectors to transfer

         wordCnt = multiCnt ? multiCnt : 1;
         if ( (int) wordCnt > numSect )
            wordCnt = numSect;
         wordCnt = wordCnt * 256;

         // Do the REP OUTSW to write the data for one block.

         reg_cmd_info.totalBytesXfer += ( wordCnt << 1 );
         pio_rep_outword( CB_DATA, buffer, wordCnt );

         DELAY400NS;    // delay so device can get the status updated

         // Note: The drive should have dropped DATA REQUEST and
         // raised BUSY by now.

         // Decrement the count of sectors to be transferred
         // and increment buffer address.

         numSect = numSect - ( multiCnt ? multiCnt : 1 );
         buffer = (void*)
	          ((unsigned long)buffer + 
		      ( 512 * ( multiCnt ? multiCnt : 1 ) ));
      }

      // So was there any error condition?

      if ( status & ( CB_STAT_BSY | CB_STAT_DF | CB_STAT_ERR ) )
      {
         reg_cmd_info.ec = 41;
         break;   // go to WRITE_DONE
      }

      // DRQ should have been set -- was it?

      if ( ( status & CB_STAT_DRQ ) == 0 )
      {
         reg_cmd_info.ec = 42;
         break;   // go to WRITE_DONE
      }

      // Wait for INT 7x -or- wait for not BUSY -or- wait for time out.

      sub_atapi_delay( dev );
      reg_wait_poll( 44, 45 );

      // Read the primary status register.  In keeping with the rules
      // stated above the primary status register is read only ONCE.

      status = pio_inbyte( CB_STAT );

      // If there was a time out error, go to WRITE_DONE.

      if ( reg_cmd_info.ec )
         break;   // go to WRITE_DONE

      // If all of the requested sectors have been transferred, make a
      // few more checks before we exit.

      if ( numSect < 1 )
      {
         // Since the drive has transferred all of the sectors without
         // error, the drive MUST not have BUSY, DEVICE FAULT, DATA REQUEST
         // or ERROR status at this time.

         if ( status & ( CB_STAT_BSY | CB_STAT_DF | CB_STAT_DRQ | CB_STAT_ERR ) )
         {
            reg_cmd_info.ec = 43;
            break;   // go to WRITE_DONE
         }

         // All sectors have been written without error, go to WRITE_DONE.

         break;   // go to WRITE_DONE

      }

      //
      // This is the end of the write loop.  If we get here, the loop
      // is repeated to write the next sector.  Go back to WRITE_LOOP.

   }

   // read the output registers and trace the command.

   sub_trace_command();

   // WRITE_DONE:

   // For interrupt mode, restore the INT 7x vector.

//   int_restore_int_vect();

   // mark end of PDO cmd in low level trace


   // All done.  The return values of this function are described in
   // ATAIO.H.

   if ( reg_cmd_info.ec )
      return 1;
   return 0;
}


