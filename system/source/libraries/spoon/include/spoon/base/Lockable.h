#ifndef _SPOON_BASE_LOCKABLE_H
#define _SPOON_BASE_LOCKABLE_H

#include <kernel.h>


/** \addtogroup base
 *
 * Quite a simple class which wraps an atomic lock around the
 * class which inherits it. Once that's done, you can lock()
 * and unlock() at will.
 */

class Lockable
{
   public:
      Lockable();
      virtual ~Lockable();
   
      virtual int lock();
      virtual int unlock();

   private:
      unsigned int m_atomic_lock;

      
};

#endif

