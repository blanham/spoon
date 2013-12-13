#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Messenger.h>

#include "PS2Driver.h"


// ************************************************************

#define MOUSE_INTERRUPT_NO (0x74)
#define KEYB_PORT (0x60)
#define KEYB_CTRL (0x64)
#define KEYB_ACK (0xfa)

#define AUX_OBUF_FULL	0x21		/* output buffer (from device) full */

// find wheel
unsigned char ps2_init_string[] = { 0xf3, 0xc8, 0xf3, 0x64, 0xf3, 0x50, 0x00 };
// find buttons
unsigned char ps2_init_string2[] = { 0xf3, 0xc8, 0xf3, 0xc8, 0xf3, 0x50, 0x00 };
// real init
unsigned char ps2_init_string3[] = { 0xf6, 0xe6, 0xf4, 0xf3, 0x64, 0xe8, 0x03, 0x00 };
// mouse type
unsigned char extwheel = 0xff, extbuttons = 0xff;

int mx = 0, my = 0, mz = 0;

unsigned int buttons[5] = {0, 0, 0, 0, 0};
unsigned int prev_buttons = 0;

void kbd_command_send(unsigned int port, unsigned char val)
{
 unsigned long timeout;
 unsigned char stat;

 for (timeout = 500000l; timeout != 0; timeout--)
 {
	stat = inb(KEYB_CTRL);
	if ((stat & 0x02) == 0) break;
 }

 if (timeout != 0) outb(port, val);
 else printf("didn't send!\n");
}

unsigned char kbd_command_read()
{
 unsigned long timeout;
 unsigned char stat, data;

 for (timeout = 500000l; timeout != 0; timeout--)
 {
	stat = inb(KEYB_CTRL);
	if ((stat & 0x01) != 0)
	{
	 data = inb(KEYB_PORT);
	 if ((stat & 0xC0) == 0) return data;
	}
 }
 printf("didn't read!\n");
 return 0xFF;
}

unsigned char kbd_command_send_ack(unsigned int port, unsigned char val)
{
 unsigned int ret_val = 0;

 kbd_command_send(port, val);
 ret_val = kbd_command_read();

 if (ret_val != KEYB_ACK)
	printf("[keyboard] error: expected %x, got %x\n", val, ret_val);
 return ret_val;
}



// ************************************************************



PS2Driver::PS2Driver( int mode, char* proc_name )
:	Application( "PS2Driver" )
{
  m_mode = mode;
 
  if ( proc_name != NULL ) server_pid = smk_find_process_id( proc_name );
  					 else  server_pid = smk_find_process_id( "input_server" );

  if ( server_pid <= 0 )
    printf("%s\n","ps2mouse driver can't find the input_server." );
}

PS2Driver::~PS2Driver()
{
}

// --------------------------------------

int PS2Driver::IRQ_0( int irq )
{
 static unsigned char buf[4] = {0, 0, 0, 0};
 int dx, dy, dz;

 smk_disable_interrupts();

	 kbd_command_send(KEYB_CTRL, 0xad); // disable keyb
	
	 buf[0] = kbd_command_read();
	 buf[1] = kbd_command_read();
	 buf[2] = kbd_command_read();
	 buf[3] = 0;
	
	 if ((extwheel == 0x03) || (extbuttons == 0x04)) 
	 {
		buf[3] = kbd_command_read();
	 }
	
	 buttons[0] = buf[0] & 1;
	 buttons[1] = (buf[0] & 2) >> 1;
	 buttons[2] = (buf[0] & 4) >> 2;
	 buttons[3] = (buf[3] & 0x10) >> 4;
	 buttons[4] = (buf[3] & 0x20) >> 5;
	 dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
	 dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];
	 dz = (buf[3] & 0x08) ? (buf[3]&7) - 8 : buf[3]&7;
	
	 if (dx > 5 || dx < -5)
		dx *= 3;
	 if (dy > 5 || dy < -5)
		dy *= 3;
	
	 mx += dx;
	 my += dy;
	 mz += dz;
	
	 if (mx > 1023) mx = 1023;
	 if (mx < 0) mx = 0;
	 if (my > 767) my = 767;
	 if (my < 0) my = 0;
	
	 kbd_command_send(KEYB_CTRL, 0xae); // enable keyb

 smk_enable_interrupts();


 	// buttons! Switch them around so that left = bit 0, right = bit 1.
	unsigned int butts = 0;				// see more?
	for ( int  i = 0; i < 5; i++ )
		butts = butts + (buttons[i] << i);

 	if ( ( dx != 0 ) || ( dy != 0 ) || ( dz != 0 ) )
	{
		Messenger::SendPulse( server_pid, 0, MOUSE_MOVED, mx, my, butts ); 
	}
 
// ---------  MOUSE BUTTON DIFFERENCES -----------------------
       unsigned int tb = (butts ^ prev_buttons);

       if ( tb != 0 )
       {
	  	  if ( butts != 0 ) 
			Messenger::SendPulse( server_pid, 0, MOUSE_DOWN, (uint32)( mx ),  (uint32)( my ), butts );
		  else 
			Messenger::SendPulse( server_pid, 0, MOUSE_UP, (uint32)( mx ),  (uint32)( my ), butts );
		}
// -----------------------------------------------------------

	prev_buttons = butts;

 
	
 return 0;

}


int PS2Driver::IRQ_1( int irq )
{
 static unsigned char buf[4] = {0, 0, 0, 0};
 static int bytes = 0;
 int dx, dy, dz;

 smk_disable_interrupts();

 	int needed = 0;
	if ((extwheel == 0x03) || (extbuttons == 0x04)) needed = 1;


    if ( (inb(KEYB_CTRL) & AUX_OBUF_FULL) != AUX_OBUF_FULL ) 
	{
		smk_enable_interrupts();
		printf("%s\n","IRQ yet buffer empty?!");
		return 0;
	}
	
	buf[ bytes++ ] = inb(KEYB_PORT);

  smk_enable_interrupts();

  if ( bytes < (3 + needed) ) return 0;

  bytes = 0;

	
	 buttons[0] = buf[0] & 1;
	 buttons[1] = (buf[0] & 2) >> 1;
	 buttons[2] = (buf[0] & 4) >> 2;
	 buttons[3] = (buf[3] & 0x10) >> 4;
	 buttons[4] = (buf[3] & 0x20) >> 5;
	 dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
	 dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];
	 dz = (buf[3] & 0x08) ? (buf[3]&7) - 8 : buf[3]&7;
	
	 if (dx > 5 || dx < -5)
		dx *= 3;
	 if (dy > 5 || dy < -5)
		dy *= 3;
	
	 mx += dx;
	 my += dy;
	 mz += dz;
	
	 if (mx > 1023) mx = 1023;
	 if (mx < 0) mx = 0;
	 if (my > 767) my = 767;
	 if (my < 0) my = 0;
	

 	// buttons! Switch them around so that left = bit 0, right = bit 1.
	unsigned int butts = 0;				// see more?
	for ( int  i = 0; i < 5; i++ )
		butts = butts + (buttons[i] << i);

 	if ( ( dx != 0 ) || ( dy != 0 ) || ( dz != 0 ) )
	{
		Messenger::SendPulse( server_pid, 0,  MOUSE_MOVED, mx, my, butts ); 
	}
 
// ---------  MOUSE BUTTON DIFFERENCES -----------------------
       unsigned int tb = (butts ^ prev_buttons);

       if ( tb != 0 )
       {
	  	  if ( butts != 0 ) 
			Messenger::SendPulse( server_pid, 0,  MOUSE_DOWN, (uint32)( mx ),  (uint32)( my ), butts );
		  else 
			Messenger::SendPulse( server_pid, 0,  MOUSE_UP, (uint32)( mx ),  (uint32)( my ), butts );
		}
// -----------------------------------------------------------

	prev_buttons = butts;

 
	
 return 0;
}


int PS2Driver::IRQ_2( int irq )
{
 static unsigned char buf[4] = {0, 0, 0, 0};
 int dx, dy, dz;

 smk_disable_interrupts();

	
	 buf[0] = kbd_command_read();
	 buf[1] = kbd_command_read();
	 buf[2] = kbd_command_read();
	 buf[3] = 0;
	
	 if ((extwheel == 0x03) || (extbuttons == 0x04)) 
	 {
		buf[3] = kbd_command_read();
	 }
	
	 buttons[0] = buf[0] & 1;
	 buttons[1] = (buf[0] & 2) >> 1;
	 buttons[2] = (buf[0] & 4) >> 2;
	 buttons[3] = (buf[3] & 0x10) >> 4;
	 buttons[4] = (buf[3] & 0x20) >> 5;
	 dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
	 dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];
	 dz = (buf[3] & 0x08) ? (buf[3]&7) - 8 : buf[3]&7;
	
	 if (dx > 5 || dx < -5)
		dx *= 3;
	 if (dy > 5 || dy < -5)
		dy *= 3;
	
	 mx += dx;
	 my += dy;
	 mz += dz;
	
	 if (mx > 1023) mx = 1023;
	 if (mx < 0) mx = 0;
	 if (my > 767) my = 767;
	 if (my < 0) my = 0;

 smk_enable_interrupts();


 	// buttons! Switch them around so that left = bit 0, right = bit 1.
	unsigned int butts = 0;				// see more?
	for ( int  i = 0; i < 5; i++ )
		butts = butts + (buttons[i] << i);

 	if ( ( dx != 0 ) || ( dy != 0 ) || ( dz != 0 ) )
	{
		Messenger::SendPulse( server_pid, 0,  MOUSE_MOVED, mx, my, butts ); 
	}
 
// ---------  MOUSE BUTTON DIFFERENCES -----------------------
       unsigned int tb = (butts ^ prev_buttons);

       if ( tb != 0 )
       {
	  	  if ( butts != 0 ) 
			Messenger::SendPulse( server_pid, 0,  MOUSE_DOWN, (uint32)( mx ),  (uint32)( my ), butts );
		  else 
			Messenger::SendPulse( server_pid, 0,  MOUSE_UP, (uint32)( mx ),  (uint32)( my ), butts );
		}
// -----------------------------------------------------------

	prev_buttons = butts;

 
	
 return 0;

}


int PS2Driver::IRQ_3( int irq )
{
 static unsigned char buf[4] = {0, 0, 0, 0};
 static int bytes = 0;
 int dx, dy, dz;

 smk_disable_interrupts();

	 kbd_command_send(KEYB_CTRL, 0xad); // disable keyb
	
	 buf[ bytes++ ] = kbd_command_read();
	
 	 int needed = 0;
	 if ((extwheel == 0x03) || (extbuttons == 0x04)) needed = 1;

	  if ( bytes < (3 + needed) )
	  {
  		smk_enable_interrupts();
		return 0;
	  }

	 bytes = 0;
	

	 buttons[0] = buf[0] & 1;
	 buttons[1] = (buf[0] & 2) >> 1;
	 buttons[2] = (buf[0] & 4) >> 2;
	 buttons[3] = (buf[3] & 0x10) >> 4;
	 buttons[4] = (buf[3] & 0x20) >> 5;
	 dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
	 dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];
	 dz = (buf[3] & 0x08) ? (buf[3]&7) - 8 : buf[3]&7;
	
	 if (dx > 5 || dx < -5)
		dx *= 3;
	 if (dy > 5 || dy < -5)
		dy *= 3;
	
	 mx += dx;
	 my += dy;
	 mz += dz;
	
	 if (mx > 1023) mx = 1023;
	 if (mx < 0) mx = 0;
	 if (my > 767) my = 767;
	 if (my < 0) my = 0;
	
	 kbd_command_send(KEYB_CTRL, 0xae); // enable keyb

 smk_enable_interrupts();


 	// buttons! Switch them around so that left = bit 0, right = bit 1.
	unsigned int butts = 0;				// see more?
	for ( int  i = 0; i < 5; i++ )
		butts = butts + (buttons[i] << i);

 	if ( ( dx != 0 ) || ( dy != 0 ) || ( dz != 0 ) )
	{
		Messenger::SendPulse( server_pid, 0,  MOUSE_MOVED, mx, my, butts ); 
	}
 
// ---------  MOUSE BUTTON DIFFERENCES -----------------------
       unsigned int tb = (butts ^ prev_buttons);

       if ( tb != 0 )
       {
	  	  if ( butts != 0 ) 
			Messenger::SendPulse( server_pid, 0,  MOUSE_DOWN, (uint32)( mx ),  (uint32)( my ), butts );
		  else 
			Messenger::SendPulse( server_pid, 0,  MOUSE_UP, (uint32)( mx ),  (uint32)( my ), butts );
		}
// -----------------------------------------------------------

	prev_buttons = butts;

 
	
 return 0;

}




// --------------------------------------

int PS2Driver::IRQ( int irq )
{
	switch (m_mode)
	{
		case 0: return IRQ_0( irq );
		case 1: return IRQ_1( irq );
		case 2: return IRQ_2( irq );
		case 3: return IRQ_3( irq );
	}

	return -1;
}





void PS2Driver::Shutdown()
{
   
	ReleaseIRQ( 12 );
	
   	smk_disable_interrupts();
   
	 kbd_command_send(KEYB_CTRL, 0xd4); // write to aux
	 kbd_command_send(KEYB_PORT, 0xf5); // disable aux
	 printf("disable response: %x\n", kbd_command_read());
	
	 kbd_command_send(KEYB_CTRL, 0xd4); // write to aux
	 kbd_command_send(KEYB_PORT, 0xff); // reset mouse
	 printf("reset response: %x\n", kbd_command_read());

	smk_enable_interrupts();
}



int PS2Driver::Init()
{
    RequestIRQ( 12 );
    
    mx = 400;
    my = 300;
	mz = 0;


	 unsigned char* ch = 0;

	 smk_disable_interrupts();

		//Let's enable mouse interrupt on the Kbd&Mouse chip
		// That is on the i8042 chip,
		// not on Programmable Interrupt Controller (PIC)
		// We need to set to 1 bit 1 (mask = 2) of
		// the control register
		// Many OS prefer to do just (not just because shorter):
		//kbd_command_send(KEYB_CTRL,0x60);//Write Control register
		//kbd_command_send(KEYB_PORT,0x47);//Sane value for it
		//But I prefer:
		kbd_command_send(KEYB_CTRL, 0x20); 						// Read Control Reg
		unsigned char control_register = kbd_command_read();
		control_register |= 2; 									// Enable Mouse interrupt bit

		printf("will write %x to Control Register of i8042\n",control_register);

		kbd_command_send(KEYB_CTRL,0x60);//Will write CtrlReg
		kbd_command_send(KEYB_PORT, control_register); // do it


	 
		 kbd_command_send(KEYB_CTRL, 0xa8); // enable aux
	
		 kbd_command_send(KEYB_CTRL, 0xa9); // check aux interface
		 printf("interface response: %x\n", kbd_command_read());
	
		 kbd_command_send(KEYB_CTRL, 0xd4); // write to aux
		 kbd_command_send_ack(KEYB_PORT, 0xf2); // identify mouse
		 printf("id response: %x\n", kbd_command_read());
		
		 for (ch = ps2_init_string; *ch != '\0'; ch++) {
			kbd_command_send(KEYB_CTRL, 0xd4);
			kbd_command_send_ack(KEYB_PORT, *ch);
		 }
		
		 kbd_command_send(KEYB_CTRL, 0xd4); // write to aux
		 kbd_command_send_ack(KEYB_PORT, 0xf2); // identify mouse
		 printf("wheel mouse was found: %i\n", (extwheel = kbd_command_read())==0x03);
		
		 for (ch = ps2_init_string2; *ch != '\0'; ch++) {
			kbd_command_send(KEYB_CTRL, 0xd4);
			kbd_command_send_ack(KEYB_PORT, *ch);
		 }
		
		 kbd_command_send(KEYB_CTRL, 0xd4); // write to aux
		 kbd_command_send_ack(KEYB_PORT, 0xf2); // identify mouse
		 printf(">3 buttons mouse was found: %i\n", (extbuttons = kbd_command_read())==0x04);
		
		 for (ch = ps2_init_string3; *ch != '\0'; ch++) {
			kbd_command_send(KEYB_CTRL, 0xd4);
			kbd_command_send_ack(KEYB_PORT, *ch);
		 }
		
		 kbd_command_send(KEYB_CTRL, 0xd4); // write to aux
		 kbd_command_send(KEYB_PORT, 0xf4); // enable mouse
		 printf("enable response: %x\n", kbd_command_read());
		
		 printf("extwheel = %i, extbuttons = %i\n", extwheel, extbuttons);
		

    smk_enable_interrupts();
	 
	
    return 0;
}

