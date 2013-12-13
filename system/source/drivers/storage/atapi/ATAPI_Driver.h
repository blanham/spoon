#ifndef _ATAPI_Driver_H
#define _ATAPI_Driver_H

#include <spoon/app/Application.h>
#include <spoon/base/IRQHandler.h>

class ATAPINode;

class ATAPI_Driver : public Application, public IRQHandler
{
    public:
      ATAPI_Driver();
      ~ATAPI_Driver();
      
      
      int Init();
      int IRQ( int irq );
      void Shutdown();


    private:
    	unsigned int Scan();
	
	ATAPINode *nodes[4];

};

#endif


