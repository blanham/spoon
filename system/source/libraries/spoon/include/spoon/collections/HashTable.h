#ifndef _SPOON_COLLECTIONS_HASHTABLE_H
#define _SPOON_COLLECTIONS_HASHTABLE_H

#include <spoon/collections/Collection.h>




/** This is a function which is supposed to return an
 * integer based on the data given. The integer can
 * be any number. It will be normalized by the HashTable.
 */
typedef int (*hash_key)( void *data );

/** This is a comparison function. It must return 0 if
 * the two objects are logically equal. It should
 * return -1 if a is less than b and 1 if a is greater
 * than b.
 */
typedef int (*hash_cmp)( void *a, void *b );


/** \addtogroup collections
 * 
 * The HashTable collection is a simple HashTable implementation
 * which allows the user to override the private Hash and Compare 
 * methods to do modify the way their HashTable works.
 *
 * This implementation hashes the items by memory address, has a
 * default size of 64 and a percentage of 70%.
 *
 * Setting a percentage of 0 will disable re-hashing, so you can
 * never insert more data than the initial size. A rehash results 
 * in a hash table double the size of the current table or half 
 * the size of the current table if hashing down. 
 *
 */


class HashTable : public Collection
{
	public:
		HashTable( hash_key key_func, hash_cmp cmp_func, 
					int size = 64, int percentage = 70 );
		virtual ~HashTable();


		bool  add(void *item);
		void* get(int index) const;
		bool  remove(void *item);
		int   count(void) const;


		void* retrieve( void *sample );
		
		
		
	private:
		hash_key m_key;
		hash_cmp m_cmp;
		
		int rehash( int size );
		
		void **m_hTable;		///< The internal hash table, referencing inserted data.
		
		int m_hSize;			///< The size of the hash table.
		int m_hTotal;		///< The total number of data already inserted into the table.
		int m_hPercent;		///< The configured percentage tolerance of the hash table.
		
};

#endif

