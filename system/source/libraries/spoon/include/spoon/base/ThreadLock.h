#ifndef _SPOON_BASE_THREADLOCK_H
#define _SPOON_BASE_THREADLOCK_H

#include <spoon/base/Lockable.h>

/** \addtogroup base
 *
 *
 * The ThreadLock class is a locking mechanism that prevents
 * multiple threads from acquiring the lock at any one time.
 *
 * However! The same thread may acquire the lock numerous
 * times and release the lock numerous times.  The number
 * of acquires and releases must balance out.
 * 
 */

class ThreadLock : public Lockable
{
   public:
      ThreadLock();
      virtual ~ThreadLock();
   
      virtual int lock();
      virtual int unlock();

   private:
      unsigned int m_atomic_lock;
	  int m_tid;
	  int m_count;

      
};

#endif

