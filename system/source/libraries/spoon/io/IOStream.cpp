#include <spoon/io/IOStream.h>



/** Apon construction, the IOStream is always valid. It is valid
 * until Invalidate is called.
 */
IOStream::IOStream()
{
	m_valid = true;
}

IOStream::~IOStream()
{
}


/** This should return an integer greater than or
 * equal to zero.
 */
int IOStream::Open()
{
	return -1;
}

int IOStream::Write(const void *buf, int len )
{
	return -1;
}

int IOStream::Read(void *buf, int len )
{
	return -1;
}

int IOStream::Close()
{
	return -1;
}


int IOStream::bytesWaiting()
{
	return 0;
}



bool IOStream::isValid()
{
	return m_valid;
}



void IOStream::Invalidate()
{
	m_valid = false;
}



