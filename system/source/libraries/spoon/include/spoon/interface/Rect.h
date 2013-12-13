#ifndef _SPOON_INTERFACE_RECT_H
#define _SPOON_INTERFACE_RECT_H

#include <spoon/interface/Point.h>


/** \ingroup interface 
 *
 */
class Rect
{
  public:
     Rect();
     Rect( int _left, int _top, int _right, int _bottom );
	 Rect( const Rect& rect );
	 virtual ~Rect();
	 

     int Width() const;
     int Height() const;

     bool IsValid() const;

     bool Contains( int x, int y ) const;
     bool Contains( const Point &point  ) const;
     bool Contains( const Rect &rect  ) const;
	 bool Intersects( const Rect &rect ) const;

	 
     void Set( int _left, int _top, int _right, int _bottom );
     
	 bool operator==(const Rect &rect) const;
	 Rect& operator=(const Rect &rect);

  public:
     int left;
     int top;
     int right;
     int bottom;


};

#endif


