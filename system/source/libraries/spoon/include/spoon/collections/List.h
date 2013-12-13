#ifndef _SPOON_COLLECTIONS_LIST_H
#define _SPOON_COLLECTIONS_LIST_H

#include <types.h>
#include <spoon/collections/Collection.h>


/** A structure which contains the pointer to the
 * data which is going to be inserted into the list, as
 * well as linked-list information (next,prev)
 */
struct list_item
{
	void *p;
	struct list_item *next;
	struct list_item *prev;
};


/** \ingroup collections
 *
 * The List class is a double-linked list implementation,
 * based on the Collection class. 
 * 
 */
class List : public Collection
{
	public:
		List();
		List(const List& anotherList);
		virtual ~List();

		virtual bool  add(void *item);
		virtual void* get(int index) const;
		virtual bool  remove(void *item);
		virtual int   count(void) const;

		

	private:
		struct list_item *first;	//< The first item in the list.
		struct list_item *last;		//< The last item in the list.
		int list_count;				//< A running count of the number of items in the list.
};

#endif


