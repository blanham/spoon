#include <types.h>
#include <macros.h>
#include <dmesg.h>
#include <random.h>
#include <atomic.h>
#include <conio.h>
#include <time.h>

//  This is going to be an array of RANDOM_POOL_SIZE 
//  and different kernel events are going to update a 
//  "random" element in the array in a "random" way
//  and different requests for a random number are 
//  going to pull from a "random" pool.

int random_pool[ RANDOM_POOL_SIZE ];
			// See <random.h>
int random_index;
			// decides which pool to increment
int random_bit;
			// this is the current bit position in the pool
spinlock	random_lock = INIT_SPINLOCK;
			// This is a lock to lock the random pool.


#define ABS(a)   (((a) < 0) ? -(a) : (a) )
#define SGN(a)   (((a) < 0) ? -1 : 1)


int init_random()
{
   int i;
   int opp_pos;
   int tmp;
   int tock;

     dmesg("initialiasing the random number pools.\n" );


     tock = (int)epoch_seconds();
    
     random_index = tock % RANDOM_POOL_SIZE;
     random_bit = 0;
    
	// Let's do a whole bunch of rubbish here with uninitialised
	// data and hope it's random-ish.
     for ( i = 0; i < RANDOM_POOL_SIZE; i++ )
      {
		 opp_pos = RANDOM_POOL_SIZE - i - 1;
         random_pool[i] |= (tock % 16) << ( tock % 32 );
      	 random_pool[i] += ~tock;
		 tock += (tock << ( i % 5 ));	
      }
   
	// Now let's mess things up a little.
     for ( i = 0; i < RANDOM_POOL_SIZE; i++ )
      {
		 opp_pos = RANDOM_POOL_SIZE - i - 1;
     
     	 tmp  = random_pool[i] % random_pool[opp_pos];
		 tmp ^= ~tock;
     
     	 if ( ( tmp % 2 ) == 0 )
        	 random_pool[i] += ( tock >> ( i % 8 ) );
		 else	 
        	 random_pool[i] += ( tock << ( i % 8 ) );
	 
		 tock += (tock << ( i % 5 )) + random_pool[i];
      }

	// This feels a little bit lame. Where did I leave
	// my crypto book?


   return 0;
}



 // This function accepts two numbers. If value2 is greater than
 // value1 then a 0 bit is added to the pool, otherwise a 1 bit
 // is added to the pool.
 //
 // The values given to this function should be time intervals and
 // preferably the time intervals between:
 //
 //	1)  keystrokes        -  fairly random
 //	2)  mouse movements   -  also fairly random
 // 3)  what else?
 //

int add_random( int value1, int value2 )
{
   int index;
   int offset;

     index  = random_bit / ( sizeof(int) * 8 );
     offset = random_bit % ( sizeof(int) * 8 );

	 acquire_spinlock( & random_lock );
	 
		   if ( value2 < value1 ) 
		   	random_pool[ index ] |= (1 << offset);
		   else random_pool[ index ] &= ~(1 << offset);
		    
		   random_bit = ( random_bit + 1 ) % ( RANDOM_POOL_SIZE * sizeof(int) * 8 );
   
	 release_spinlock( & random_lock );

   return 0;
}


int random()
{
   int temp;
   int random_next;

   
	 acquire_spinlock( & random_lock );
	 
	   random_next = (random_index + 1) % RANDOM_POOL_SIZE;
	   temp = random_pool[ random_index ];
	   random_index = random_next;

	 release_spinlock( & random_lock );
	
   return temp;
}



