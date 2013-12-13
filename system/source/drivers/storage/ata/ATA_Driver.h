#ifndef _ATA_Driver_H
#define _ATA_Driver_H

#include <spoon/app/Application.h>
#include <spoon/base/IRQHandler.h>

class ATANode;
class PartitionNode;

class ATA_Driver : public Application, public IRQHandler
{
    public:
      ATA_Driver();
      ~ATA_Driver();
      
      
      int Init();
      int IRQ( int irq );
      void Shutdown();


    private:
    	unsigned int Scan();
		unsigned int PartitionSupport();	
	
	ATANode *nodes[4];
	PartitionNode *parts[32];

    private:
        char *part_name( int dev, int num );
    	uint32 primary_partitions( ATANode* ata, int dev );
		uint32 extended_partitions(
				ATANode *ata,
				uint32 lba_sector, 
				int dev,
				int offset
				);

};

#endif


