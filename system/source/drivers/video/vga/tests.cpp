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
#include <kernel.h>
#include <stdio.h>
#include "tauron.h"

#define ABS(a)   ((a < 0) ? -a : a)
#define SGN(a)   ((a < 0) ? -1 : 1)

void Pixel13H(int x, int y, char color)
{
   char *screen;

   screen = (char*)(0x10000000);
   screen[ y * 320 + x ] = color;
}

void pixel(int x, int y, char color)
{
   char *screen;
   int width = Mode.width;
   unsigned int position;
   unsigned int something;
   unsigned char tmp;

   screen = (char*)(0x10000000);
   
   if (Mode.mode == MODE13H)
      Pixel13H(x,y,color);
   else if (Mode.attrib & TVU_UNCHAINED) 
   {
	outb( 0x3C4, 2 );
	outb( 0x3C5,  (1 << (x & 0x3))  );
        screen[  ( x / 4 ) + ( y * (width / 4) ) ] = (color & 0xFF);
   }
   else if (Mode.attrib & TVU_PLANAR)
   {

      position = (x >> 3) + (y * 80);
      something = (1 << (((x & 0xFF) & 0x7) ^ 0x7));
      outw( 0x3CE,  ((something << 8) + 0x8) );
      tmp = screen[ position ];
      screen[ position ]  = 0;
      outw( 0x3C4,  ( ((color & 0xFF) << 8) + 0x2 ));
      screen[ 0x3C4 ] = 0xFF;
      outw( 0x3C4, 0xF02 );
      outw( 0x3CE, 0xFF08 );
   }
}

// This is Bresenham's Line Drawing Algorithm
void drawline(int x1, int y1, int x2, int y2, char col)
{
   int d, x, y, ax, ay, sx, sy, dx, dy;

   dx = x2-x1;
   ax = ABS(dx) << 1;
   sx = SGN(dx);
   dy = y2-y1;
   ay = ABS(dy) << 1;
   sy = SGN(dy);

   x = x1;
   y = y1;
   if( ax > ay )
   {
      d = ay - (ax >> 1);
      while( x != x2 )
      {
	      pixel( x, y, col );
  	      if( d >= 0 )
	      {
	         y += sy;
	         d -= ax;
	      }
	      x += sx;
	      d += ay;
      }
   }
   else
   {
      d = ax - (ay >> 1);
      while( y != y2 )
      {
	      pixel( x, y, col );
	      if( d >= 0 )
	      {
	         x += sx;
	         d -= ay;
	      }
	      y += sy;
	      d += ax;
      }
   }
   return;
}

void drawrect(int x1, int y1, int x2, int y2, char color)
{
   drawline(x1,y1,x2,y1,color);
   drawline(x1,y2,x2,y2,color);
   drawline(x1,y1,x1,y2,color);
   drawline(x2,y1,x2,y2+1,color);
}

void hline(int x1, int x2, int y, char color)
{
   drawline(x1,y,x2,y,color);
}

void vline(int y1, int y2, int x, char color)
{
   drawline(x,y1,x,y2,color);
}

void fillrect(int x1, int y1, int x2, int y2, char color)
{
   for (int i = y1; i < y2; i++)
      hline(x1,x2,i,color);
}

void ModeTest()
{
   if (Mode.mode == MODE13H)
   {
      Clear13H(0);
   }
   else if (Mode.attrib & TVU_PLANAR)
   {
      PlanarClear(0);
   }
   else if (Mode.attrib & TVU_UNCHAINED)
   {
      UnchainedClear(0);
   }

   /* Print mode stats
    *
    */

   drawrect(0,0,Mode.width-1,Mode.height-1,15);
   for (int i = 50; i < Mode.height; i += 100)
   {
      hline(0,10,i,15);
      hline(0,25,i+50,15);
   }

   for (int i = 40; i < Mode.width; i += 80)
   {
      vline(0,15,i,15);
      vline(0,30,i+40,15);
   }

   if (Mode.colors == 2)
   {
      int x,y;
      x = Mode.width - 420;
      y = Mode.height - 220;
      fillrect(x,y,x+200,y+200,0);
      x+=200;
      fillrect(x,y,x+200,y+200,15);
      drawrect(x-200,y,x+200,y+200,15);
   }
   else if (Mode.colors == 16)
   {
      int x,y;
      x = Mode.width - 522;
      y = Mode.height - 135;
      for (int i = 0; i < 8; i++)
      {
         fillrect(x,y,x+64,y+64,i);
         x+=64;
      }
      x = Mode.width - 522;
      y += 64;
      for (int i = 8; i < 16; i++)
      {
         fillrect(x,y,x+64,y+64,i);
         x+=64;
      }
      drawrect(x-512,y-64,x,y+64,15);
   }
   else if (Mode.colors == 256)
   {
      int x,y,k;
      x = Mode.width - 165;
      y = Mode.height - 165;
      k = 0;
      for (int i = 0; i < 16; i++)
      {
         for (int j = 0; j < 16; j++)
         {
            fillrect(x,y,x+10,y+10,k);
            x+=10;
            k++;
         }
         x = Mode.width - 165;
         y += 10;
      }
      drawrect(Mode.width - 165,Mode.height - 165,Mode.width -5,Mode.height - 5,15);
   }
}

void TextTest()
{
   TextClear(0x1F);
   smk_conio_gotoxy(1,1);
   if (Mode.mode == MODE00H)
   {
      // ** NOTE ** the smk_conio_gotoxy's here are a hack so that i could use the
      // standard C functions to program this mode.  Normally you would just
      // calculate the proper address and display the text but since DOS still
      // thinks we are in mode 03h (it checks the BIOS), this is necessary.
      smk_conio_gotoxy(1,1);
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      smk_conio_gotoxy(41,1);
      printf("³Text Mode: 00H        ³\n");
      smk_conio_gotoxy(1,2);
      printf("³Width: 40   Height: 25³\n");
      smk_conio_gotoxy(41,2);
      printf("³Bytes per screen: 2000³\n");
      smk_conio_gotoxy(1,3);
      printf("³Number of pages: 8    ³\n");
      smk_conio_gotoxy(41,3);
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE03H)
   {
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      printf("³Text Mode: 03H        ³\n");
      printf("³Width: 80   Height: 25³\n");
      printf("³Bytes per screen: 4000³\n");
      printf("³Number of pages: 8    ³\n");
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_J)
   {
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      printf("³Text Mode K           ³\n");
      printf("³Width: 80   Height: 43³\n");
      printf("³Bytes per screen: 3440³\n");
      printf("³Number of pages: 8    ³\n");
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_K)
   {
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      printf("³Text Mode K           ³\n");
      printf("³Width: 80   Height: 50³\n");
      printf("³Bytes per screen: 4000³\n");
      printf("³Number of pages: 8    ³\n");
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_L)
   {
      // ** NOTE ** the smk_conio_gotoxy's here are a hack so that i could use the
      // standard C functions to program this mode.  Normally you would just
      // calculate the proper address and display the text but since DOS still
      // thinks we are in mode 03h (it checks the BIOS), this is necessary.
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      smk_conio_gotoxy(41,1);
      printf("³Text Mode L           ³\n");
      smk_conio_gotoxy(1,2);
      printf("³Width: 40   Height: 43³\n");
      smk_conio_gotoxy(41,2);
      printf("³Bytes per screen: 3440³\n");
      smk_conio_gotoxy(1,3);
      printf("³Number of pages: 8    ³\n");
      smk_conio_gotoxy(41,3);
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
   else if (Mode.mode == MODE_M)
   {
      // ** NOTE ** the smk_conio_gotoxy's here are a hack so that i could use the
      // standard C functions to program this mode.  Normally you would just
      // calculate the proper address and display the text but since DOS still
      // thinks we are in mode 03h (it checks the BIOS), this is necessary.
      printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      smk_conio_gotoxy(41,1);
      printf("³Text Mode K           ³\n");
      printf("³Width: 40   Height: 50³\n");
      smk_conio_gotoxy(1,2);
      smk_conio_gotoxy(41,2);
      printf("³Bytes per screen: 4000³\n");
      smk_conio_gotoxy(1,3);
      printf("³Number of pages: 8    ³\n");
      smk_conio_gotoxy(41,3);
      printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }
}
