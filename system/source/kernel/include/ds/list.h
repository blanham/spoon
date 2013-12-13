#ifndef _KERNEL_DS_LIST_H
#define _KERNEL_DS_LIST_H


#define NEXT_LINEAR( obj, type )					\
	(( obj->list->next == NULL ) ? NULL :			\
			(type)obj->list->next->obj;)			\
		

#define PREV_LINEAR( obj, type )					\
	(( obj->list->prev == NULL ) ? NULL :			\
			(type)obj->list->prev->obj;)			\


#define LINKED_LIST			struct ds_list list

#define LIST_INIT( robj )	list_init( &(robj->list), robj )	

struct ds_list	
{										
	void* object;
	struct ds_list* prev;					
	struct ds_list* next;					
};							


static inline void list_init( struct ds_list *l, void *object )
{
	l->object = object;
	l->prev = NULL;
	l->next = NULL;
}


static inline void list_remove( struct ds_list *l )
{
	if ( l->prev != NULL )
			 l->prev->next = l->next;
	if ( l->next != NULL )
			  l->next->prev = l->prev;

	list_init( l, l->object );
}


static inline void list_insertBefore( struct ds_list *p, struct ds_list* l )
{
	l->prev = p->prev;
	l->next = p;
	p->prev = l;

	if ( l->prev != NULL ) l->prev->next = l;
}

static inline void list_insertAfter( struct ds_list *p, struct ds_list* l )
{
	l->prev = p;
	l->next = p->next;
	p->next = l;

	if ( l->next != NULL ) l->next->prev = l;
}



#endif

