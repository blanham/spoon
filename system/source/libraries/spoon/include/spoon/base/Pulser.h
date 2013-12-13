#ifndef _SPOON_BASE_PULSER_H
#define _SPOON_BASE_PULSER_H

#include <spoon/base/Thread.h>
#include <spoon/ipc/Looper.h>

/** \ingroup base
 *
 */
class Pulser : public Thread
{
	public:
		Pulser( Looper* target, unsigned int milliseconds );
		virtual ~Pulser();

		unsigned int setRate( unsigned int milliseconds );
		unsigned int getRate();

		virtual int Run();
		
	private:
		volatile unsigned int m_milliseconds;
		Looper *m_target;
};

#endif

