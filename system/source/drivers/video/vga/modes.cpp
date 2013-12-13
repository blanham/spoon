//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=-                                                                         -=
//=-                   Tauron VGA Utilities Version 3.0                      -=
//=-                      Released September 20, 1998                        -=
//=-                                                                         -=
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=- Copyright (c) 1997, 1998 by Jeff Morgan  =-= This code is FREE provided -=
//=- All Rights Reserved.                     =-= that you put my name some- -=
//=-                                          =-= where in your credits.     -=
//=- DISCLAIMER:                              =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=- I assume no responsibility whatsoever for any effect that this package, -=
//=- the information contained therein or the use thereof has on you, your   -=
//=- sanity, computer, spouse, children, pets or anything else related to    -=
//=- you or your existance. No warranty is provided nor implied with this    -=
//=- source code.                                                            -=
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "tauron.h"
#include "modes_c.inc"
#include "palette.inc"
#include <kernel.h>

void setpalette4();
void setpalette16();
void setpalette256();
Vmode Mode;

#define SEQ_ADDR 			0x03C4
#define GRACON_ADDR 			0x03CE
#define CRTC_ADDR 			0x03D4

void SetMode(unsigned char *regs)
{
   int i;
   unsigned int word;

   outb( MISC_ADDR, regs[0] );
   outb( STATUS_ADDR, regs[1] );


   for ( i = 0; i < 5; i++)
   {
     outb( SEQ_ADDR, i );
     outb( SEQ_ADDR + 1,  regs[2+i] );
   }


   // Clear Protection bits
   outw( CRTC_ADDR,  0xE011 );

   // Send CRTC regs
   for ( i = 0; i < 25; i++)
   {
     outb( CRTC_ADDR, i );
     outb( CRTC_ADDR + 1, regs[7+i] );
   }

   // Send GRAPHICS regs
   for ( i = 0; i < 9; i++)
   {
     outb( GRACON_ADDR, i );
     outb( GRACON_ADDR + 1,  regs[32+i] );
   }

   // read the status
   inb( STATUS_ADDR );

   // Send ATTRCON regs
   for ( i = 0; i < 21; i++)
   {
     word = inw( ATTRCON_ADDR );
     outb( ATTRCON_ADDR, i );
     outb( ATTRCON_ADDR, regs[41+i] );
   }

   outb( ATTRCON_ADDR, 0x20 );
}

void SetVideoMode(int mode)
{
   Mode.mode = mode;
   if (mode == MODE00H)                        // 40 x 25 x 16
   {
      SetMode(mode00h);
      setpalette16();

      Mode.width = 40;
      Mode.height = 25;
      Mode.width_bytes = 1000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE03H)                   // 80 x 25 x 16
   {
      SetMode(mode03h);
      setpalette16();

      Mode.width = 80;
      Mode.height = 25;
      Mode.width_bytes = 2000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE04H)                   // 320 x 200 x 4
   {
      SetMode(mode04h);
      setpalette4();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 8192;
      Mode.colors = 4;
      Mode.attrib = TVU_GRAPHICS;
   }
   else if (mode == MODE06H)                    // 640 x 200 x 2
   {
      SetMode(mode06h);

      Mode.width = 640;
      Mode.height = 200;
      Mode.width_bytes = 8192;
      Mode.colors = 2;
      Mode.attrib = TVU_GRAPHICS;
   }
   else if (mode == MODE07H)                    // 80 x 25 x 2
   {
      SetMode(mode07h);

      Mode.width = 80;
      Mode.height = 25;
      Mode.width_bytes = 2000;
      Mode.colors = 2;
      Mode.attrib = TVU_TEXT | TVU_MONOCHROME;
   }
   else if (mode == MODE0DH)                    // 320 x 200 x 16
   {
      SetMode(mode0Dh);
      setpalette16();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 8000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE0EH)                    // 640 x 200 x 16
   {
      SetMode(mode0Eh);
      setpalette16();

      Mode.width = 640;
      Mode.height = 200;
      Mode.width_bytes = 16000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE0FH)                    // 640 x 350 x 2
   {
      SetMode(mode0Fh);

      Mode.width = 640;
      Mode.height = 350;
      Mode.width_bytes = 28000;
      Mode.colors = 2;
      Mode.attrib = TVU_GRAPHICS | TVU_MONOCHROME;
   }
   else if (mode == MODE10H)                    // 640 x 350 x 16
   {
      SetMode(mode10h);
      setpalette16();

      Mode.width = 640;
      Mode.height = 350;
      Mode.width_bytes = 28000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE11H)                    // 640 x 480 x 2
   {
      SetMode(mode11h);

      Mode.width = 640;
      Mode.height = 480;
      Mode.width_bytes = 38400u;
      Mode.colors = 2;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE12H)                    // 640 x 480 x 16
   {
      SetMode(mode12h);
      setpalette16();

      Mode.width = 640;
      Mode.height = 480;
      Mode.width_bytes = 38400u;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE13H)                    // 320 x 200 x 256
   {
      SetMode(mode13h);
      setpalette256();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 64000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS;
   }
   else if (mode == CHAIN4)                     // unchained 320 x 200 x 256
   {
      SetMode(modeC4);
      setpalette256();

      Mode.width = 320;
      Mode.height = 200;
      Mode.width_bytes = 16000;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_X)                     // unchained 320 x 240 x 256
   {
      SetMode(modeC4);

      outb(MISC_ADDR,0xE3);
      // turn off write protect
      outw(CRTC_ADDR,0x2C11);
      // vertical total
      outw(CRTC_ADDR,0x0D06);
      // overflow register
      outw(CRTC_ADDR,0x3E07);
      // vertical retrace start
      outw(CRTC_ADDR,0xEA10);
      // vertical retrace end AND wr.prot
      outw(CRTC_ADDR,0xAC11);
      // vertical display enable end
      outw(CRTC_ADDR,0xDF12);
      // start vertical blanking
      outw(CRTC_ADDR,0xE715);
      // end vertical blanking
      outw(CRTC_ADDR,0x0616);

      setpalette256();
      Mode.width = 320;
      Mode.height = 240;
      Mode.width_bytes = 19200;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_A)                     // unchained 320 x 350 x 256
   {
      SetMode(modeC4);

      // turn off double scanning mode
      outb(CRTC_ADDR,9);
      outb(CRTC_ADDR+1,inb(CRTC_ADDR+1) & ~0x1F);
      // change the vertical resolution flags to 350
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0x80);
      // turn off write protect
      outw(CRTC_ADDR,0x2C11);
      // vertical total
      outw(CRTC_ADDR,0xBF06);
      // overflow register
      outw(CRTC_ADDR,0x1F07);
      // vertical retrace start
      outw(CRTC_ADDR,0x8310);
      // vertical retrace end AND wr.prot
      outw(CRTC_ADDR,0x8511);
      // vertical display enable end
      outw(CRTC_ADDR,0x5D12);
      // start vertical blanking
      outw(CRTC_ADDR,0x6315);
      // end vertical blanking
      outw(CRTC_ADDR,0xBA16);

      setpalette256();
      Mode.width = 320;
      Mode.height = 350;
      Mode.width_bytes = 28000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_B)                     // unchained 320 x 400 x 256
   {
      SetMode(modeC4);
      // turn off double scanning mode
      outb(CRTC_ADDR,9);
      outb(CRTC_ADDR+1,inb(CRTC_ADDR+1) & ~0x1F);
      // change the vertical resolution flags to 400
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0x40);

      setpalette256();
      Mode.width = 320;
      Mode.height = 400;
      Mode.width_bytes = 32000;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_C)                     // unchained 320 x 480 x 256
   {
      SetMode(modeC4);

      // turn off double scanning mode
      outb(CRTC_ADDR,9);
      outb(CRTC_ADDR+1,inb(CRTC_ADDR+1) & ~0x1F);
      // change the vertical resolution flags to 480
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0xC0);
      // turn off write protect
      outw(CRTC_ADDR,0x2C11);
      // vertical total
      outw(CRTC_ADDR,0x0D06);
      // overflow register
      outw(CRTC_ADDR,0x3E07);
      // vertical retrace start
      outw(CRTC_ADDR,0xEA10);
      // vertical retrace end AND wr.prot
      outw(CRTC_ADDR,0xAC11);
      // vertical display enable end
      outw(CRTC_ADDR,0xDF12);
      // start vertical blanking
      outw(CRTC_ADDR,0xE715);
      // end vertical blanking
      outw(CRTC_ADDR,0x0616);

      setpalette256();
      Mode.width = 320;
      Mode.height = 480;
      Mode.width_bytes = 38400u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_D)                     // unchained 360 x 200 x 256
   {
      SetMode(mode13h);

      // Turn off Chain 4
      outw(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outw(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outw(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 400
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0x40);

      // turn off write protect
      outw(CRTC_ADDR,0x2C11);

      outw(CRTC_ADDR,0x6B00);
      outw(CRTC_ADDR,0x5901);
      outw(CRTC_ADDR,0x5A02);
      outw(CRTC_ADDR,0x8E03);
      outw(CRTC_ADDR,0x5E04);
      outw(CRTC_ADDR,0x8A05);
      outw(CRTC_ADDR,0x0008);
      outw(CRTC_ADDR,0xC009);
      outw(CRTC_ADDR,0x000A);
      outw(CRTC_ADDR,0x000B);
      outw(CRTC_ADDR,0x000C);
      outw(CRTC_ADDR,0x000D);
      outw(CRTC_ADDR,0x000E);
      outw(CRTC_ADDR,0x000F);
      outw(CRTC_ADDR,0xAC11);
      outw(CRTC_ADDR,0x2D13);
      outw(CRTC_ADDR,0x0014);
      outw(CRTC_ADDR,0xE317);
      outw(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 200;
      Mode.width_bytes = 18000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_E)                     // unchained 360 x 240 x 256
   {
      SetMode(mode13h);

      // Turn off Chain 4
      outw(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outw(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outw(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 480
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0xC0);

      // turn off write protect
      outw(CRTC_ADDR,0x2C11);

      outw(CRTC_ADDR,0x6B00);
      outw(CRTC_ADDR,0x5901);
      outw(CRTC_ADDR,0x5A02);
      outw(CRTC_ADDR,0x8E03);
      outw(CRTC_ADDR,0x5E04);
      outw(CRTC_ADDR,0x8A05);
      outw(CRTC_ADDR,0x0D06);
      outw(CRTC_ADDR,0x3E07);
      outw(CRTC_ADDR,0x0008);
      outw(CRTC_ADDR,0xC009);
      outw(CRTC_ADDR,0x000A);
      outw(CRTC_ADDR,0x000B);
      outw(CRTC_ADDR,0x000C);
      outw(CRTC_ADDR,0x000D);
      outw(CRTC_ADDR,0x000E);
      outw(CRTC_ADDR,0x000F);
      outw(CRTC_ADDR,0xEA10);
      outw(CRTC_ADDR,0xAC11);
      outw(CRTC_ADDR,0xDF12);
      outw(CRTC_ADDR,0x2D13);
      outw(CRTC_ADDR,0x0014);
      outw(CRTC_ADDR,0xE715);
      outw(CRTC_ADDR,0x0616);
      outw(CRTC_ADDR,0xE317);
      outw(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 240;
      Mode.width_bytes = 21600;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_F)                     // unchained 360 x 350 x 256
   {
      SetMode(mode13h);

      // Turn off Chain 4
      outw(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outw(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outw(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 350
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0x80);

      // turn off write protect
      outw(CRTC_ADDR,0x2C11);

      outw(CRTC_ADDR,0x6B00);
      outw(CRTC_ADDR,0x5901);
      outw(CRTC_ADDR,0x5A02);
      outw(CRTC_ADDR,0x8E03);
      outw(CRTC_ADDR,0x5E04);
      outw(CRTC_ADDR,0x8A05);
      outw(CRTC_ADDR,0xBF06);
      outw(CRTC_ADDR,0x1F07);
      outw(CRTC_ADDR,0x0008);
      outw(CRTC_ADDR,0x4009);
      outw(CRTC_ADDR,0x000A);
      outw(CRTC_ADDR,0x000B);
      outw(CRTC_ADDR,0x000C);
      outw(CRTC_ADDR,0x000D);
      outw(CRTC_ADDR,0x000E);
      outw(CRTC_ADDR,0x000F);
      outw(CRTC_ADDR,0x8310);
      outw(CRTC_ADDR,0x8511);
      outw(CRTC_ADDR,0x5D12);
      outw(CRTC_ADDR,0x2D13);
      outw(CRTC_ADDR,0x0014);
      outw(CRTC_ADDR,0x6315);
      outw(CRTC_ADDR,0xBA16);
      outw(CRTC_ADDR,0xE317);
      outw(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 350;
      Mode.width_bytes = 31500;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_G)                     // unchained 360 x 400 x 256
   {
      SetMode(mode13h);

      // Turn off Chain 4
      outw(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outw(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outw(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 400
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0x40);

      // turn off write protect
      outw(CRTC_ADDR,0x2C11);

      outw(CRTC_ADDR,0x6B00);
      outw(CRTC_ADDR,0x5901);
      outw(CRTC_ADDR,0x5A02);
      outw(CRTC_ADDR,0x8E03);
      outw(CRTC_ADDR,0x5E04);
      outw(CRTC_ADDR,0x8A05);
      outw(CRTC_ADDR,0x0008);
      outw(CRTC_ADDR,0x4009);
      outw(CRTC_ADDR,0x000A);
      outw(CRTC_ADDR,0x000B);
      outw(CRTC_ADDR,0x000C);
      outw(CRTC_ADDR,0x000D);
      outw(CRTC_ADDR,0x000E);
      outw(CRTC_ADDR,0x000F);
      outw(CRTC_ADDR,0xAC11);
      outw(CRTC_ADDR,0x2D13);
      outw(CRTC_ADDR,0x0014);
      outw(CRTC_ADDR,0xE317);
      outw(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 400;
      Mode.width_bytes = 36000u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_H)                     // unchained 360 x 480 x 256
   {
      SetMode(mode13h);

      // Turn off Chain 4
      outw(SEQ_ADDR,0x0604);
      // Activate a synchronous reset
      outw(SEQ_ADDR,0x0100);
      // Select 28 mhz pixel clock
      outb(MISC_ADDR,0xE7);
      // Release synchronous reset
      outw(SEQ_ADDR,0x0300);

      // change the vertical resolution flags to 480
      outb(MISC_ADDR,(inb(0x3CC) & ~0xC0) | 0xC0);

      // turn off write protect
      outw(CRTC_ADDR,0x2C11);

      outw(CRTC_ADDR,0x6B00);
      outw(CRTC_ADDR,0x5901);
      outw(CRTC_ADDR,0x5A02);
      outw(CRTC_ADDR,0x8E03);
      outw(CRTC_ADDR,0x5E04);
      outw(CRTC_ADDR,0x8A05);
      outw(CRTC_ADDR,0x0D06);
      outw(CRTC_ADDR,0x3E07);
      outw(CRTC_ADDR,0x0008);
      outw(CRTC_ADDR,0x4009);
      outw(CRTC_ADDR,0x000A);
      outw(CRTC_ADDR,0x000B);
      outw(CRTC_ADDR,0x000C);
      outw(CRTC_ADDR,0x000D);
      outw(CRTC_ADDR,0x000E);
      outw(CRTC_ADDR,0x000F);
      outw(CRTC_ADDR,0xEA10);
      outw(CRTC_ADDR,0xAC11);
      outw(CRTC_ADDR,0xDF12);
      outw(CRTC_ADDR,0x2D13);
      outw(CRTC_ADDR,0x0014);
      outw(CRTC_ADDR,0xE715);
      outw(CRTC_ADDR,0x0616);
      outw(CRTC_ADDR,0xE317);
      outw(CRTC_ADDR,0xFF18);

      setpalette256();
      Mode.width = 360;
      Mode.height = 480;
      Mode.width_bytes = 43200u;
      Mode.colors = 256;
      Mode.attrib = TVU_GRAPHICS | TVU_UNCHAINED;
   }
   else if (mode == MODE_I)                     // 640 x 400 x 16
   {
      SetMode(mode10h);
      outb( 0x3C2, ((inb(0x3CC) & 0x3F) | 0x40));
      outw( CRTC_ADDR, 0x9C10 );
      outw( CRTC_ADDR, 0x8311 );
      outw( CRTC_ADDR, 0x8F12 );
      outw( CRTC_ADDR, 0x9615 );
      outw( CRTC_ADDR, 0xB916 );
      setpalette16();
      Mode.width = 640;
      Mode.height = 400;
      Mode.width_bytes = 32000;
      Mode.colors = 16;
      Mode.attrib = TVU_GRAPHICS | TVU_PLANAR;
   }
   else if (mode == MODE_J)                    // 80 x 43 x 16
   {
      SetMode(modeJ);

      Mode.width = 80;
      Mode.height = 43;
      Mode.width_bytes = 3440;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE_K)                    // 80 x 50 x 16
   {
      SetMode(modeK);

      Mode.width = 80;
      Mode.height = 50;
      Mode.width_bytes = 4000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE_L)                    // 40 x 43 x 16
   {
      SetMode(modeL);

      Mode.width = 40;
      Mode.height = 43;
      Mode.width_bytes = 4000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
   else if (mode == MODE_M)                    // 40 x 50 x 16
   {
      SetMode(modeM);

      Mode.width = 40;
      Mode.height = 50;
      Mode.width_bytes = 4000;
      Mode.colors = 16;
      Mode.attrib = TVU_TEXT;
   }
}

void setpal(int color, char r, char g, char b)
{

   outb( 0x3C8, (color & 0xFF) );
   outb( 0x3C9,    ( r & 0xFF) );
   outb( 0x3C9,    ( g & 0xFF) );
   outb( 0x3C9,    ( b & 0xFF) );
}

void setpalette4()
{
   setpal( 0,  0,  0,  0);
   setpal( 1,  0, 42, 42);
   setpal( 2, 42,  0, 42);
   setpal( 3, 63, 63, 63);
}

void setpalette16()
{
   int j = 0;
   for (int i = 0; i < 48; i+=3)
   {
      setpal(j, Pal[i], Pal[i+1], Pal[i+2]);
      j++;
   }
}

void setpalette256()
{
   int j = 0;
   for (int i = 0; i < 768; i+=3)
   {
      setpal(j, Pal[i], Pal[i+1], Pal[i+2]);
      j++;
   }
}
