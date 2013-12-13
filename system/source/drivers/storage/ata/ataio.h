//********************************************************************
// ATA LOW LEVEL I/O DRIVER -- ATAIO.H (driver's public data)
//
// by Hale Landis (hlandis@ibm.net)
//
// There is no copyright and there are no restrictions on the use
// of this ATA Low Level I/O Driver code.  It is distributed to
// help other programmers understand how the ATA device interface
// works and it is distributed without any warranty.  Use this
// code at your own risk.
//
// This code is based on the ATA-2, ATA-3 and ATA-4 standards and
// on interviews with various ATA controller and drive designers.
//
// This code has been run on many ATA (IDE) drives and
// MFM/RLL controllers.  This code may be a little
// more picky about the status it sees at various times.  A real
// BIOS probably would not check the status as carefully.
//
// Compile with one of the Borland C or C++ compilers.
//
// This C source file is the header file for the for this driver
// and is used in the ATAIOxxx.C files and must also be used
// by any program using this driver code.
//********************************************************************

#ifndef _ATAIO_H
#define _ATAIO_H

#define ATA_DRIVER_VERSION "14C"


#define 	BASE1	0x1f0
#define		BASE2	0x170

// Global defines -- ATA register and register bits.
//
// command block & control block regs
//
// these are the offsets into pio_reg_addrs[]

#define CB_DATA  0   // data reg         in/out pio_base_addr1+0
#define CB_ERR   1   // error            in     pio_base_addr1+1
#define CB_FR    1   // feature reg         out pio_base_addr1+1
#define CB_SC    2   // sector count     in/out pio_base_addr1+2
#define CB_SN    3   // sector number    in/out pio_base_addr1+3
#define CB_CL    4   // cylinder low     in/out pio_base_addr1+4
#define CB_CH    5   // cylinder high    in/out pio_base_addr1+5
#define CB_DH    6   // device head      in/out pio_base_addr1+6
#define CB_STAT  7   // primary status   in     pio_base_addr1+7
#define CB_CMD   7   // command             out pio_base_addr1+7
#define CB_ASTAT 8   // alternate status in     pio_base_addr2+6
#define CB_DC    8   // device control      out pio_base_addr2+6
#define CB_DA    9   // device address   in     pio_base_addr2+7

// error reg (CB_ERR) bits

#define CB_ER_ICRC 0x80    // ATA Ultra DMA bad CRC
#define CB_ER_BBK  0x80    // ATA bad block
#define CB_ER_UNC  0x40    // ATA uncorrected error
#define CB_ER_MC   0x20    // ATA media change
#define CB_ER_IDNF 0x10    // ATA id not found
#define CB_ER_MCR  0x08    // ATA media change request
#define CB_ER_ABRT 0x04    // ATA command aborted
#define CB_ER_NTK0 0x02    // ATA track 0 not found
#define CB_ER_NDAM 0x01    // ATA address mark not found

#define CB_ER_P_SNSKEY 0xf0   // ATAPI sense key (mask)
#define CB_ER_P_MCR    0x08   // ATAPI Media Change Request
#define CB_ER_P_ABRT   0x04   // ATAPI command abort
#define CB_ER_P_EOM    0x02   // ATAPI End of Media
#define CB_ER_P_ILI    0x01   // ATAPI Illegal Length Indication

// ATAPI Interrupt Reason bits in the Sector Count reg (CB_SC)

#define CB_SC_P_TAG    0xf8   // ATAPI tag (mask)
#define CB_SC_P_REL    0x04   // ATAPI release
#define CB_SC_P_IO     0x02   // ATAPI I/O
#define CB_SC_P_CD     0x01   // ATAPI C/D

// bits 7-4 of the device/head (CB_DH) reg

#define CB_DH_DEV0 0xa0    // select device 0
#define CB_DH_DEV1 0xb0    // select device 1

// status reg (CB_STAT and CB_ASTAT) bits

#define CB_STAT_BSY  0x80  // busy
#define CB_STAT_RDY  0x40  // ready
#define CB_STAT_DF   0x20  // device fault
#define CB_STAT_WFT  0x20  // write fault (old name)
#define CB_STAT_SKC  0x10  // seek complete
#define CB_STAT_SERV 0x10  // service
#define CB_STAT_DRQ  0x08  // data request
#define CB_STAT_CORR 0x04  // corrected
#define CB_STAT_IDX  0x02  // index
#define CB_STAT_ERR  0x01  // error (ATA)
#define CB_STAT_CHK  0x01  // check (ATAPI)

// device control reg (CB_DC) bits

#define CB_DC_HD15   0x08  // bit should always be set to one
#define CB_DC_SRST   0x04  // soft reset
#define CB_DC_NIEN   0x02  // disable interrupts

//**************************************************************

// Most mandtory and optional ATA commands (from ATA-3),

#define CMD_CFA_ERASE_SECTORS            0xC0
#define CMD_CFA_REQUEST_EXT_ERR_CODE     0x03
#define CMD_CFA_TRANSLATE_SECTOR         0x87
#define CMD_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define CMD_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define CMD_CHECK_POWER_MODE1            0xE5
#define CMD_CHECK_POWER_MODE2            0x98
#define CMD_DEVICE_RESET                 0x08
#define CMD_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define CMD_FLUSH_CACHE                  0xE7
#define CMD_FORMAT_TRACK                 0x50
#define CMD_IDENTIFY_DEVICE              0xEC
#define CMD_MEDIA_EJECT			 0xED
#define CMD_IDENTIFY_DEVICE_PACKET       0xA1
#define CMD_IDENTIFY_PACKET_DEVICE       0xA1
#define CMD_IDLE1                        0xE3
#define CMD_IDLE2                        0x97
#define CMD_IDLE_IMMEDIATE1              0xE1
#define CMD_IDLE_IMMEDIATE2              0x95
#define CMD_INITIALIZE_DRIVE_PARAMETERS  0x91
#define CMD_INITIALIZE_DEVICE_PARAMETERS 0x91
#define CMD_NOP                          0x00
#define CMD_PACKET                       0xA0
#define CMD_READ_BUFFER                  0xE4
#define CMD_READ_DMA                     0xC8
#define CMD_READ_DMA_QUEUED              0xC7
#define CMD_READ_MULTIPLE                0xC4
#define CMD_READ_SECTORS                 0x20
#define CMD_READ_VERIFY_SECTORS          0x40
#define CMD_RECALIBRATE                  0x10
#define CMD_SEEK                         0x70
#define CMD_SET_FEATURES                 0xEF
#define CMD_SET_MULTIPLE_MODE            0xC6
#define CMD_SLEEP1                       0xE6
#define CMD_SLEEP2                       0x99
#define CMD_STANDBY1                     0xE2
#define CMD_STANDBY2                     0x96
#define CMD_STANDBY_IMMEDIATE1           0xE0
#define CMD_STANDBY_IMMEDIATE2           0x94
#define CMD_WRITE_BUFFER                 0xE8
#define CMD_WRITE_DMA                    0xCA
#define CMD_WRITE_DMA_QUEUED             0xCC
#define CMD_WRITE_MULTIPLE               0xC5
#define CMD_WRITE_SECTORS                0x30
#define CMD_WRITE_VERIFY                 0x3C

//

// real-mode compats

#define MK_FP(seg,off) ((seg<<4) | off)
#define FP_OFF(val) (((unsigned int)val) & 0xffff)
#define FP_SEG(val) (((unsigned int)val)>>4)
typedef void (*int_func)();
void memset( void* buffer, char c, unsigned int len );
void enable();
void disable();
void setvect( int vector, int_func bob );
int_func getvect( int vector );



//**************************************************************

// Public functions in ATAIOISA.C

extern int dma_isa_config( int chan );

extern int dma_isa_ata_lba( int dev, int cmd,
                            int fr, int sc,
                            long lba,
                            unsigned seg, unsigned off );

extern int dma_isa_ata( int dev, int cmd,
                        int fr, int sc,
                        unsigned int cyl, int head, int sect,
                        unsigned seg, unsigned off );

extern int dma_isa_packet( int dev,
                           unsigned int cpbc,
                           unsigned int cpseg, unsigned int cpoff,
                           int dir,
                           long dpbc,
                           unsigned int dpseg, unsigned int dpoff );

//**************************************************************

// Public data in ATAIOINT.C

// Interrupt mode flag (interrupts in use if != 0 )

// IRQ number in use (valid if int_use_intr_flag != 0 )

extern int int_irq_number;

// INT vector in use (valid if int_use_intr_flag != 0 )

extern int int_int_vector;

//**************************************************************

// Public functions in ATAIOINT.C

extern int int_enable_irq( int irqNum );

extern void int_disable_irq( void );

//**************************************************************

// Public data in ATAIOPCI.C

extern unsigned int dma_pci_sff_reg_addr;    // SFF-8038 reg address
extern unsigned int dma_pci_prd_addr;        // PRD address

// Public functions in ATAIOPCI.C

extern int dma_pci_config( unsigned int regAddr );

extern int dma_pci_ata_lba( int dev, int cmd,
                            int fr, int sc,
                            long lba,
                            unsigned seg, unsigned off );

extern int dma_pci_ata( int dev, int cmd,
                        int fr, int sc,
                        unsigned int cyl, int head, int sect,
                        unsigned seg, unsigned off );

extern int dma_pci_packet( int dev,
                           unsigned int cpbc,
                           unsigned int cpseg, unsigned int cpoff,
                           int dir,
                           long dpbc,
                           unsigned int dpseg, unsigned int dpoff );

//**************************************************************

// Public data in ATAIOPIO.C

extern unsigned int pio_base_addr1;
extern unsigned int pio_base_addr2;
extern unsigned int pio_memory_seg;

extern unsigned int pio_reg_addrs[10];

extern unsigned char pio_last_write[10];
extern unsigned char pio_last_read[10];

extern int pio_xfer_width;

//**************************************************************

// Public functions in ATAIOPIO.C

extern void pio_set_iobase_addr( unsigned int base1, unsigned int base2 );

extern void pio_set_memory_addr( unsigned int seg );

extern unsigned char pio_inbyte( unsigned int addr );

extern void pio_outbyte( unsigned int addr, unsigned char data );

extern unsigned int pio_inword( unsigned int addr );

extern void pio_outword( unsigned int addr, unsigned int data );

extern void pio_rep_inbyte( unsigned int addrDataReg,
                            void* buffer,
                            long byteCnt );

extern void pio_rep_outbyte( unsigned int addrDataReg,
                             void* buffer,
                             long byteCnt );

extern void pio_rep_inword( unsigned int addrDataReg,
                            void* buffer,
                            unsigned int wordCnt );

extern void pio_rep_outword( unsigned int addrDataReg,
                             void* buffer,
                             unsigned int wordCnt );

extern void pio_rep_indword( unsigned int addrDataReg,
                             void* buffer,
                             unsigned int dwordCnt );

extern void pio_rep_outdword( unsigned int addrDataReg,
                              void* buffer,
                              unsigned int dwordCnt );

//**************************************************************

// Public data in ATAIOREG.C

// Extended error information returned by
// reg_reset(), reg_non_data(), reg_pio_data_in() and reg_pio_data_out().

struct REG_CMD_INFO
{
   unsigned char flg;         // see TRC_FLAG_xxx below
   unsigned char ct;          // see TRC_TYPE_xxx below
   unsigned char cmd;         // command code
   unsigned char fr1;         // feature reg before
   unsigned char sc1;         // sec cnt before
   unsigned char sn1;         // sec num before
   unsigned char cl1;         // cyl low before
   unsigned char ch1;         // cyl high before
   unsigned char dh1;         // device head before
   unsigned char dc1;         // device control before
   unsigned char ec;          // detailed error code
   unsigned char to;          // not zero if time out error
   unsigned char st2;         // status reg after
   unsigned char as2;         // alt status reg after
   unsigned char er2;         // error reg after
   unsigned char sc2;         // sec cnt after
   unsigned char sn2;         // sec num after
   unsigned char cl2;         // cyl low after
   unsigned char ch2;         // cyl high after
   unsigned char dh2;         // device head after
   long totalBytesXfer;       // total bytes transfered
   unsigned int failbits;     // failure bits (ATAPI protocol errors
      #define FAILBIT15 0x8000   // extra interrupts detected
      #define FAILBIT14 0x4000
      #define FAILBIT13 0x2000
      #define FAILBIT12 0x1000
      #define FAILBIT11 0x0800
      #define FAILBIT10 0x0400
      #define FAILBIT9  0x0200
      #define FAILBIT8  0x0100   // SC( CD/IO bits) wrong at end of cmd
      #define FAILBIT7  0x0080   // byte count odd at data packet xfer time
      #define FAILBIT6  0x0040   // byte count wrong at data packet xfer time
      #define FAILBIT5  0x0020   // SC (IO bit) wrong at data packet xfer time
      #define FAILBIT4  0x0010   // SC (CD bit) wrong at data packet xfer time
      #define FAILBIT3  0x0008   // byte count wrong at cmd packet xfer time
      #define FAILBIT2  0x0004   // SC wrong at cmd packet xfer time
      #define FAILBIT1  0x0002   // got interrupt before cmd packet xfer
      #define FAILBIT0  0x0001   // slow setting BSY=1 or DRQ=1 after AO cmd
   long drqPackets;           // number of PIO DRQ packets
} ;

extern struct REG_CMD_INFO reg_cmd_info;

// Configuration data for device 0 and 1
// returned by the reg_config() function.

extern int reg_config_info[2];

#define REG_CONFIG_TYPE_NONE  0
#define REG_CONFIG_TYPE_UNKN  1
#define REG_CONFIG_TYPE_ATA   2
#define REG_CONFIG_TYPE_ATAPI 3

// flag to control the ~110ms delay for ATAPI devices,
// no delay if the flag is zero.

extern int reg_atapi_delay_flag;

// flag to control the slow data transfer option:
// 0 = no slow data transfer
// !0= slow data transfer before this sector

extern unsigned char reg_atapi_reg_fr;
extern unsigned char reg_atapi_reg_sc;
extern unsigned char reg_atapi_reg_sn;
extern unsigned char reg_atapi_reg_dh;

// the values in these variables are placed into the Feature,
// Sector Count, Sector Number and Device/Head register by
// reg_packet() before the A0H command is issued.  reg_packet()
// sets these variables to zero before returning.  These variables
// are initialized to zero.  Only bits 3,2,1,0 of reg_atapi_reg_dh
// are used.

extern int reg_slow_xfer_flag;

// the maximum number of bytes that the reg_packet() function
// will transfer on the next packet command.  the caller
// can set this to the caller's buffer size minus the
// maximum DRQ packet size so that the packet command
// does not overrun the caller's buffer in memory.
// this is set to 32768 at the end of each packet command.

extern long reg_atapi_max_bytes;

// last ATAPI command packet size and data

extern int reg_atapi_cp_size;
extern unsigned char reg_atapi_cp_data[16];

//**************************************************************

// Public functions in ATAIOREG.C

extern int reg_config( void );

extern int reg_reset( int skipFlag, int devRtrn );

extern int reg_non_data_lba( int dev, int cmd,
                             int fr, int sc,
                             long lba );

extern int reg_non_data( int dev, int cmd,
                           int fr, int sc,
                           unsigned int cyl, int head, int sect );

extern int reg_pio_data_in_lba( int dev, int cmd,
                                int fr, int sc,
                                long lba,
				void* buffer,
                                int numSect, int multiCnt );

extern int reg_pio_data_in( int dev, int cmd,
                            int fr, int sc,
                            unsigned int cyl, int head, int sect,
			    void* buffer,
                            int numSect, int multiCnt );

extern int reg_pio_data_out_lba( int dev, int cmd,
                                 int fr, int sc,
                                 long lba,
			         void* buffer,
                                 int numSect, int multiCnt );

extern int reg_pio_data_out( int dev, int cmd,
                             int fr, int sc,
                             unsigned int cyl, int head, int sect,
			     void* buffer,
                             int numSect, int multiCnt );

extern int reg_packet( int dev,
                       unsigned int cpbc,
		       void* cpbuf,
                       int dir,
                       long dpbc,
		       void* dpbuf );

//**************************************************************

// Public data in ATAIOTMR.C

extern long tmr_time_out;     // command time out in seconds

//**************************************************************

// Public functions in ATAIOTMR.C

extern long tmr_read_bios_timer( void );

extern void tmr_set_timeout( void );

extern int tmr_chk_timeout( void );

//**************************************************************

// Public functions in ATAIOTRC.C

// command history trace entry types used by reg_cmd_info.flg,
// trc_get_cmd_name(), etc.
#define TRC_FLAG_EMPTY 0
#define TRC_FLAG_SRST  1
#define TRC_FLAG_CMD   2
#define TRC_FLAG_CDB   3

// command types (protocol types) used by reg_cmd_info.ct,
// trc_cht_types(), etc.  this is a bit shift value.
#define TRC_TYPE_ALL    16    // trace all cmd types
#define TRC_TYPE_NONE    0    // don't trace any cmd types
#define TRC_TYPE_ADMAI   1    // ATA DMA In
#define TRC_TYPE_ADMAO   2    // ATA DMA Out
#define TRC_TYPE_AND     3    // ATA PIO Non-Data
#define TRC_TYPE_APDI    4    // ATA PIO Data In
#define TRC_TYPE_APDO    5    // ATA PIO Data Out
#define TRC_TYPE_ASR     6    // ATA Soft Reset
#define TRC_TYPE_PDMAI   7    // ATAPI DMA In
#define TRC_TYPE_PDMAO   8    // ATAPI DMA Out
#define TRC_TYPE_PND     9    // ATAPI PIO Non-Data
#define TRC_TYPE_PPDI   10    // ATAPI PIO Data In
#define TRC_TYPE_PPDO   11    // ATAPI PIO Data Out

extern unsigned char * trc_get_cmd_name( unsigned char flg, unsigned char cc );

extern unsigned char * trc_get_st_bit_name( unsigned char st );

extern unsigned char * trc_get_er_bit_name( unsigned char er );

extern unsigned char * trc_get_err_name( int ec );

extern void trc_err_dump1( void );
extern unsigned char * trc_err_dump2( void );

extern void trc_cht_types( int type );
extern void trc_cht_dump0( void );
extern void trc_cht_dump1( void );
extern unsigned char * trc_cht_dump2( void );

extern void trc_llt_dump0( void );
extern void trc_llt_dump1( void );
extern unsigned char * trc_llt_dump2( void );

#endif

// end ataio.h


