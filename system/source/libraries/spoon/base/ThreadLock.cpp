#include <kernel.h>
#include <spoon/base/ThreadLock.h>


ThreadLock::ThreadLock()
{
	m_atomic_lock = 0;
	m_count = 0;
	m_tid = 0;
}

ThreadLock::~ThreadLock()
{
}
   
int ThreadLock::lock()
{
	bool gotit = false;

	while ( gotit == false )
	{
		smk_acquire_spinlock( &m_atomic_lock );

		if ( m_count == 0 )
		{
			m_count += 1;
			m_tid = smk_gettid();
			gotit = true;
		}
		else
		{
			if ( m_tid == smk_gettid() )
			{
				m_count += 1;
				gotit = true;
			}
		}
			
		smk_release_spinlock( &m_atomic_lock );

		if ( gotit == false ) smk_release_timeslice();
	}

	return 0;
}

int ThreadLock::unlock()
{
	smk_acquire_spinlock( &m_atomic_lock );

		m_count -= 1;

	smk_release_spinlock( &m_atomic_lock );
	return 0;
}



