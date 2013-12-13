#include <kernel.h>
#include <stdio.h>
#include <spoon/base/IRQHandler.h>


/** This is the application-wide IRQ stub which is called by the kernel
 * when an IRQ event occurs. Fortunately, you don't have to worry about
 * this. You just need to implement the virtual IRQ method in your
 * class. 
 *
 *
 * \return the return value of the IRQ() method.
 */
int __irq( int irq, void *data )
{
  return ((IRQHandler*)data)->IRQ( irq ); 
}




/** This method requests the ability to handle the given IRQ from the
 * system.
 *
 * \param irq The IRQ number that your object wishes to handle.
 * \return 0 if successful.
 */
int IRQHandler::RequestIRQ( int irq )
{
  char name[32];
  sprintf( name, "%s%i", "__irq", irq );
	
  return smk_request_irq(  __irq, irq, name, this );
}


/** Indicates to the system that you no longer wish to handle the
 * specified IRQ. Note that you have to have requested the IRQ
 * previously, of course.
 * 
 * \param irq The IRQ that your object no longer wishes to handle.
 * \return 0 if successful.
 */
int IRQHandler::ReleaseIRQ( int irq )
{
  return smk_release_irq( irq );
}




/** The virtual method that your classes should override. This method is
 * called by the __irq stub whenever a system IRQ event occurs. 
 *
 * The default implementation returns IRQ_NOT_HANDLED, as it should.
 * 
 * \param irq The IRQ which occurred is passed to the method in this parameter.
 * 
 * \return IRQ_HANDLED if your IRQ handler successfully handled the IRQ.
 * or IRQ_NOT_HANDLED if your IRQ handler did not successfully handle the
 * IRQ or it was not intended for you.
 */

int IRQHandler::IRQ( int irq )
{
	return IRQ_NOT_HANDLED;
}


