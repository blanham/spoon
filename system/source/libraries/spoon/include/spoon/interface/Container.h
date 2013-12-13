#ifndef _SPOON_INTERFACE_CONTAINER_H
#define _SPOON_INTERFACE_CONTAINER_H


#include <types.h>
#include <spoon/collections/List.h>
#include <spoon/base/ThreadLock.h>


class View;

/**  \addtogroup interface
 *
 * The Container class is similar to the Collection
 * class in that you can add Views to to it. In addition
 * to this, it implements the lockable class.
 *
 * The Container should be thread-safe. However,
 * retrieving a View and then working on it is
 * not thread-safe unless your Views are thread-safe.
 * 
 */

class Container : public ThreadLock
{
	public:
		Container();
		Container( const Container& container );
		virtual ~Container();

		
		virtual bool  addChild( View *view );
		View* getChild( int i );
		virtual bool  removeChild( View *view );
		int   countChildren();



	private:
		List m_list;
	
};


#endif

