#include <inttypes.h>
#include <alloc.h>
#include <ds/htable.h>

/*  hash table implementation for fast-lookups in the kernel.  */ 



struct hash_table *init_htable( int size, 
				 				int percent, 
								hash_key_func key,
								hash_compare_func compare
							  )
{
	int i;
	struct hash_table *table = (struct hash_table*)malloc(sizeof(struct hash_table));
	if ( table == NULL ) return NULL;
	

	  table->percent = percent;
	  table->size    = size;
	  table->key     = key;
	  table->compare = compare;
	  table->total   = 0;

	  if ( table->size <= 0 )  table->size = 32;

	  table->table = (void**)malloc(table->size * sizeof(void**));

		for ( i = 0; i < table->size; i++ )
				table->table[i] = NULL;


	return table;
}


int delete_htable( struct hash_table* table )
{
	free( table->table );
	free( table );
	return 0;
}

// -------------------------------------------------------------


int htable_rehash( struct hash_table* table, int new_size )
{
	// Allocate the new table..
	void **newTable = (void**)malloc(sizeof(void**) * new_size);
	if ( newTable == NULL ) return -1;		// Out of memory.


	void **oldTable = table->table;
	int oldSize     = table->size;
	int i;

	for ( i = 0; i < new_size; i++) newTable[i] = NULL;

	table->size = new_size;
	table->total = 0;
	table->table = newTable;


	for ( i = 0; i < oldSize; i++ )
		if ( oldTable[i] != NULL ) htable_insert( table, oldTable[i] );

	free( oldTable );
	return 0;
}

// -------------------------------------------------------------

int htable_insert( struct hash_table* table, void *data )
{
	int current_percent;
	int position;
	int key;
	int i;
			
	if ( table->total == table->size ) return -1;

	current_percent = ((table->total + 1) * 100) / table->size;
 
	if ( current_percent > table->percent ) 
				htable_rehash( table, table->size * 2 );
	

	key = table->key( data );
	if ( key < 0 ) key = -key;


	for ( i = 0; i < table->size; i++ )
	{
		position = ( i + key ) % table->size;

		if ( table->table[position] == NULL )
		{
			table->table[position] = data;
			table->total += 1;
			break;
		}
	}
	
	return 0;
}


int htable_remove( struct hash_table* table, void *data )
{
	int current_percent;
	int position;
	int key;
	int i;
			
	if ( table->total == 0 ) return -1;

	current_percent = ((table->total - 1) * 100) / table->size;
   
	if ( (current_percent< (table->percent / 4 )) && (table->size > 32) )
			htable_rehash( table, table->size / 2 );
	

	key = table->key( data );
	if ( key < 0 ) key = -key;

	for ( i = 0; i < table->size; i++ )
	{
		position = ( i + key ) % table->size;

		if ( table->table[position] == data )
		{
			table->table[position] = NULL;
			table->total -= 1;
			break;
		}
	}
	
	return 0;

}

void *htable_retrieve( struct hash_table* table, void *sample )
{
	int position;
	int key;
	int i;
			
	if ( table->total == 0 ) return NULL;


	key = table->key( sample );
	if ( key < 0 ) key = -key;

	for ( i = 0; i < table->size; i++ )
	{
		position = ( i + key ) % table->size;

		if ( table->table[position] != NULL )
			if ( table->compare( table->table[position], sample ) == 0  )
				return table->table[position];


	}
	
	return NULL;
}



void *htable_get( struct hash_table* table, int num )
{
	int i;
	int count = 0;
			
	if ( table->total == 0 ) return NULL;
	if ( num >= table->total ) return NULL;


	for ( i = 0; i < table->size; i++ )
	{
		if ( table->table[i] != NULL )
			if ( count++ == num ) return table->table[i];
	}
	
	return NULL;
}





