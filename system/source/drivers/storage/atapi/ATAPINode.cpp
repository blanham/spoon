#include <kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ATAPINode.h"


//#define PRINTIT(code)	printf("%s(%i): %i\n",__FUNCTION__,__LINE__, code);
#define PRINTIT(code) /*code*/

int ATAPINode::loaded()
{
	return -1;
}

int ATAPINode::eject()
{
   unsigned char cdb[16];
   int rc;

   PRINTIT(0);

   int failures = 0;
   char buffer[2048];

   for ( failures = 0; failures < 10; failures++ )
   {
	   // do an ATAPI Unload command
	   memset( cdb, 0, sizeof( cdb ) );
	   cdb[0] = 0x1B;   // command code
	   cdb[4] = 2; // eject the CD, no power changes.
	
	   rc = packet( 10, cdb, 0, 4096, buffer, 0 );
	   if ( rc == 0 ) return 0;

	   PRINTIT(0);
	   smk_sleep( 1 );
	   Reset();
    }
 
  return -1;
}

int ATAPINode::read( int64 pos, void *buffer, int len )
{
   unsigned char cdb[16];
   int rc;
   int bytes = 0;
   
   long lba    = pos / 2048;
   int sectors = len / 2048;
   int left    = len % 2048;

    void *dest = buffer;

   char temp[2048];

   while ( sectors-- > 0 )
   {
       int failures = 0;
       for ( failures = 0; failures < 10; failures++ )
       {
		   // do an ATAPI read command
		   memset( cdb, 0, sizeof( cdb ) );
		   cdb[0] = 0x28;   // command code
		   cdb[2] = (lba >> 24) & 0xFF;
		   cdb[3] = (lba >> 16) & 0xFF;
		   cdb[4] = (lba >>  8) & 0xFF;
		   cdb[5] = lba & 0xFF;    // LBA
		   cdb[8] = 1;    // sectors
	
  		   PRINTIT(0);
		   rc = packet( 10, cdb, 0, 2048, dest, lba );
  		   PRINTIT(0);
		   
		   if ( rc == 0 )  break;
		   
		   PRINTIT(0);
		   smk_sleep( 1 );
	   	   Reset();
       }    

       if ( failures >= 10 ) return -1;


       dest = (void*)((uint32)dest + 2048 );
       lba += 1;
       bytes += reg_cmd_info.totalBytesXfer;
   }
   
  // ........ left overs ....
   if ( left > 0 )
   {
       int failures = 0;
       for ( failures = 0; failures < 10; failures++ )
       {
		   // do an ATAPI Unload command
		   memset( cdb, 0, sizeof( cdb ) );
		   cdb[0] = 0x28;   // command code
		   cdb[2] = (lba >> 24) & 0xFF;
		   cdb[3] = (lba >> 16) & 0xFF;
		   cdb[4] = (lba >>  8) & 0xFF;
		   cdb[5] = lba & 0xFF;    // LBA
		   cdb[8] = 1;    // sectors
	
		   rc = packet( 10, cdb, 0, 2048, temp, lba );
		   if ( rc == 0 )  break;

		   PRINTIT(0);
		   smk_sleep( 1 );
	   	   Reset();
	   }    

        if ( failures >= 10 ) return -1;


		memcpy( dest, temp, left );
        bytes += left;
  }

  return bytes;
}

int ATAPINode::write( int64 pos, void *buffer, int len )
{
   return -1;
}


int ATAPINode::packet( int cmdSize, void *cmd, int dir,
       			   long bufSize, void* buffer,
			   unsigned long lba )
{
  int ans = 0;
  smk_disable_interrupts();
   for ( int failures = 0; failures < 10; failures++ )
   {
	   ans = reg_packet( cmdSize, cmd, dir, bufSize, buffer, lba );
	   if ( ans == 0 ) break;
	   Reset( false );
   }
  smk_enable_interrupts();
  return ans;
}

int ATAPINode::reg_packet( int cmdSize, void *cmd, int dir,
       			   long bufSize, void* buffer,
			   unsigned long lba )
{
	PRINTIT(0);
		
   long dpbc = bufSize;
   unsigned int cpbc = bufSize;
   void* cpbuf = cmd;
   void* dpbuf = buffer;

   unsigned char devCtrl;
   unsigned char devHead;
   unsigned char cylLow;
   unsigned char cylHigh;
   unsigned char frReg;
   unsigned char scReg;
   unsigned char snReg;
   unsigned char status;
   unsigned char reason;
   unsigned char lowCyl;
   unsigned char highCyl;
   unsigned int byteCnt;
   unsigned int wordCnt;
   int ndx;
   unsigned long dpaddr;
   unsigned char * cp;
   unsigned char * cfp;
   int slowXferCntr = 0;
   int prevFailBit7 = 0;
   int dev = _device;

	// ----------------------
	unsigned char reg_atapi_reg_fr;  // see reg_packet()
	unsigned char reg_atapi_reg_sc;  // see reg_packet()
	unsigned char reg_atapi_reg_sn;  // see reg_packet()
	unsigned char reg_atapi_reg_dh;  // see reg_packet()
	long reg_atapi_max_bytes = 32768L;
	
	// ATAPI command packet size and data
	
	int reg_atapi_cp_size;
	unsigned char reg_atapi_cp_data[16];
	// ....................................


   // mark start of PI cmd in low level trace


   // setup register values

   devCtrl = CB_DC_HD15 | ( int_use_intr_flag ? 0 : CB_DC_NIEN );
   reg_atapi_reg_dh = reg_atapi_reg_dh & 0x0f;
   devHead = ( dev ? CB_DH_DEV1 : CB_DH_DEV0 ) | reg_atapi_reg_dh;
   cylLow = dpbc & 0x00ff;
   cylHigh = ( dpbc & 0xff00 ) >> 8;
   frReg = reg_atapi_reg_fr;
   scReg = reg_atapi_reg_sc;
   snReg = reg_atapi_reg_sn;
   reg_atapi_reg_fr = 0;
   reg_atapi_reg_sc = 0;
   reg_atapi_reg_sn = 0;
   reg_atapi_reg_dh = 0;

   // Reset error return data.

   sub_zero_return_data();
   reg_cmd_info.flg = TRC_FLAG_CMD;
   reg_cmd_info.ct  = dir ? TRC_TYPE_PPDO : TRC_TYPE_PPDI;
   reg_cmd_info.cmd = CMD_PACKET;
   reg_cmd_info.fr1 = frReg;
   reg_cmd_info.sc1 = scReg;
   reg_cmd_info.sn1 = snReg;
   reg_cmd_info.cl1 = cylLow;
   reg_cmd_info.ch1 = cylHigh;
   reg_cmd_info.dh1 = devHead;
   reg_cmd_info.dc1 = devCtrl;

   // Make sure the command packet size is either 12 or 16
   // and save the command packet size and data.

   cpbc = cpbc < 12 ? 12 : cpbc;
   cpbc = cpbc > 12 ? 16 : cpbc;
   reg_atapi_cp_size = cpbc;
   cp = reg_atapi_cp_data;
   cfp = (unsigned char*)cpbuf;
   for ( ndx = 0; ndx < (int)cpbc; ndx ++ )
   {
      * cp = * cfp;
      cp ++ ;
      cfp ++ ;
   }

   // Set command time out.

	PRINTIT(0);
   tmr_set_timeout();
	PRINTIT(0);

   // Select the drive - call the sub_select function.
   // Quit now if this fails.

   if ( sub_select( dev ) )
   {
		PRINTIT(0);
      sub_trace_command();
      reg_atapi_max_bytes = 32768L;    // reset max bytes
	  PRINTIT(0);
      return 1;
   }

   // Set up all the registers except the command register.

	PRINTIT(0);
   pio_outbyte( CB_DC, devCtrl );
   pio_outbyte( CB_FR, frReg );
   pio_outbyte( CB_SC, scReg );
   pio_outbyte( CB_SN, snReg );
   pio_outbyte( CB_CL, cylLow );
   pio_outbyte( CB_CH, cylHigh );
   pio_outbyte( CB_DH, devHead );

   // For interrupt mode,
   // Take over INT 7x and initialize interrupt controller
   // and reset interrupt flag.

//   int_save_int_vect();

   // Start the command by setting the Command register.  The drive
   // should immediately set BUSY status.

   pio_outbyte( CB_CMD, CMD_PACKET );

   // Waste some time by reading the alternate status a few times.
   // This gives the drive time to set BUSY in the status register on
   // really fast systems.  If we don't do this, a slow drive on a fast
   // system may not set BUSY fast enough and we would think it had
   // completed the command when it really had not even started the
   // command yet.

   DELAY400NS;

   // Command packet transfer...
   // Check for protocol failures,
   // the device should have BSY=1 or
   // if BSY=0 then either DRQ=1 or CHK=1.

	PRINTIT(0);
   sub_atapi_delay( dev );
   status = pio_inbyte( CB_ASTAT );
   if ( status & CB_STAT_BSY )
   {
      // BSY=1 is OK
   }
   else
   {
      if ( status & ( CB_STAT_DRQ | CB_STAT_ERR ) )
      {
         // BSY=0 and DRQ=1 is OK
         // BSY=0 and ERR=1 is OK
      }
      else
      {
         reg_cmd_info.failbits |= FAILBIT0;  // not OK
      }
   }

   // Command packet transfer...
   // Poll Alternate Status for BSY=0.

	PRINTIT(0);
   while ( 1 )
   {
	PRINTIT(0);
      status = pio_inbyte( CB_ASTAT );       // poll for not busy
      if ( ( status & CB_STAT_BSY ) == 0 )
	  {
	     PRINTIT(0);
             break;
	  }
	PRINTIT(0);
      if ( tmr_chk_timeout() )               // time out yet ?
      {
         reg_cmd_info.to = 1;
         reg_cmd_info.ec = 51;
         dir = -1;   // command done
  	 PRINTIT(0);
         break;
      }
   }

   // Command packet transfer...
   // Check for protocol failures... no interrupt here please!
   // Clear any interrupt the command packet transfer may have caused.

//   if ( int_intr_flag )
//      reg_cmd_info.failbits |= FAILBIT1;
//   int_intr_flag = 0;

   // Command packet transfer...
   // If no error, transfer the command packet.

	PRINTIT(0);
   if ( reg_cmd_info.ec == 0 )
   {
	PRINTIT(0);

      // Command packet transfer...
      // Read the primary status register and the other ATAPI registers.

      status = pio_inbyte( CB_STAT );
      reason = pio_inbyte( CB_SC );
      lowCyl = pio_inbyte( CB_CL );
      highCyl = pio_inbyte( CB_CH );

      // Command packet transfer...
      // check status: must have BSY=0, DRQ=1 now

      if (    ( status & ( CB_STAT_BSY | CB_STAT_DRQ | CB_STAT_ERR ) )
           != CB_STAT_DRQ
         )
      {
		PRINTIT(0);
         reg_cmd_info.ec = 52;
         dir = -1;   // command done
      }
      else
      {
		PRINTIT(0);
         // Command packet transfer...
         // Check for protocol failures...
         // check: C/nD=1, IO=0.

         if ( ( reason &  ( CB_SC_P_TAG | CB_SC_P_REL | CB_SC_P_IO ) )
              || ( ! ( reason &  CB_SC_P_CD ) )
            )
            reg_cmd_info.failbits |= FAILBIT2;
         if ( ( lowCyl != cylLow ) || ( highCyl != cylHigh ) )
            reg_cmd_info.failbits |= FAILBIT3;

         // Command packet transfer...
         // trace cdb byte 0,
         // xfer the command packet (the cdb)

         pio_rep_outword( CB_DATA, cpbuf, cpbc >> 1 );

         DELAY400NS;    // delay so device can get the status updated
		PRINTIT(0);
      }
   }

   // Data transfer loop...
   // If there is no error, enter the data transfer loop.
   // First adjust the I/O buffer address so we are able to
   // transfer large amounts of data (more than 64K).

	PRINTIT(0);
   dpaddr = (unsigned long)dpbuf;

	PRINTIT(0);
   while ( reg_cmd_info.ec == 0 )
   {
		PRINTIT(0);
      // Data transfer loop...
      // Wait for INT 7x -or- wait for not BUSY -or- wait for time out.

      sub_atapi_delay( dev );
		PRINTIT(0);
      reg_wait_poll( 53, 54 );
		PRINTIT(0);

      // Data transfer loop...
      // If there was a time out error, exit the data transfer loop.

      if ( (reg_cmd_info.ec)  )
      {
         dir = -1;   // command done
		 PRINTIT(reg_cmd_info.ec);
         break;
      }

      // Data transfer loop...
      // Read the primary status register.

      status = pio_inbyte( CB_STAT );
      reason = pio_inbyte( CB_SC );
      lowCyl = pio_inbyte( CB_CL );
      highCyl = pio_inbyte( CB_CH );

      // Data transfer loop...
      // Exit the read data loop if the device indicates this
      // is the end of the command.

		PRINTIT(0);
      if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) == 0 )
      {
         dir = -1;   // command done
		 PRINTIT(0);
         break;
      }

      // Data transfer loop...
      // The device must want to transfer data...
      // check status: must have BSY=0, DRQ=1 now.

		PRINTIT(0);
      if ( ( status & ( CB_STAT_BSY | CB_STAT_DRQ ) ) != CB_STAT_DRQ )
      {
         reg_cmd_info.ec = 55;
         dir = -1;   // command done
		 PRINTIT(0);
         break;
      }
      else
      {
		PRINTIT(0);
         // Data transfer loop...
         // Check for protocol failures...
         // check: C/nD=0, IO=1 (read) or IO=0 (write).

         if ( ( reason &  ( CB_SC_P_TAG | CB_SC_P_REL ) )
              || ( reason &  CB_SC_P_CD )
            )
            reg_cmd_info.failbits |= FAILBIT4;
         if ( ( reason & CB_SC_P_IO ) && dir )
            reg_cmd_info.failbits |= FAILBIT5;
      }

      // do the slow data transfer thing
      int reg_slow_xfer_flag = 0;

		PRINTIT(0);
      if ( reg_slow_xfer_flag )
      {
         slowXferCntr ++ ;
         if ( slowXferCntr <= reg_slow_xfer_flag )
         {
            sub_xfer_delay();
            reg_slow_xfer_flag = 0;
         }
      }

		PRINTIT(0);
      // Data transfer loop...
      // get the byte count, check for zero...

      byteCnt = ( highCyl << 8 ) | lowCyl;
      if ( byteCnt < 1 )
      {
         reg_cmd_info.ec = 60;
         dir = -1;   // command done
		 PRINTIT(0);
         break;
      }
		PRINTIT(0);

      // Data transfer loop...
      // and check protocol failures...

      if ( (int)byteCnt > dpbc )
         reg_cmd_info.failbits |= FAILBIT6;
      reg_cmd_info.failbits |= prevFailBit7;
      prevFailBit7 = 0;
      if ( byteCnt & 0x0001 )
         prevFailBit7 = FAILBIT7;

      // Data transfer loop...
      // make sure we don't overrun the caller's buffer

		PRINTIT(0);
      if ( (int)( reg_cmd_info.totalBytesXfer + byteCnt ) > reg_atapi_max_bytes )
      {
         reg_cmd_info.ec = 59;
         dir = -1;   // command done
		 PRINTIT(0);
         break;
      }
		PRINTIT(0);

      // increment number of DRQ packets

      reg_cmd_info.drqPackets ++ ;

      // Data transfer loop...
      // transfer the data and update the i/o buffer address
      // and the number of bytes transfered.

      wordCnt = ( byteCnt >> 1 ) + ( byteCnt & 0x0001 );
      reg_cmd_info.totalBytesXfer += ( wordCnt << 1 );
      dpbuf = (void*)dpaddr;
      if ( dir )
	  {
		 PRINTIT(0);
         pio_rep_outword( CB_DATA, dpbuf, wordCnt );
	  }
      else
	  {
		 PRINTIT(0);
         pio_rep_inword( CB_DATA, dpbuf, wordCnt );
	  }
      dpaddr = dpaddr + byteCnt;

      DELAY400NS;    // delay so device can get the status updated
	  PRINTIT(0);
   }
	PRINTIT(0);

   // End of command...
   // Wait for interrupt or poll for BSY=0,
   // but don't do this if there was any error or if this
   // was a commmand that did not transfer data.

   if ( ( reg_cmd_info.ec == 0 ) && ( dir >= 0 ) )
   {
      sub_atapi_delay( dev );
      reg_wait_poll( 56, 57 );
   }
	PRINTIT(0);

   // Final status check, only if no previous error.

   if ( reg_cmd_info.ec == 0 )
   {
      // Final status check...
      // Read the primary status register and other regs.

      status = pio_inbyte( CB_STAT );
      reason = pio_inbyte( CB_SC );
      lowCyl = pio_inbyte( CB_CL );
      highCyl = pio_inbyte( CB_CH );

      // Final status check...
      // check for any error.

      if ( status & ( CB_STAT_BSY | CB_STAT_DRQ | CB_STAT_ERR ) )
      {
         reg_cmd_info.ec = 58;
      }
   }

   // Final status check...
   // Check for protocol failures...
   // check: C/nD=1, IO=1.

	PRINTIT(0);
   if ( ( reason & ( CB_SC_P_TAG | CB_SC_P_REL ) )
        || ( ! ( reason & CB_SC_P_IO ) )
        || ( ! ( reason & CB_SC_P_CD ) )
      )
      reg_cmd_info.failbits |= FAILBIT8;

   // Done...
   // Read the output registers and trace the command.
   // Also put the command packet in the trace.

		PRINTIT(0);
   if ( ! reg_cmd_info.totalBytesXfer )
      reg_cmd_info.ct = TRC_TYPE_PND;
   sub_trace_command();

   // For interrupt mode, restore the INT 7x vector.

//   int_restore_int_vect();

   // mark end of PI cmd in low level trace


   // All done.  The return values of this function are described in
   // ATAIO.H.

	PRINTIT(0);
   reg_atapi_max_bytes = 32768L;    // reset max bytes
   if ( reg_cmd_info.ec )
   {
      PRINTIT(reg_cmd_info.ec);
      return 1;
   }
   
   PRINTIT(0);
   return 0;
}



// ******************************************************

bool ATAPINode::Present()
{
   unsigned char sc;
   unsigned char sn;
   unsigned char cl;
   unsigned char ch;
   unsigned char st;
   unsigned char devCtrl;
   int attempts;

   // setup register values

   devCtrl = CB_DC_HD15 | ( int_use_intr_flag ? 0 : CB_DC_NIEN );

   unsigned char devSel;

   if ( _device == 0 ) devSel = CB_DH_DEV0;
		   		  else devSel = CB_DH_DEV1;
   
   reg_config_info[_device] = REG_CONFIG_TYPE_NONE;

   smk_disable_interrupts();
	
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

  PRINTIT(0);
   pio_outbyte( CB_DH, devSel );
   DELAY400NS;
   reg_reset( 0, _device );
  PRINTIT(0);

   // lets check device 0 again, is device 0 really there?
   // is it ATA or ATAPI?

   for ( attempts = 0; attempts < 10; attempts++ )
   {
       reg_config_info[_device] = REG_CONFIG_TYPE_NONE;
	   
	   pio_outbyte( CB_DH, devSel );
	   DELAY400NS;
	   sc = pio_inbyte( CB_SC );
	   sn = pio_inbyte( CB_SN );

//	   printf("%s (%x,%x)\n","ATAPI device returned FIRST:", sc,sn );
	   
	   if ( ( sc == 0x01 ) && ( sn == 0x01 ) )
	   {
	      reg_config_info[_device] = REG_CONFIG_TYPE_UNKN;
	      cl = pio_inbyte( CB_CL );
	      ch = pio_inbyte( CB_CH );
	      st = pio_inbyte( CB_STAT );

//	   	  printf("%s (%x,%x)\n","ATAPI device returned SECOND:", sc,sn );

		  
	      if ( ( cl == 0x14 ) && ( ch == 0xeb ) )
	         reg_config_info[_device] = REG_CONFIG_TYPE_ATAPI;
	      else
	         if ( ( cl == 0x00 ) && ( ch == 0x00 ) && ( st != 0x00 ) )
	            reg_config_info[_device] = REG_CONFIG_TYPE_ATA;
	   }
	
	   // Is there a device present?
	      if ( reg_config_info[_device] != REG_CONFIG_TYPE_UNKN ) 
		  {
				  //printf("%s\n","ATAPINode::Present unknown device.");
				  break;
		  }
		  
		  //printf("%s%i%s%i\n", "ATAPINode::Present failed for ",
				 //_device, " on attempt ", attempts );
   	   
	   pio_outbyte( CB_DH, devSel );
	   DELAY400NS;			// delete
       reg_reset( 0, _device );	// delete
       //printf("%s\n","ATAPINode::Present() reset.");
	}
				  
	      if ( reg_config_info[_device] == REG_CONFIG_TYPE_ATAPI )
		   	{
			  //printf("%s%i\n","ATAPINode::Present succeeded on ", _device );
		      pio_outbyte( CB_DH, devSel );

			  smk_enable_interrupts();
			  return true;
	 	    }

	smk_enable_interrupts();
	  
   return false;
}


// ******************************************************

void ATAPINode::pio_set_iobase_addr( unsigned int base1, unsigned int base2 )
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



void ATAPINode::reg_wait_poll( int we, int pe )

{
   //printf("IN [ %i --> %i :  %i ]\n", tmr_read_bios_timer(), tmr_time_out, timeOut );
   
   // Wait for INT 7x -or- wait for not BUSY -or- wait for time out.

      while ( 1 )
      {
         unsigned char status = pio_inbyte( CB_ASTAT );       // poll for not busy
         if ( ( status & CB_STAT_BSY ) == 0 )
            break;
         if ( tmr_chk_timeout() != 0 )               // time out yet ?
         {
            reg_cmd_info.to = 1;
            reg_cmd_info.ec = pe;
            break;
         }

      }


   //printf("OUT [ %i --> %i :  %i ]\n", tmr_read_bios_timer(), tmr_time_out, timeOut );
}





//*************************************************************
//
// These functions do basic IN/OUT of byte and word values.
//
//*************************************************************


unsigned char ATAPINode::pio_inbyte( unsigned int addr )
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

void ATAPINode::pio_outbyte( unsigned int addr, unsigned char data )
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

unsigned int ATAPINode::pio_inword( unsigned int addr )
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

void ATAPINode::pio_outword( unsigned int addr, unsigned int data )
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

void ATAPINode::pio_rep_inbyte( unsigned int addrDataReg,
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
			"  mov %0, %%edi\n"
			"  mov %1, %%ecx\n"
			"  mov %2, %%edx\n"
	  
			"  cld\n"
	  
			"  rep insb\n"

     :
	 : "p" (buffer),
	   "g" (bCnt), "g" (dataRegAddr)
	 : "eax", "ecx", "edx", "edi"
      );



      byteCnt = byteCnt - (long) bCnt;

      pio_inbyte( CB_ASTAT );    // just for debugging

   }
}

//*********************************************************

// Note: pio_rep_outbyte() can be called directly but usually it
// is called by pio_rep_outword() based on the value of the
// pio_xfer_width variables.

void ATAPINode::pio_rep_outbyte( unsigned int addrDataReg,
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
			"  mov %0, %%esi\n"
			"  mov %1, %%ecx\n"
			"  mov %2, %%edx\n"
	  
			"  cld\n"
	  
			"  rep outsb\n"
         :
		 : "p" (buffer),
		   "g" (bCnt), "g" (dataRegAddr)
		 : "eax", "ecx", "edx", "esi"
      );


      byteCnt = byteCnt - (long) bCnt;

      pio_inbyte( CB_ASTAT );    // just for debugging

   }
}

//*********************************************************

// Note: pio_rep_indword() can be called directly but usually it
// is called by pio_rep_inword() based on the value of the
// pio_xfer_width variable.

void ATAPINode::pio_rep_indword( unsigned int addrDataReg,
                      void* buffer,
                      unsigned int dwordCnt )
{
   unsigned int dataRegAddr;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];


      __asm__ __volatile__
      (
		"  mov %0, %%edi\n"
		"  mov %1, %%ecx\n"
		"  mov %2, %%edx\n"
		  
		"  cld\n"
	  
		"  rep insl\n"
     :
	 : "p" (buffer),
	   "g" (dwordCnt), "g" (dataRegAddr)
	 : "eax", "edx", "ecx", "edi"
      );


}

//*********************************************************

// Note: pio_rep_outdword() can be called directly but usually it
// is called by pio_rep_outword() based on the value of the
// pio_xfer_width variable.

void ATAPINode::pio_rep_outdword( unsigned int addrDataReg,
                       void* buffer,
                       unsigned int dwordCnt )
{
   unsigned int dataRegAddr;

   dataRegAddr = pio_reg_addrs[ addrDataReg ];

      __asm__ __volatile__
      (
		"  mov %0, %%esi\n"
		"  mov %1, %%ecx\n"
		"  mov %2, %%edx\n"
		  
		"  cld\n"
	  
		"  rep outsl\n"
     :
	 : "p" (buffer),
	   "g" (dwordCnt), "g" (dataRegAddr)
	 : "eax", "ecx", "edx", "esi"
      );

}

//*********************************************************

// Note: pio_rep_inword() is the primary way perform PIO
// Data In transfers. It will handle 8-bit, 16-bit and 32-bit
// I/O based data transfers and 8-bit and 16-bit PCMCIA Memory
// mode transfers.

void ATAPINode::pio_rep_inword( unsigned int addrDataReg,
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
			"  mov %0, %%edi\n"
			"  mov %1, %%ecx\n"
			"  mov %2, %%edx\n"
	  
			"  cld\n"
	  
			"  rep insw\n"
         :
		 : "p" (buffer),
		   "g" (wordCnt), "g" (dataRegAddr)
		: "eax", "ecx", "edx", "edi"
      );

   }
}

//*********************************************************

// Note: pio_rep_outword() is the primary way perform PIO
// Data Out transfers. It will handle 8-bit, 16-bit and 32-bit
// I/O based data transfers and 8-bit and 16-bit PCMCIA Memory
// mode transfers.

void ATAPINode::pio_rep_outword( unsigned int addrDataReg,
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
		 " mov %0, %%esi\n"
		 " mov %1, %%ecx\n"
		 " mov %2, %%edx\n"
	  
		 " cld\n"
	  
		 " rep outsw\n"
         :
		 : "p" (buffer),
		   "g" (wordCnt), "g" (dataRegAddr)
		 : "eax", "ecx", "edx", "esi"
      );

   }
}


void ATAPINode::Reset( bool setflags )
{
  	PRINTIT( 0 );
	if ( setflags ) 
	{
		smk_disable_interrupts();
		PRINTIT(0);
	}

   		unsigned char devSel;

	    if ( _device == 0 ) devSel = CB_DH_DEV0;
		               else devSel = CB_DH_DEV1;
	
	   DELAY400NS;
	   pio_outbyte( CB_DH, devSel );
	   DELAY400NS;
	   reg_reset( 0, _device );

	if ( setflags ) 
	{
		smk_enable_interrupts();
		PRINTIT(0);
	}
  	PRINTIT( 0 );
}


//*************************************************************
//
// reg_reset() - Execute a Software Reset.
//
// See ATA-2 Section 9.2, ATA-3 Section 9.2, ATA-4 Section 8.3.
//
//*************************************************************

int ATAPINode::reg_reset( int skipFlag, int devRtrn )

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

   PRINTIT( 0 );
   if ( ! skipFlag )
   {
      pio_outbyte( CB_DC, devCtrl | CB_DC_SRST );
      DELAY400NS;
      pio_outbyte( CB_DC, devCtrl );
      DELAY400NS;
   }
   PRINTIT( 0 );


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

   PRINTIT( 0 );
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
   PRINTIT( 0 );

   // RESET_DONE:

   // We are done but now we must select the device the caller
   // requested before we trace the command.  This will cause
   // the correct data to be returned in reg_cmd_info.

   pio_outbyte( CB_DH, devRtrn ? CB_DH_DEV1 : CB_DH_DEV0 );
   DELAY400NS;
   sub_trace_command();

   // If possible, select a device that exists,
   // try device 0 first.

  /*
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
   */

   // mark end of reset in low level trace


   // All done.  The return values of this function are described in
   // ATAIO.H.

   if ( reg_cmd_info.ec )
   {
      PRINTIT( reg_cmd_info.ec );
      return 1;
   }
   PRINTIT( 0 );
   return 0;
}

//*************************************************************
//
// sub_zero_return_data() -- zero the return data areas.
//
//*************************************************************

void ATAPINode::sub_zero_return_data( void )

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

void ATAPINode::sub_atapi_delay( int dev )
{
    smk_ndelay(110);
}

void ATAPINode::sub_xfer_delay( void )
{
    smk_ndelay(60);
}



//*************************************************************
//
// sub_trace_command() -- trace the end of a command.
//
//*************************************************************

void ATAPINode::sub_trace_command( void )
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

int ATAPINode::sub_select( int dev )
{
   unsigned char status;

   // PAY ATTENTION HERE
   // The caller may want to issue a command to a device that doesn't
   // exist (for example, Exec Dev Diag), so if we see this,
   // just select that device, skip all status checking and return.
   // We assume the caller knows what they are doing!

   PRINTIT(0);
   pio_outbyte( CB_DH, dev ? CB_DH_DEV1 : CB_DH_DEV0 );
   DELAY400NS;
	  
   if ( reg_config_info[ dev ] < REG_CONFIG_TYPE_ATA )
   {
	  PRINTIT(0);
      // select the device and return
      return 0;
   }
   PRINTIT(0);

   // The rest of this is the normal ATA stuff for device selection
   // and we don't expect the caller to be selecting a device that
   // does not exist.
   // We don't know which drive is currently selected but we should
   // wait for it to be not BUSY.  Normally it will be not BUSY
   // unless something is very wrong!

   while ( 1 )
   {
	  PRINTIT(0);
      status = pio_inbyte( CB_STAT );
      if ( ( status & CB_STAT_BSY ) == 0 )
         break;
	  PRINTIT(status);
      if ( tmr_chk_timeout() )
      {
	     PRINTIT(0);
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
   PRINTIT(0);

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

   PRINTIT(0);
   while ( 1 )
   {
	  PRINTIT(0);
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
	  PRINTIT(0);
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
   {
      PRINTIT( reg_cmd_info.ec );
      return 1;
   }
   PRINTIT(0);
   return 0;
}



//*************************************************************
//
// tmr_set_timeout() - function used to set command timeout time
//
// The command time out time is computed as follows:
//
//    timer + tmr_time_out
//
//**************************************************************

void ATAPINode::tmr_set_timeout( void )

{
   timeOut = tmr_read_bios_timer() + tmr_time_out;
}

//*************************************************************
//
// tmr_chk_timeout() - function used to check for command timeout.
//
// Gives non-zero return if command has timed out.
//
//**************************************************************

int ATAPINode::tmr_chk_timeout( void )
{
   unsigned int time = tmr_read_bios_timer();
   if ( time >= timeOut ) 
   {
     //printf("%s%i\n","tmr_chk_timeout invalid = ", time );
     return 1;
   }
   return 0;
}

//*************************************************************
//
// tmr_read_bios_timer() - function to read the BIOS timer
//
//**************************************************************

unsigned int ATAPINode::tmr_read_bios_timer( void )
{
   uint32 seconds;
   uint32 milli;

   smk_system_time( &seconds, &milli );
   return seconds;
}



// **********************************************

int ATAPINode::reg_pio_data_in_lba( int dev, int cmd,
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

int ATAPINode::reg_pio_data_in( int dev, int cmd,
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
      	PRINTIT( reg_cmd_info.ec );
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
      	 PRINTIT( reg_cmd_info.ec );
         break;   // go to READ_DONE
      }

      // DRQ should have been set -- was it?

      if ( ( status & CB_STAT_DRQ ) == 0 )
      {
//         printf("%s\n","ata: drq was not set. ");
         reg_cmd_info.ec = 32;
      	 PRINTIT( reg_cmd_info.ec );
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
      	    PRINTIT( reg_cmd_info.ec );
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
   {
	  printf("%s%i\n","ATAPI error code = ", reg_cmd_info.ec );
	  PRINTIT( reg_cmd_info.ec );
      return 1;
   }
   return 0;
}


//*************************************************************
//
// reg_pio_data_out_lba() - Easy way to execute a PIO Data In command
//                          using an LBA sector address.
//
//*************************************************************

int ATAPINode::reg_pio_data_out_lba( int dev, int cmd,
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

int ATAPINode::reg_pio_data_out( int dev, int cmd,
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
         if ( (int)wordCnt > numSect )
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
   {
	  printf("%s%i\n","ATAPI error code = ", reg_cmd_info.ec );
      	 PRINTIT( reg_cmd_info.ec );
      return 1;
   }
   return 0;
}


