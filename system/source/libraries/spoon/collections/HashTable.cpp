#include <types.h>
#include <stdlib.h>
#include <spoon/collections/HashTable.h>



/** The HashTable constructor takes two hash functions (key and compare),
 * percent and size as agurments. A percent size of 0 will disable rehashing 
 * and limit the HashTable to it's initial size. A percentage of 100 will 
 * only rehash the table when the table becomes full.
 * 
 * The valid range for percentage are [0, 100] and the valid range
 * for size is [32, infinity] ( Assuming you have inifinity GB of ram ).
 *  
 *  \note A size less than 32 will result in an initial size of 32.
 *  
 *  \param key_func A key function which returns an integer base on data.
 *  \param cmp_func A comparision function.
 *  \param percentage The percentage level at which to rehash the table.
 *  \param size The initial size of the HashTable.
 */

HashTable::HashTable( hash_key key_func, hash_cmp cmp_func, int size, int percentage )
{
	m_key = key_func;
	m_cmp = cmp_func;

	m_hSize		= size;
	m_hPercent 	= percentage;
	m_hTotal 	= 0;

	if ( m_hSize < 32 ) m_hSize = 32;
	if ( (m_hPercent < 0) || (m_hPercent > 100) )   m_hPercent = 70;
		

		m_hTable = (void**)malloc( size * sizeof(void**) );
	
		
	for ( int i = 0; i < m_hSize; i++ )
		m_hTable[i] = NULL;
}

/** The destructor frees the internal hash table (m_hTable) but not the
 * data stored in the hash table. It's up to you to clean the hash
 * table data up.
 */
HashTable::~HashTable()
{
	if ( m_hTable != NULL ) free( m_hTable );
}


/** The normal Collection add method. This add implementation will
 * rehash if the percentage used before the insertion is equal to
 * or greater than the configured percentage.  
 *
 * \param data The data to be added to the hash table.
 * \return true if the data was successfully added. 
 */
bool  HashTable::add( void *data )
{
		// Full and hashing disabled, return failure.
	if ( (m_hTotal == m_hSize) && (m_hPercent == 0 ) ) return false;
		
		
		// Check to see if rehashing is required.
	
		if ( (((m_hTotal * 100 ) / m_hSize) >= m_hPercent) && (m_hPercent != 0) ) 
		{
			rehash( m_hSize * 2 );	
		}

		// Not great.. We're full and rehashing failed.
		if ( m_hTotal == m_hSize ) return false;
	
	// Hash the data.
	int key = m_key( data );
		if ( key < 0 ) key = -key;
		key = key % m_hSize;
	
	for ( int i = 0; i < m_hSize; i++ )
	{
		int position = (i + key) % m_hSize;

		if ( m_hTable[position] == NULL )
		{
			m_hTable[position] = data;
			m_hTotal += 1;
			return true;
		}
	}

	// Please don't ever get here.
	return false;
}


/** Remove is the standard Collection remove and this
 * does not trigger a rehashing if the hash table becomes
 * too empty.
 *
 * \param data The actual data to be removed.
 *
 * \return true if the data was found and removed successfully.
 */
bool  HashTable::remove( void *data )
{
	if ( (data == NULL) || (m_hTotal == 0) ) return false;
	
	// Hash the data.
	int key = m_key( data );
		if ( key < 0 ) key = -key;
		key = key % m_hSize;
	
	
	for ( int i = 0; i < m_hSize; i++ )
	{
		int position = (i + key) % m_hSize;
			
		if ( m_hTable[ position ] == data )
		{
			m_hTable[ position ] = NULL;
			m_hTotal -= 1;
			return true;
		}
	}

	return false;
}


int   HashTable::count() const
{
	return m_hTotal;
}



/** This is a slow method and the results are not sorted 
 * after multiple additions and removals.
 *
 * \warning This iterates the whole hash table and returns
 * the data the moment it sees it. So, worst case scenario,
 * it takes N iterations to check a hash table of N elements.
 *
 */
void*  HashTable::get( int i ) const
{
	if ( (m_hTotal == 0) || (i >= m_hTotal) || ( i < 0 ) ) return NULL;
	
	int count = 0;
	for ( int i = 0; i < m_hSize; i++ )
	{
		if ( m_hTable[ i ] == NULL ) continue;
		
		if ( count++ == i ) return m_hTable[ i ];
	}

	return NULL;
}


/** This method accepts a "sample" of data in order to locate
 * the data stored in the hash table. The sample is passed to
 * the m_key method to find an initial starting point and then
 * m_cmp is called on all following data ( with sample being
 * the second parameter ) in order to determine if the data being
 * compared in the correct one.
 *
 * \param sample A sample of the data to locate. Passed to m_cmp as parameter 2.
 *
 * \return The data retrieved or NULL if not found.
 */
void* HashTable::retrieve( void *sample )
{
	int key = m_key( sample );
	if ( key < 0 ) key = -key;
	key = key % m_hSize;

	for ( int i = 0; i < m_hSize; i++ )
	{
		int position = (i + key) % m_hSize;

		if ( m_hTable[position] == NULL ) continue;
		
		if ( m_cmp( m_hTable[position], sample ) == 0 ) 
					return m_hTable[position];
	}

	return NULL;
}



		
		
	
/** The rehash method removes the internal hash table,
 * sets up the HashTable of the new size with brand new data 
 * (empty hash table, 0 count, etc) and then start's
 * add()'ing in all the old data. After it's done, the old
 * hash table is freed up.
 *
 *
 * \param size The new size of the hash table.
 * \return 0 if successfully rehashed.
 */
int HashTable::rehash( int size )
{
	void **newTable = (void**)malloc( sizeof(void**) * size );
	if ( newTable == NULL ) return -1;
	
	for ( int i = 0; i < size; i++ )
		newTable[i] = NULL;

	int oldSize = m_hSize;

	// swap tables..
	void **tmpTable = m_hTable;
	        m_hTable = newTable;
			m_hSize  = size;
			m_hTotal = 0;

	for ( int i = 0; i < oldSize; i++ )
		if ( tmpTable[i] != NULL ) add( tmpTable[i] );

	free( tmpTable );
	return 0;
}





