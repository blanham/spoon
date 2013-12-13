#include <spoon/interface/Rect.h>

Rect::Rect()
: left(0), top(0), right(-1), bottom(-1) 
{}

Rect::Rect( int _left, int _top, int _right, int _bottom )
: left(_left), top(_top), right(_right), bottom(_bottom) 
{}

Rect::Rect( const Rect& rect )
{
	left = rect.left;
	right = rect.right;
	top = rect.top;
	bottom = rect.bottom;
}

Rect::~Rect() 
{
}

void Rect::Set( int _left, int _top, int _right, int _bottom )
{ 
	left = _left; 
	top = _top; 
	right = _right; 
	bottom = _bottom; 
}

int Rect::Width() const 
{ 
	return (right - left) + 1; 
}

int Rect::Height() const
{ 
	return (bottom - top) + 1; 
}

bool Rect::IsValid() const
{ 
	return ((right >= left) && (bottom >= top)); 
}


bool Rect::Contains( int x, int y ) const
{
  if (  (x>=left) && (x<=right) &&
        (y>=top)  && (y<=bottom)   ) return true;
  return false;
}
     

bool Rect::Contains( const Point &point  ) const
{
	return Contains( point.x, point.y );
}

bool Rect::Contains( const Rect &rect  ) const
{
	Point a(rect.left,rect.top);
	Point b(rect.right,rect.bottom);

		
	return ( Contains(a) && Contains(b) );
}


bool Rect::Intersects( const Rect& rect ) const
{
	if ( rect.bottom < top ) 	  return false;
	if ( bottom      < rect.top ) return false;
	

	if ( rect.right < left ) 	  return false;
	if ( right      < rect.left) return false;
	
	return true;
}




bool Rect::operator==(const Rect &rect) const
{
	if ( left 	!= rect.left ) return false;
	if ( top 	!= rect.top ) return false;
	if ( right 	!= rect.right ) return false;
	if ( bottom != rect.bottom ) return false;
	return true;
}

Rect& Rect::operator=(const Rect &rect)
{
	left 	= rect.left;
	right 	= rect.right;
	top 	= rect.top;
	bottom 	= rect.bottom;
	return *this;
}


