#include <types.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Scrollable.h>

#include <spoon/interface/ScrollBar.h>

ScrollBar::ScrollBar( Rect frame, Scrollable* target, 
		      bool vertical, uint32 follows )
: View( frame, follows ),  
  _target( target ), _vertical(vertical),  _size(100),
  _unit( 10 ), _value(0), _moving(false)
{
  if ( (vertical) && (target != NULL) )
      target->_vertScrollBar = this;
  if ( (vertical==false) && (target != NULL) )
      target->_horizScrollBar = this;
}


ScrollBar::ScrollBar( Rect frame, Scrollable* target, 
			int size, int unit,
			bool vertical, uint32 follows )
: View( frame, follows ),
	_target( target ), _vertical(vertical), 
	_size( size ), _unit( unit ), _value( 0 ), _moving(false)
{
  if ( (vertical) && (target != NULL) )
      target->_vertScrollBar = this;
  if ( (vertical==false) && (target != NULL) )
      target->_horizScrollBar = this;
}

ScrollBar::~ScrollBar()
{
  if ( (_vertical) && (_target != NULL) )
      _target->_vertScrollBar = NULL;
  if ( (_vertical==false) && (_target != NULL) )
      _target->_horizScrollBar = NULL;
}


void ScrollBar::SetSize( int size, int unit )
{
  _size = size;
  _unit = unit;
  if ( _value > _size )  ScrollTo( _value );
}

// ----------------------------

void ScrollBar::ScrollTo( int value )
{
   _value = value;

   if ( _value < 0 ) _value = 0;
   if ( _value > (_size - _unit) ) _value = _size - _unit;

   if ( _target != NULL ) _target->ScrollTo( value, Vertical() );

   Draw( Bounds() );
}

void ScrollBar::ScrollBy( int diff )
{
   _value = _value + diff;

   if ( _value < 0 ) _value = 0;
   if ( _value > (_size - _unit) ) _value = _size - _unit;

   if ( _target != NULL ) _target->ScrollBy( diff, Vertical() );

   Draw( Bounds() );
}

// ----------------------------------

void ScrollBar::Draw( Rect area )
{
  if ( _vertical ) DrawVertical( area );
  	    else   DrawHorizontal( area );
}


void ScrollBar::DrawVertical( Rect area )
{
  Rect place;


	// top
  place = Bounds();
  place.bottom = 20;

  DrawRect( place, 0x444444 );

  place.left += 1;
  place.right -= 1;
  place.top += 1;
  place.bottom -= 1;

  FillRect( place, 0x888888 );


	// middle
     place = Bounds();
     place.top = 21;
     place.bottom -= 21;

     FillRect( place, 0x666666 );


  // bottom
  place = Bounds();
  place.top = place.bottom - 20;

  DrawRect( place, 0x444444 );

  place.left += 1;
  place.right -= 1;
  place.top += 1;
  place.bottom -= 1;

  FillRect( place, 0x888888 );


  if ( Size() <= 1 ) return;

	// BAR.....  
	/// \todo make this inline.. IE, draw top, middle-top, bar,	middle-bottom, bottom

  double percent = (Bounds().bottom - 40) / Size();

  int start  = 20 +  (int)( Value() * percent ); 
  int bottom = start + (int)( Unit() * percent );


   DrawRect( Rect(0, start, Bounds().right, bottom), 0x222222 );
   FillRect( Rect(1, start+1, Bounds().right-1, bottom-1), 0x777777 );
}

void ScrollBar::DrawHorizontal( Rect area )
{
  Rect place;

	// left
  place = Bounds();
  place.right = 20;

  DrawRect( place, 0x444444 );

  place.left += 1;
  place.right -= 1;
  place.top += 1;
  place.bottom -= 1;

  FillRect( place, 0x888888 );


	// middle
     place = Bounds();
     place.left = 21;
     place.right -= 21;

     FillRect( place, 0x666666 );


  // right
  place = Bounds();
  place.left = place.right - 20;

  DrawRect( place, 0x444444 );

  place.left += 1;
  place.right -= 1;
  place.top += 1;
  place.bottom -= 1;

  FillRect( place, 0x888888 );


  if ( Size() <= 1 ) return;

	// BAR.....  
	/// \todo make this inline.. IE, draw top, middle-top, bar,	middle-bottom, bottom

  double percent = (Bounds().right - 40) / Size();

  int start  = 20 +  (int)( Value() * percent ); 
  int bottom = start + (int)( Unit() * percent );


   DrawRect( Rect(start, 0, bottom, Bounds().bottom), 0x222222 );
   FillRect( Rect(start+1, 1, bottom-1, Bounds().bottom), 0x777777 );
}

// ---------------------------------------


void ScrollBar::MouseDown( int x, int y, unsigned int buttons )
{
  _moving = true;
  if ( Size() <= 1 ) return;

  if ( Vertical() )
   {
     if ( y < 20 ) ScrollBy( - Unit() );
     else
       if ( y > (Bounds().bottom - 20) ) ScrollBy( Unit() );
         else
	  { 
	     double percent = (Bounds().bottom - 40) / Size();
	     int new_val = 0;
	     if ( percent != 0 )
	     	new_val = (int)(  (y - 20) / percent );
	     ScrollTo( new_val );
	  }
   }
  else
   {
     if ( x < 20 ) ScrollBy( - Unit() );
     else
       if ( x > (Bounds().right - 20) ) ScrollBy( Unit() );
         else
	  { 
	     double percent = (Bounds().right - 40) / Size();
	     int new_val = 0;
	     if ( percent != 0 ) 
	       new_val = (int)(  (x - 20) / percent );
	     ScrollTo( new_val );
	  }
   }
}

void ScrollBar::MouseUp( int x, int y, unsigned int buttons )
{
  _moving = false;
}

void ScrollBar::MouseMoved( int x, int y, unsigned int buttons )
{
  if ( _moving == false ) return;
  if ( Size() <= 1 ) return;
    
    
    if ( Vertical() )
     {
       if ( (y > 20) && (y < Bounds().bottom - 20)  )
  	  MouseDown( x, y, buttons );
     }
    else
     {
       if ( (x > 20) && (x < Bounds().right - 20)  )
  	  MouseDown( x, y, buttons );
     }

}

	
