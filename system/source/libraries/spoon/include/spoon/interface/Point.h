#ifndef _SPOON_INTERFACE_POINT_H
#define _SPOON_INTERFACE_POINT_H


/** \ingroup interface 
 *
 */
class Point
{
  public:
     Point() : x(0), y(0) {};
     Point( int _x, int _y ) : x(_x), y(_y)  {};
	 Point( const Point& p )
	 : x( p.x ), y( p.y ) {};
	 
     virtual ~Point() {};

     void Set( int _x, int _y )
       { x = _x; y = _y; }

     
     

  public:
     int x;
     int y;


};

#endif


