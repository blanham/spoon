#ifndef _SPOON_INTERFACE_REGION_H
#define _SPOON_INTERFACE_REGION_H


#include <types.h>
#include <spoon/collections/List.h>
#include <spoon/interface/Rect.h>
#include <spoon/interface/Point.h>


/** \ingroup interface
 *
 * Region is a class which maintains Rects in a "region" and allows
 * one to include or exclude additional rectangles.
 *
 */

class Region
{
	public:
		Region(const Region& region);
		Region(const Rect& rect);
		Region();
		virtual ~Region();

		Rect Frame(void) const;
	
	
		Rect* RectAt(int index) const;
		int CountRects(void) const;
		void MakeEmpty(void);
	
		void Set(Rect rect);

		void Include(const Rect& rect);
		void Include(const Region& region);
		void Exclude(const Rect& rect);
		void Exclude(const Region& region);

		bool Contains(const Point &point) const;
		void IntersectWith(const Region& region);
		bool Intersects(const Rect &rect) const;
		

		void OffsetBy(int horizontal, int vertical);
		

		Region& operator =(const Region&);
	
	
    private:
		List m_boxes;
};

#endif

