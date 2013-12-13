#include <types.h>
#include <stdlib.h>
#include <spoon/support/String.h>
#include <spoon/collections/Collection.h>



/** Just your standard constructor */
Collection::Collection()
{
}

/** Collection copy constructor which accepts another Collection
 * and uses the "=" operator to assign the values to this
 * collection. 
 *
 * Values are not duplicated, they are merely referenced.
 */

Collection::Collection(const Collection& anotherCollection)
{
	*this = anotherCollection;
}


/** The destructor does nothing. It's up to your code or child
 * classes to clean up anything that should be cleaned up.
 */
Collection::~Collection()
{
}


/** Inserts all the data from the given Collection into the 
 * current Collection.
 *
 * \param anotherCollection The Collection to insert.
 *
 * \return true if the Collection was successfully inserted.
 */
bool Collection::insert( const Collection& anotherCollection )
{
	for ( int i = 0; i < anotherCollection.count(); i++ )
	 if ( add( anotherCollection.get(i) ) == false ) 
		return false;

	return true;
}

/** This returns the index of the given item within the Collection.
 *
 * \param item The data to find the index of.
 * \return -1 if the data was not found or else the index of the data.
 *
 */
int  Collection::indexOf(void *item) const
{
	for ( int i = 0; i < count(); i++ )
		if ( get(i) == item ) return i;

	return -1;
}

// ************************************************************


/** For every single item in the Collection, call the given function
 * with arg2 as the second argument.  If the given function returns
 * true, then the forEach method returns. If the given function
 * returns false, the loop continues and further items are processed.
 *
 * \param func The function to which to pass the data.
 * \param arg2 The second argument passed to the func parameter.
 */
void Collection::forEach(bool (*func)(void *, void *), void *arg2)
{
	for ( int i = 0; i < count(); i++ )
		if ( func( get(i), arg2 ) == true ) break;
}


// ************************************************************


/** This method removes every single item from the Collection, individually,
 * from index 0. */
void Collection::empty(void)
{
	while ( count() > 0 ) remove( get(0) );
}

/** The same as empty() but this method also free()'s the data
 * that has been removed.
 *
 * \warning This won't work very well if you're free()'ing objects or 
 * 			structures which contain objects because the destructors
 * 			will never be called!! So it's only safe to use this on
 * 			raw data.
 */
void Collection::freeAll(void)
{
	while ( count() > 0 )
	{
		void *data = get(0);
		remove( data );
		if ( data != NULL ) free( data );
	}
}



// **********************************************************************


/** Assignment operator which does a empty() call before adding
 * all the data from the provided Collection its own Collection.
 * 
 *
 * \warning Because it's only adding pointers, the data itself is not 
 * 			duplicated.	Remember that.
 *
 * 	\param lst The Collection to be absorbed.
 * 	\return This collection.
 */
Collection& Collection::operator =(const Collection& lst)
{
	empty();

	for ( int i = 0; i < lst.count(); i++)
	  add( lst.get(i) );
	
	return *this;
}

/** Adds all the data within the given Collection to this collection.
 *
 * \warning Because it's only adding pointers, the data itself is not 
 * 			duplicated.	Remember that.
 * 
 * 	\param lst The Collection to be absorbed.
 * 	\return This collection.
 */
Collection& Collection::operator +=(const Collection& lst)
{
	for ( int i = 0; i < lst.count(); i++)
	  add( lst.get(i) );
	
	return *this;
}


