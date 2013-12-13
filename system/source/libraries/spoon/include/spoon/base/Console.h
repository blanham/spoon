#ifndef _SPOON_BASE_CONSOLE_H
#define _SPOON_BASE_CONSOLE_H

#include <types.h>

/** \ingroup base 
 *
 *
 */
class Console
{
  public:
     Console( int cols, int rows );
     virtual ~Console();


     int Cols() { return _cols; }
     int Rows() { return _rows; }

  
     virtual void PutChar( uint32 c, char fg, char bg );
     virtual void Clear( char fg, char bg );
     virtual void GotoXY( int x, int y );


  private:
    int _rows;
    int _cols;

};

#endif

