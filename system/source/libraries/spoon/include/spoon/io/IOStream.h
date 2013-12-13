#ifndef _SPOON_IO_IOSTREAM_H
#define _SPOON_IO_IOSTREAM_H


/** \ingroup io
 *
 *  The IOStream provides the basic operations on an IO source: Open,
 * Read, Write, Close, etc..
 *
 * This is the base class for several IO sources so that other classes,
 * such as the BufferedReader, can be used on multiple inherited classes
 * and not have to worry. 
 *
 * It really makes life easier.
 */

class IOStream
{
	public:
		IOStream();
		virtual ~IOStream();

		virtual int Open();
		virtual int Write(const void *buf, int len );
		virtual int Read(void *buf, int len );
		virtual int Close();

		virtual int bytesWaiting();
		virtual bool isValid();

	protected:
		virtual void Invalidate();


	private:
		bool  m_valid;
};

#endif

