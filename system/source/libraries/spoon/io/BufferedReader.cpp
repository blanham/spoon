#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <spoon/io/BufferedReader.h>



BufferedReader::BufferedReader( IOStream *stream, const char *tokens, int token_count )
{
	m_stream = stream;

	if ( token_count < 0 )
			m_tokenCount = strlen( tokens );
	else	
			m_tokenCount = token_count;

	m_tokens = (char*)malloc( m_tokenCount );

	memcpy( m_tokens, tokens, m_tokenCount );

	m_buffer = NULL;
	m_bufferSize = 0;
	m_bytesStored = 0;
	m_lastPos = 0;
}

BufferedReader::~BufferedReader()
{
	if ( m_buffer != NULL ) free( m_buffer );
	if ( m_tokens != NULL ) free( m_tokens );

	m_stream->Close();
	delete m_stream;
}



const char *BufferedReader::getTokens()
{
	return (const char*)m_tokens;
}

void BufferedReader::setTokens( const char *tokens )
{
	if ( m_tokens != NULL ) free( m_tokens );
	m_tokens = strdup( tokens );
}
	   
IOStream *BufferedReader::getStream()
{
	return m_stream;
}


const char *BufferedReader::readLine( int *len, char *token )
{
	// First see if we need to move all the data to the start again.
	if ( m_lastPos != 0 )
	{
		if ( m_bytesStored != 0 )
			memcpy( m_buffer, (void*)((uintptr_t)m_buffer + m_lastPos), m_bytesStored );
		m_lastPos = 0;
	}


	// The stream is no longer valid with no data.
	if ( (m_stream->isValid() == false) && (m_bytesStored == 0) )
	{
		if ( len != NULL ) *len = -1;
		return NULL;
	}


	// Now go into a loop until we have a real line.
	
	bool hasToken = false;

	
	while ( hasToken == false )
	{

		// Immediately scan the buffer for any tokens left over.
			for ( int i = 0; i < m_bytesStored; i++ )
			{
				for ( int j = 0; j < m_tokenCount; j++ )
					if ( m_buffer[i] == m_tokens[j] ) 
					{
						hasToken = true;
						m_lastPos = i + 1;
						if ( token != NULL ) *token = m_buffer[ i ];
						m_buffer[ i ] = 0;
						break;
					}
	
				if ( hasToken == true ) break;
			}
	
			if ( hasToken == true ) break;					// Yay! data
			if ( m_stream->isValid() == false ) break;		// Yay! invalid.
			

			// Make sure we have some buffer to read into.
			if ( m_bufferSize < ( m_bytesStored + BUFFEREDREADER_BUFSIZE ) )
			{
				m_bufferSize = m_bufferSize + BUFFEREDREADER_BUFSIZE;
		
				m_buffer = (char*)realloc( m_buffer, m_bufferSize );
				if ( m_buffer == NULL ) 
				{
					/// \todo This should really be an application-crash scenario.
					m_bufferSize = 0;
					m_bytesStored =  0;
					if ( len   != NULL )   *len = -1;
					if ( token != NULL ) *token = 0;
					return NULL;
				}
			}
			// ... tada 


			// Read in data.
			
			int rc = m_stream->Read( (void*)(m_buffer + m_bytesStored), BUFFEREDREADER_BUFSIZE - 1 );
			if ( rc <= 0 ) break;
			
			m_bytesStored += rc;  // Update the count.
	}
	

	if ( m_lastPos == 0 )		// Read returned error or 0 bytes.
	{
		if ( m_stream->isValid() == true )	// Okay. It was a timeout. So, readLine shouldn't honour it's call.
		{
			m_buffer[ m_bytesStored ] = 0;
			if ( len != NULL ) *len = 0;
			if ( token != NULL ) *token = 0;
			return NULL;
		}
		else
		{
			// The stream went bad. Return whatever was left.
			m_buffer[ m_bytesStored ] = 0;
			if ( len   != NULL ) *len   = m_bytesStored;
			if ( token != NULL ) *token = 0;
			m_bytesStored = 0;
		}
	}
	else
	{
		if ( len != NULL ) *len = m_lastPos - 1;
		m_bytesStored = m_bytesStored - m_lastPos;
	}
	
	
	return m_buffer;
}


	   

