#ifndef _KERNEL_DS_HTABLE_H
#define _KERNEL_DS_HTABLE_H


typedef int (*hash_key_func)(void *);
typedef int (*hash_compare_func)(void *,void*);


struct hash_table 
{
	hash_compare_func compare;
	hash_key_func key;
	int total;
	int size;
	int percent;
	void **table;
};



struct hash_table *init_htable( int size, 
				 				int percent, 
								hash_key_func key,
								hash_compare_func compare
							  );

int delete_htable( struct hash_table* );

int htable_rehash( struct hash_table*, int new_size );

int htable_insert( struct hash_table* table, void *data );

int htable_remove( struct hash_table* table, void *data );

void *htable_retrieve( struct hash_table* table, void *sample );

void *htable_get( struct hash_table* table, int num );



#endif

