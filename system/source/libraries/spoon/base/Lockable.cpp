#include <kernel.h>
#include <spoon/base/Lockable.h>


Lockable::Lockable() 
{
	m_atomic_lock = 0;
}

Lockable::~Lockable() 
{
}
   
int Lockable::lock()
{
	smk_acquire_spinlock( & m_atomic_lock );
	return 0;
}
      
int Lockable::unlock()
{
	smk_release_spinlock( & m_atomic_lock );
	return 0;
}
      

