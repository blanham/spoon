#ifndef _SPOON_BASE_IRQHANDLER_H
#define _SPOON_BASE_IRQHANDLER_H


#define	IRQ_HANDLED			0
#define IRQ_NOT_HANDLED		-1

/** 
 * \ingroup base
 * 
 * This class provides an easy way for an object to handle any IRQ
 * events in a system. 
 *
 * If you add this class into your object's inheritence list, then 
 * you can quite easily request, release and handle the events.
 *
 */


class IRQHandler
{
	public:
      int RequestIRQ( int irq );
      int ReleaseIRQ( int irq );

	private:
	 friend int __irq( int, void* ); /**<	function stub which calls the private IRQ() method. */
	  
      virtual int IRQ( int irq );
};

#endif

