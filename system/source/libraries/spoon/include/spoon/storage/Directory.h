#ifndef _SPOON_STORAGE_DIRECTORY_H
#define _SPOON_STORAGE_DIRECTORY_H

#include <string.h>
#include <spoon/ipc/Message.h>

/** \ingroup storage
 *
 *
 */
class Directory
{

  public:
    Directory( const char *node );
    virtual ~Directory();

    bool Set( const char *node );
    bool Exists();
    void Rewind();
    int CountEntries();
    char *NextEntry();

	static int Create( const char *node );
	static int Delete( const char *node );
	
  private:
    char *_node;
	char *m_entry;
	
    Message *_list;
    int _pos;
    int _count;


    

};

#endif

