#ifndef _SPOON_IO_BUFFEREDREADER_H
#define _SPOON_IO_BUFFEREDREADER_H


#include <spoon/io/IOStream.h>


#define BUFFEREDREADER_BUFSIZE		16384

/** \ingroup io
 * Once you have created a BufferedReader with an IOStream,
 * the IOStream is no longer yours. You might as well forget about it.
 * The BufferedReader will close it and delete it for you when it
 * is deleted.
 */
class BufferedReader
{
	public:
		BufferedReader( IOStream *stream, const char *tokens = "\r\n", int token_count = -1 );
	   ~BufferedReader();


	   const char *getTokens();
	   void setTokens( const char *tokens );
	   
	   IOStream *getStream();

	   const char *readLine( int *len = NULL, char *token = NULL );


	private:
	   IOStream *m_stream;
	   char *m_tokens;
	   int	 m_tokenCount;
	   
	   char *m_buffer;
	   int   m_bufferSize;
	   int 	 m_bytesStored;
	   int   m_lastPos;
	   
};



#endif

