#ifndef _SPOON_COLLECTIONS_COLLECTION_H
#define _SPOON_COLLECTIONS_COLLECTION_H

#include <stdlib.h>



/**  \defgroup collections Collections 	
 *
 */

/** \addtogroup collections 
 *
 * The Collection class is the base class from which all collections,
 *  such as List, HashTable, Map, etc, will be derived. This allows
 *  for much better class design and the ability to pass multiple
 *  types of collections to some useful functions without having to
 *  implement a function per collection type.
 *
 *  All children class must implement the pure virtual functions, of 
 *  course. Any other virtual functions should also be implemented
 *  if your implementation can speed up the method.
 */


class Collection
{
	public:
		Collection();
		Collection( const Collection& anotherCollection );
		virtual ~Collection();


		virtual bool  add(void *item) = 0;
		virtual void* get(int index) const = 0;
		virtual bool  remove(void *item) = 0;
		virtual int   count(void) const = 0;


		virtual bool insert( const Collection& anotherCollection );
		virtual int  indexOf(void *item) const;
		
		
		virtual void forEach(bool (*func)(void *, void *), void *arg2 = NULL);


		virtual void empty(void);
		virtual void freeAll(void);


		virtual Collection& operator =(const Collection&);
		virtual Collection& operator +=(const Collection&);
		
};

#endif


