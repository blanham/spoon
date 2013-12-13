#include <spoon/sysmsg.h>
#include <spoon/base/Pulser.h>

Pulser::Pulser( Looper* target, unsigned int milliseconds )
: Thread( "_pulser" )
{
	m_milliseconds = milliseconds;
	m_target = target;
}


/** 
 *
 * \warning Do not forget to Terminate() the Pulser first. You
 * have to tell it to quit. Worst case, you'll be waiting 1 second.
 */
Pulser::~Pulser()
{
}


unsigned int Pulser::setRate( unsigned int milliseconds )
{
	unsigned int old = m_milliseconds;
	m_milliseconds = milliseconds;
	return old;
}

unsigned int Pulser::getRate()
{
	return m_milliseconds;
}

		
int Pulser::Run()
{
		
	while ( Terminated() == false )
	{
		int seconds = m_milliseconds / 1000;

		// Sigh. Worse case scenario, you have to wait 1 second for a 
		// pulser to die.
		for ( int i = 0; i < seconds; i++ )
		{
			// In case the Pulser was changed in the middle of sleeping.
			int new_seconds = m_milliseconds / 1000;
			if ( new_seconds != seconds ) break;

			if (Terminated()) return 0;
			smk_sleep( 1 );
		}


		int milli = m_milliseconds % 1000;
		if ( milli != 0 )	smk_delay( milli );

		if ( m_target != NULL ) m_target->PostPulse( PULSE );
	}

	return 0;
}


