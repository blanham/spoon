#include <types.h>
#include <stdio.h>

#include "ATAPINode.h"
#include "ATAPI_Driver.h"

#include "ataio.h"


// *********************************************

ATAPI_Driver::ATAPI_Driver()
: Application("driver.atapi")
{
}

ATAPI_Driver::~ATAPI_Driver()
{
}

// *********************************************

int ATAPI_Driver::Init()
{
   RequestIRQ( 0xE );
   RequestIRQ( 0xF );
	
   Scan();

   return 0;
}

void ATAPI_Driver::Shutdown()
{
   ReleaseIRQ( 0xE );
   ReleaseIRQ( 0xF );
}

// *********************************************

int ATAPI_Driver::IRQ( int irq )
{
  return 0;
}

// *********************************************


unsigned int ATAPI_Driver::Scan()
{

  for ( int i = 0; i < 4; i++ )
  {
    char name[64];
     sprintf(name,"%s%i","storage/atapi/disc",i);
     
     switch (i)
     {
       case 0: nodes[i] = new ATAPINode( name, BASE1, 0); break;
       case 1: nodes[i] = new ATAPINode( name, BASE1, 1); break;
       case 2: nodes[i] = new ATAPINode( name, BASE2, 0); break;
       case 3: nodes[i] = new ATAPINode( name, BASE2, 1); break;
     }

     if ( nodes[i]->Present() == false ) 
     {
       delete nodes[i];
       nodes[i] = NULL;
	   printf("%s%s\n","No detection for device node: ", name );
     }
     else 
	 {
		printf("%s%s\n","Starting device node: ", name );
		nodes[i]->Resume();
	 }

  }

   return 0;
}




