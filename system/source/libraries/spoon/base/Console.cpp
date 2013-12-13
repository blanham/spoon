#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <spoon/base/Console.h>

Console::Console( int rows, int cols )
:
  _rows( rows ),
  _cols( cols )
{
}

Console::~Console()
{
}


void Console::PutChar( uint32 c, char fg, char bg ) {}
void Console::Clear( char fg, char bg ) {}
void Console::GotoXY( int x, int y ) {}





