#ifndef _SPOON_INTERFACE_SCROLLBAR_H
#define _SPOON_INTERFACE_SCROLLBAR_H

#include <types.h>
#include <spoon/interface/View.h>
#include <spoon/interface/Scrollable.h>


/** \ingroup interface 
 *
 */
class ScrollBar : public View
{
   public:
     ScrollBar( Rect frame,
     		Scrollable* target, 
     		bool vertical = true, 
		uint32 follows = FOLLOW_ALL_SIDES );

     ScrollBar( Rect frame,
     		Scrollable* target,
		int size,
		int unit,
     		bool vertical = true, 
		uint32 follows = FOLLOW_ALL_SIDES );

     ~ScrollBar();

	void SetSize( int size, int unit );
	int Value() { return _value; }
	int Size() { return _size; }
	int Unit() { return _unit; }
	bool Vertical() { return _vertical; }



	void ScrollTo( int value );
	void ScrollBy( int diff );


	virtual void Draw( Rect area );

	virtual void MouseDown( int x, int y, unsigned int buttons );
	virtual void MouseUp( int x, int y, unsigned int buttons );
	virtual void MouseMoved( int x, int y, unsigned int buttons );

   protected:
     virtual void DrawVertical( Rect area );
     virtual void DrawHorizontal( Rect area );

   private:
     Scrollable *_target;
     bool _vertical;
     int _size;
     int _unit;
     int _value;
     bool _moving;

};

#endif

