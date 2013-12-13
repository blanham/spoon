#include <types.h>
#include <kernel.h>
#include "RAMDriver.h"
#include "RamNode.h"

RAMDriver::RAMDriver(int mb)
:	Application( "RAMDriver" )
{
	(new RamNode( "storage/ramdisk0", mb))->Resume();
}


RAMDriver::~RAMDriver()
{
}

		
