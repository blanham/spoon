#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <spoon/sysmsg.h>
#include <spoon/ipc/Messenger.h>

#include "defines.h"
#include "funcs.h"
#include "keymap.h"


#include "KBDDriver.h"

#define		MODIFIERS_CHANGED	0xA0010020

#define SHIFT_KEY		(1<<0)
//#define COMMAND_KEY  		(1<<1)	
#define CAPS_LOCK 		(1<<2) 
#define CONTROL_KEY 		(1<<3) 
#define MENU_KEY 		(1<<4) 
#define SCROLL_LOCK 		(1<<5) 
#define OPTION_KEY  		(1<<6) 
#define NUM_LOCK 		(1<<7) 


#define COMMAND_KEY  		CONTROL_KEY


#define LEFT_SHIFT_KEY 		(1<<16) 
#define RIGHT_SHIFT_KEY 	(1<<17) 
#define LEFT_CONTROL_KEY 	(1<<18) 
#define RIGHT_CONTROL_KEY 	(1<<19) 
#define LEFT_OPTION_KEY 	(1<<20) 
#define RIGHT_OPTION_KEY 	(1<<21) 
#define LEFT_COMMAND_KEY 	(1<<22) 
#define RIGHT_COMMAND_KEY 	(1<<23) 


// .............................
#define	BACKSPACE			  0x08
#define ENTER				  0x0a 
#define RETURN				  ENTER 
#define SPACE				  0x20 
#define TAB 	 	 	 	  0x09 
#define ESCAPE  			  0x1b 
#define LEFT_ARROW  			  0x1c 
#define RIGHT_ARROW  			  0x1d 
#define UP_ARROW  			  0x1e 
#define DOWN_ARROW  			  0x1f 
#define INSERT  			  0x05 
#undef DELETE
#define DELETE  			  0x7f 
#define HOME  				  0x01 
#define END  				  0x04 
#define PAGE_UP 			  0x0b 
#define PAGE_DOWN  			  0x0c 
#define FUNCTION_KEY  			  0x10 



// FUNCTION B_KEYS.............

#define F1_KEY				  0x2
#define F2_KEY				  0x3
#define F3_KEY				  0x4
#define F4_KEY				  0x5
#define F5_KEY				  0x6
#define F6_KEY				  0x7
#define F7_KEY				  0x8
#define F8_KEY				  0x9
#define F9_KEY				  0xA
#define F10_KEY				  0xB
#define F11_KEY				  0xC
#define F12_KEY				  0xD
#define PRINT_KEY			  0xE
#define SCROLL_KEY			  0xF
#define PAUSE_KEY			  0x22



#define UTF8_ELLIPSIS  			  "xE2x80xA6" 
#define UTF8_OPEN_QUOTE 		  "xE2x80x9C" 
#define UTF8_CLOSE_QUOTE  		  "xE2x80x9D" 
#define UTF8_COPYRIGHT  		  "xC2xA9" 
#define UTF8_REGISTERED  		  "xC2xAE" 
#define UTF8_TRADEMARK  		  "xE2x84xA2" 
#define UTF8_SMILING_FACE  		  "xE2x98xBB"  
#define UTF8_HIROSHI  			  "xE5xBCx98"  



#define SHIFTED	((modifiers & SHIFT_KEY) != 0) || ((leds & LED_CAPS) != 0) 


// -------------------------------------------------------------


KBDDriver::KBDDriver( char* proc_name )
:	Application( "KBDDriver" )
{
  leds = LED_NUM;
  modifiers = 0;
  extended = 0;

  if ( proc_name != NULL ) server_pid = smk_find_process_id( proc_name );
                     else  server_pid = smk_find_process_id( "input_server" );


  if ( server_pid <= 0 )
    printf("%s\n","kbd driver can't find the input_server." );
}

KBDDriver::~KBDDriver()
{
}

int32 KBDDriver::IRQ( int32 irq )
{
    uint8 c;
    uint32 ascii;
    uint32 what;
	int test;

	smk_disable_interrupts();
	test = inb(0x64) & 0x1;
	if ( test != 0 ) c = ReadChar();
	smk_enable_interrupts();
	
    if (test == 0 ) return -1;


	if ( c == EXTENDED )
	{
	  	extended = 1;
		return 0;
	}


    extended = 0;

    if ( c & 0x80 ) what = KEY_UP;
	   		  else  what = KEY_DOWN;

	c = c & 0x7f;

	if ( what == KEY_UP )
	{
	   	 switch (c)
		 {
		 	case CTRL: 
				modifiers &= ~ CONTROL_KEY; 
				SendMessage( MODIFIERS_CHANGED, 0, CONTROL_KEY, modifiers );
				break;
		 	case ALT:  
				modifiers &= ~ OPTION_KEY; 
				SendMessage( MODIFIERS_CHANGED, 0, OPTION_KEY, modifiers );
				break;

			case LSHIFT:
			case RSHIFT:  
			     modifiers &= ~ SHIFT_KEY;
				 break;

			default: 
				 if ( SHIFTED ) ascii = shifted_keymap[c];
				 		   else ascii = unshifted_keymap[c];

				 if ( ascii != 0 ) SendMessage( what,  ascii, c, modifiers );
				 break;
		 }
	    
	}
	else 
	{
	    switch(c)
	    {
	    	    case CTRL:    
					  modifiers |= CONTROL_KEY;
			   	  	  SendMessage( MODIFIERS_CHANGED, 0, CONTROL_KEY, modifiers );
		    		  break;
	    	    case ALT:     
					  modifiers |= OPTION_KEY;
			          SendMessage( MODIFIERS_CHANGED, 0, OPTION_KEY, modifiers );
		    		  break;
			    
			    case LSHIFT:
			    case RSHIFT:
			    	 modifiers |= SHIFT_KEY;
					 break;

			    case CAPS:
					 if ( leds & LED_CAPS ) leds &= ~LED_CAPS;
					 				   else leds |= LED_CAPS;
					 set_leds( leds );
					 break;

			    case SCRLOCK:
					 if ( leds & LED_SCROLL ) leds &= ~LED_SCROLL;
					 					 else leds |= LED_SCROLL;
					 set_leds( leds );
					 break;
			    case NUMLOCK:
					 if ( leds & LED_NUM ) leds &= ~LED_NUM;
					 				  else leds |= LED_NUM;
					 set_leds( leds );
					 break;
			    case F11:
					 smk_spit();
					 break;
			    case F12:
					 smk_reboot();
					 break;
			    default:
					 if ( SHIFTED ) ascii = shifted_keymap[c];
					 		   else ascii = unshifted_keymap[c];
	
					 if ( ascii != 0 ) SendMessage( what,  ascii, c, modifiers );
					 break;
			    
	  	}
	      
   }


   return 0; 
}



void KBDDriver::Shutdown()
{
   ReleaseIRQ( 1 );
}


int KBDDriver::Init()
{
    RequestIRQ( 1 );
	smk_disable_interrupts();
    while ((inb(0x64) & 0x1) != 0 ) { inb(0x60); }
	smk_enable_interrupts();
    return 0;
}

int KBDDriver::SendMessage(  uint32 what, uint32 unicode, uint32 scancode, uint32 modifiers )
{
    return Messenger::SendPulse( server_pid, 0, what, unicode, scancode, modifiers, 0, 0 );
}



