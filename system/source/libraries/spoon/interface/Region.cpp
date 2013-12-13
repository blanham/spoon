#include <types.h>
#include <stdio.h>
#include <spoon/collections/List.h>

#include <spoon/interface/Rect.h>
#include <spoon/interface/Point.h>
#include <spoon/interface/Region.h>


Region::Region(const Region& region)
{
	*this = region;
}


Region::Region(const Rect& rect)
{
	m_boxes.add( new Rect(rect) );
}


Region::Region()
{
}


Region::~Region()
{
	MakeEmpty();
}


bool Region::Contains(const Point& point) const
{
	int i;
	Rect *sam;

	for ( i = 0; i < m_boxes.count(); i++)
	{
		sam = (Rect*)m_boxes.get( i );
		if ( sam->Contains( point ) ) return true;
	}
	return false;
}


Rect Region::Frame(void) const
{
	int left;
	int top;
	int right;
	int bottom;
	int i;
	Rect *sam;

	if ( m_boxes.count() == 0 ) return Rect(0,0,0,0);
	sam = (Rect*)m_boxes.get(0);

	left   = sam->left;
	top    = sam->top;
	right  = sam->right;
	bottom = sam->bottom;

	for ( i = 1; i < m_boxes.count(); i++)
	{
		sam = (Rect*)m_boxes.get( i );
		if ( sam->left   < left   )  left  = sam->left;
		if ( sam->top    < top    )   top  = sam->top;
		if ( sam->right  > right  )  right = sam->right;
		if ( sam->bottom > bottom ) bottom = sam->bottom;
	}

	return Rect( left,top,right,bottom );
}


bool Region::Intersects(const Rect& rect) const
{
	for ( int i = 0; i < m_boxes.count(); i++)
	{
		Rect *sam = (Rect*)m_boxes.get( i );
		if ( sam->Intersects( rect ) == true ) return true;
	}
	return false;
}


void Region::MakeEmpty(void)
{
	m_boxes.freeAll();
}


void Region::OffsetBy(int horizontal, int vertical)
{
	for ( int i = 0; i < m_boxes.count(); i++)
	{
		Rect *sam = (Rect*)m_boxes.get( i );
		sam->left 		+= horizontal;
		sam->right 		+= horizontal;
		sam->top  		+= vertical;
		sam->bottom  	+= vertical;
	}
}



Rect* Region::RectAt(int index) const
{
	if ( index < 0 ) return NULL;
	if ( index >= m_boxes.count() ) return NULL;
	return (Rect*)m_boxes.get( index );
}


int Region::CountRects(void) const
{
	return m_boxes.count();
}



void Region::Set(Rect rect)
{
	MakeEmpty();
	m_boxes.add( new Rect( rect ) );
}


void Region::Include(const Rect& rect)
{
	m_boxes.add( new Rect( rect ) );
}


void Region::Include(const Region& region)
{
	for ( int i = 0; i < region.CountRects(); i++)
	{
		m_boxes.add( new Rect(*(region.RectAt( i ))) );
	}
}


void Region::Exclude(const Rect& rect)
{
	Rect *sam;
	Rect *tmp;
	int sl,st,sb,sr;
	int rl,rt,rb,rr;
	int mil,mit,mir,mib;
	int i;

	int total = m_boxes.count();

	for ( i = 0; i < total; i++)
	{
	    sam = (Rect*)m_boxes.get( i );

		// No connection.
	    if ( sam->Intersects( rect ) == false ) continue;

		// Rect is bigger than or equal to this. This rect is pointless.
		if ( (rect == *sam) || (rect.Contains(*sam)) )
		{
			m_boxes.remove( sam );
			i -= 1;
			total -= 1;
			continue;
		}

		// The current rect contains it entirely. We need to split it up.
		if ( sam->Contains( rect ) )
		{
			if ( (rect.top-1) > sam->top )	// -1 because of exclusion.
			{
				tmp = new Rect( sam->left, sam->top, sam->right, rect.top - 1 ); 
				m_boxes.add( tmp );
			}
	
			if ( (rect.bottom+1) < sam->bottom )
			{
				tmp = new Rect( sam->left, rect.bottom+1, sam->right, sam->bottom );
				m_boxes.add( tmp );
			}
	
			if ( sam->left < (rect.left-1) )
			{
				tmp =  new Rect( sam->left, rect.top, rect.left-1, rect.bottom ); 
				m_boxes.add( tmp );
			}
	
			if ( sam->right > (rect.right+1) )
			{
				tmp = new Rect( rect.right+1, rect.top, sam->right, rect.bottom );
				m_boxes.add( tmp );
			}
	
			m_boxes.remove( sam );
			i -= 1;
			total -= 1;
			continue;
		}

	

		// Overlapping...
		
			// Durand's Fantabulous Rectangle Exclusion Algorithm
			//	November 7th 2003
	
			sl = sam->left;
			sr = sam->right;
			rl = rect.left - 1;
			rr = rect.right + 1;

			st = sam->top;
			sb = sam->bottom;
			rt = rect.top - 1;
			rb = rect.bottom + 1;
	
			sl < rl ? mil = rl : mil = sl;
			st < rt ? mit = rt : mit = st;
			sr < rr ? mir = sr : mir = rr;
			sb < rb ? mib = sb : mib = rb;

			// ....... top row

			if ( (sl < mil) && (st < mit) )  m_boxes.add( new Rect( sl, st, mil, mit ) );
			if ( (mil < mir) && (st < mit) ) m_boxes.add( new Rect( mil, st, mir, mit ) );
			if ( (mir < sr) && (st < mit) )  m_boxes.add( new Rect( mir, st, sr, mit ) );
	
				// ....... middle row
	
	
			if ( (sl < mil) && (mit < mib) )  m_boxes.add( new Rect( sl, mit, mil, mib ) );
			if ( (mir < sr) && (mit < mib) )  m_boxes.add( new Rect( mir, mit, sr, mib ) );
	
				// ....... bottom row
	

			if ( (sl < mil) && (mib < sb) )  m_boxes.add( new Rect( sl, mib, mil, sb ) );
			if ( (mil < mir) && (mib < sb) )  m_boxes.add( new Rect( mil, mib, mir, sb ) );
			if ( (mir < sr) && (mib < sb) )  m_boxes.add( new Rect( mir, mib, sr, sb ) );
	
	
				// ..........................
	
			m_boxes.remove(sam);
			i -= 1;
			total -= 1;
	}

}


void Region::Exclude(const Region& region)
{
}


void Region::IntersectWith(const Region& region)
{
}


Region& Region::operator=(const Region& region)
{
	m_boxes.empty();

	for ( int i = 0; i < region.CountRects(); i++ )
	{
		m_boxes.add( new Rect( *( region.RectAt(i) ) ) );
	}

	return *this;
}



