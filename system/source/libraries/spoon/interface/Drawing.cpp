#include <types.h>
#include <math.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Point.h>
#include <spoon/interface/Drawing.h>
#include <spoon/interface/Bitmap.h>

#include <misc/font.h>

Drawing::Drawing()
{
}

void Drawing::FillRect( Rect block, uint32 col )
{
  FillRect( block.left, block.top, block.right, block.bottom, col );
}

void Drawing::FillRect( int x1, int y1, int x2, int y2, uint32 col )
{
  for ( int x = x1; x <= x2; x++ )
   for ( int y = y1; y <= y2; y++ )
    PutPixel( x, y, col );
}

void Drawing::DrawRect( Rect block, uint32 col )
{
  DrawRect( block.left, block.top, block.right, block.bottom, col );
}

void Drawing::DrawRect( int x1, int y1, int x2, int y2, uint32 col )
{
  for ( int x = x1; x <= x2; x++ )
   {
     PutPixel( x, y1, col );
     PutPixel( x, y2, col );
   }
  for ( int y = y1; y <= y2; y++ )
   {
     PutPixel( x1, y, col );
     PutPixel( x2, y, col );
   }

}



void Drawing::DrawLine( int x1, int y1, int x2, int y2, uint32 col )
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
	      PutPixel( x, y, col );
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
	      PutPixel( x, y, col );
	      if( d >= 0 )
	      {
	         x += sx;
	         d -= ay;
	      }
	      y += sy;
	      d += ax;
      }
   }

   // Do the last pixel. 
	PutPixel( x, y, col );
}


void Drawing::DrawString( int x, int y, char *string, uint32 col )
{
   int nx = x;
   int ny = y;
   int i = 0;
   int c;

   while ( string[i] != 0 ) 
    {
       c = string[i];
       
       bool draw = true;
	  
	  switch ( c )
	  {
	    case '\n':
	      nx = x - 8;
	      ny = ny + 16;
	      draw = false;
	      break;
	      
	    default:
	      draw = true;
	      break;
	  }


       if ( draw == true )
       {
         for ( int row = 0; row < 16; row++ )
          for ( int column = 0; column < 8; column++ )
          {
	      if ( system_font[ c ][row][column] == 1 ) 
	         PutPixel( nx + column, ny + row, col  );
	  }
       }

      nx += 8;
      i++;
    }
}


void Drawing::DrawString( int x, int y, char *string, uint32 col, uint32 bg )
{
   int nx = x;
   int ny = y;
   int i = 0;
   int c;

   while ( string[i] != 0 ) 
    {
       c = string[i];
       
       bool draw = true;
	  
	  switch ( c )
	  {
	    case '\n':
	      nx = x - 8;
	      ny = ny + 16;
	      draw = false;
	      break;
	      
	    default:
	      draw = true;
	      break;
	  }


       if ( draw == true )
       {
         for ( int row = 0; row < 16; row++ )
          for ( int column = 0; column < 8; column++ )
          {
	      if ( system_font[ c ][row][column] == 1 ) 
	         PutPixel( nx + column, ny + row, col  );
	      else
	         PutPixel( nx + column, ny + row, bg  );
	  }
       }

      nx += 8;
      i++;
    }
}

void Drawing::DrawBitmap( int x, int y, Bitmap* bitmap )
{
	for ( int i = 0; i < bitmap->Frame().Width(); i++ )
	 for ( int j = 0; j < bitmap->Frame().Height(); j++ )
		PutPixel( x + i, y + j, bitmap->GetPixel( i,j ) );
}

void Drawing::DrawBitmap( int x, int y, int width, int height, Bitmap* bitmap )
{
	DrawBitmap( x, y, bitmap );
}

