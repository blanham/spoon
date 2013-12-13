#include <types.h>
#include <string.h>
#include <stdio.h>

#include "ATANode.h"
#include "PartitionNode.h"
#include "ATA_Driver.h"

#include "ataio.h"


// *********************************************

ATA_Driver::ATA_Driver()
: Application("driver.ata")
{
}

ATA_Driver::~ATA_Driver()
{
}

// *********************************************

int ATA_Driver::Init()
{
   RequestIRQ( 0xE );
   RequestIRQ( 0xF );
	
   Scan();
   PartitionSupport();

   return 0;
}

void ATA_Driver::Shutdown()
{
   ReleaseIRQ( 0xE );
   ReleaseIRQ( 0xF );
}

// *********************************************

int ATA_Driver::IRQ( int irq )
{
  return 0;
}

// *********************************************


unsigned int ATA_Driver::Scan()
{

  for ( int i = 0; i < 4; i++ )
  {
    char name[64];
     sprintf(name,"%s%i","storage/ata/disc",i);
     
     switch (i)
     {
       case 0: nodes[i] = new ATANode( name, BASE1, 0); break;
       case 1: nodes[i] = new ATANode( name, BASE1, 1); break;
       case 2: nodes[i] = new ATANode( name, BASE2, 0); break;
       case 3: nodes[i] = new ATANode( name, BASE2, 1); break;
     }

     if ( ! nodes[i]->Present() ) 
     {
       delete nodes[i];
       nodes[i] = NULL;
     }
     else nodes[i]->Resume();

  }

   return 0;
}


// ***************************************************


char *ATA_Driver::part_name( int dev, int num )
{
   static char result[1024];	// means no multiple threads
   char ans[16];

   strcpy( result, "storage/hd" );

   switch (dev)
   {
     case 0: strcat( result, "a" ); break;
     case 1: strcat( result, "b" ); break;
     case 2: strcat( result, "c" ); break;
     case 3: strcat( result, "d" ); break;
   }

   sprintf(ans, "%i", num );
   strcat( result, ans );
   return result;
}

uint32 ATA_Driver::extended_partitions(
		ATANode *ata,
		uint32 lba_sector, 
		int dev,
		int offset
		)
{
	uint32 j,i;
	struct partition_descriptor pd;
	char temp[512];

	if ( ata->read( lba_sector, temp, 512 ) != 512 )
	   return 0;


	i = offset;
	for ( j = 446; j < 495; j+= 16)
	{
		pd = *((struct partition_descriptor*) & (temp[j]));

		if ( pd.size > 0 )
		{
		  PartitionNode *part = new
		    PartitionNode( pd.start_sector + lba_sector,
		    		   pd.start_sector + lba_sector + pd.size,
				   part_name( dev, i ),
				   ata->GetBase(),
				   ata->GetDevice() );

		   part->Resume();


//			   printf("%s%x%s%x\n",
//			     "There's an extended partition at: ",
//			     pd.start_sector + lba_sector,
//			     " until ",
//			     pd.start_sector + lba_sector + pd.size );

//				  pd.start_sector + lba_sector,
//				  pd.start_sector + lba_sector + pd.size );
		}
		i++;
	}


	return 0;
}


uint32 ATA_Driver::primary_partitions( ATANode* ata, int dev )
{
	int i,j;
	uint32 p;
	struct partition_descriptor pd;
	char temp[512];

	if ( ata->read( 0, temp, 512 ) != 512 ) return 0;

	i = 0;
	for ( j = 446; j < 495; j+= 16)
	{
		pd = *((struct partition_descriptor*) (& (temp[j])));
		if ( pd.size > 0 )
		{
			if ( pd.type != 0x5 )
			{
			  PartitionNode *part = new
			    PartitionNode( pd.start_sector,
			    		   pd.start_sector + pd.size,
					   part_name( dev, i ),
					   ata->GetBase(),
					   ata->GetDevice() );

			   part->Resume();

			/*
			   printf("%s%x%s%x\n",
			     "There's a partition at: ",
			     pd.start_sector,
			     " until ",
			     pd.start_sector + pd.size );
			*/
			}
			else
			{
			  p = extended_partitions( ata, 
			  			   pd.start_sector, 
						   dev,
			  			   5
			  			   );
			  if ( p != 0 ) return p;
			}
		}
		i++;
	}
	return 0;
}




unsigned int ATA_Driver::PartitionSupport()
{

   for ( int i = 0; i < 4; i++ )
    if ( nodes[i] != NULL )
     {
       ATANode *ata = nodes[i];
       primary_partitions( ata, i );
     }

  return 0;
}


