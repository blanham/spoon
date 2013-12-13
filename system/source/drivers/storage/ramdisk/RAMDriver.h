#include <types.h>
#include <spoon/app/Application.h>

#ifndef _RAMDriver_H
#define _RAMDriver_H

class RAMDriver 	:	public Application
{
	public:
		RAMDriver( int mb = 8);
		~RAMDriver();


};

#endif

