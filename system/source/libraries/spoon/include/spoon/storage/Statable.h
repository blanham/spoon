#ifndef _SPOON_STORAGE_STATABLE_H
#define _SPOON_STORAGE_STATABLE_H



#include <spoon/storage/StorageDefs.h>

/** \ingroup storage
 *
 *
 */
class Statable
{
	public:
		Statable();
		virtual ~Statable();
		
			
     	virtual int Stat( struct stat *st );
};


#endif

