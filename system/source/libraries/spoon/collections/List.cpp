#include <types.h>
#include <stdlib.h>
#include <spoon/collections/List.h>



// ***************************************************

/** Initializes an empty list. */
List::List()
{
	first = NULL;
	last = NULL;
	list_count = 0;
}

/** Initializes a List, which is a duplicate of the provided List. */
List::List(const List& anotherList)
{
	last = NULL;
	first = NULL;
	list_count = 0;
	*this = anotherList;
}

List::~List()
{
}


// ***************************************************

bool List::add(void *item)
{
	struct list_item *tmp;

	tmp =(struct list_item*) malloc( sizeof( struct list_item ) );
	if ( tmp == NULL ) return false;

	tmp->p = item;
	tmp->next = NULL;
	tmp->prev = last;

	if ( last != NULL ) last->next = tmp;
	last = tmp;

	if ( first == NULL ) first = tmp;
	
	list_count += 1;
	return true;
}

int List::count(void) const
{
	return list_count;
}


void* List::get(int index) const
{
	if ( (list_count == 0 ) || (index < 0) || (index >= list_count) ) return NULL;
	if ( index == 0 ) return first->p;
	if ( index == (list_count - 1) ) return last->p;

	int i = 0;
	struct list_item *tmp = first;

	while ( tmp != NULL )
	{
		if ( i++ ==  index ) return tmp->p;
		tmp = tmp->next;
	}

	return NULL;
}


bool List::remove(void *item)
{
	struct list_item *tmp = first;

	while ( tmp != NULL )
	{
		if ( tmp->p == item ) break;
		tmp = tmp->next;
	}

	if ( tmp == NULL ) return false;

	if ( tmp == last ) last = last->prev;
	if ( tmp == first ) first = first->next;

	if ( tmp->prev != NULL ) tmp->prev->next = tmp->next;
	if ( tmp->next != NULL ) tmp->next->prev = tmp->prev;
	free( tmp );

	list_count -= 1;

	if ( list_count == 0 )
	{
	 	first = NULL;
		last = NULL;
	}

	return true;
}




