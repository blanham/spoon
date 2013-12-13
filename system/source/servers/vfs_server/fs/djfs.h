#ifndef _FS_DJFS_H
#define _FS_DJFS_H

#include "FileSystem.h"
#include "djfs_defs.h"
#include <spoon/collections/List.h>
#include <spoon/collections/HashTable.h>

extern FileSystem *create_djfs( VFS::Device *dev );

// Predefinitions.

class DJFSRoot;
class DJFSDiskManager;
class DJFSFileManager;
class DJFSFile;
class DJFSTable;
class fs_djfs;

/** A DJFSRoot class to abstract a root system.
 */
class DJFSRoot
{
	public:
		DJFSRoot( fs_djfs *parent, DJFSDiskManager* manager, int location );
		~DJFSRoot();


		DJFSFile *getFile( const char *name );
		void releaseFile( DJFSFile *file );

		int deleteFile( const char *name );
		int createFile( const char *name );

	private:
		fs_djfs *m_parent;
		DJFSDiskManager *m_manager;
		DJFSFileManager *m_filemanager;
		int m_location;



		DJFSFile* searchDirectory( char *name, char *fullName, DJFSFile *file );
		int pruneNode( char *node, char **parent, char **leaf );
};

/** A DJFSDiskManager to abstract an actual device
 */
class DJFSDiskManager
{
	public:
		DJFSDiskManager( fs_djfs *parent, int total, int location );
		~DJFSDiskManager();

		int get( int owner );
		int release( int location );

		int save();
		int load();

	private:
		fs_djfs *m_parent;
		int *m_blocks;
		int m_total;
		int m_location;
};


/** A DJFSFile class to make my life easier and abstract all the 
 * important things.
 */

class DJFSFile
{
	public:
		DJFSFile( fs_djfs *parent, DJFSDiskManager *manager, int location );
		DJFSFile( fs_djfs *parent, DJFSDiskManager *manager, int location, const char *filename );


		
		~DJFSFile();

		int read( int64 position, void *buffer, int len );
		int write( int64 position, void *buffer, int len );

		int setSize( int new_size );
		int size();

		bool setDirectory();
		bool isDirectory();

		int save();
		int load();

		int getLocation();
	
	private:
		fs_djfs *m_parent;
		DJFSDiskManager *m_manager;
		List m_tables;
		int m_location;
		struct djfs_file m_file;

		int m_dirty;
};

/** A DJFSTable class to make my table life easier.
 */
class DJFSTable
{
	public:
		DJFSTable( fs_djfs *parent, int location );
		~DJFSTable();

		int getLocation();
		
		int get( int i );
		int set( int i, int location );

		int save();
		int load();

		int next();
		int setNext( int i );

	private:
		fs_djfs *m_parent;
		struct djfs_table_block m_table;
		int m_location;
};


struct djfs_fm
{
	char *node;
	DJFSFile *file;
	int usage;
};

/** The DJFSFileManager to maintain open files.
 */
class DJFSFileManager : public HashTable
{
	public:
		DJFSFileManager();
		~DJFSFileManager();

		bool addFile( const char *node, DJFSFile* file );
		DJFSFile *getFile( const char *node );
		bool releaseFile( DJFSFile* file );

		bool fileOpen( const char *node );
};

/** The DJFS FileSystem class proper.
 */


class fs_djfs : public FileSystem
{
   public:
     fs_djfs( VFS::Device *dev );
     virtual ~fs_djfs();
     

	virtual int Init(); 
	virtual int Shutdown();

	virtual int Test();


	virtual int   Create( char *node );
	
   	virtual void *Open( const char *node ); 
	virtual int   Read( int64 pos, void *buffer, int len, void *data );
	virtual int   Write( int64 pos, void *buffer, int len, void *data );
	virtual int   Delete( char *node );

	
	virtual void  Close( void *data );

	virtual int Stat( struct stat *st, void *data );
	virtual int List( char *node, Message **msg );


	int MakeDirectory( char *node );
	int DeleteDirectory( char *node );





   private:
	struct djfs_superblock *getSuperblock();
   	struct djfs_superblock pvd;

	// ..............

	DJFSRoot *m_root;
	DJFSDiskManager *m_manager;
	
	// Friends
	

	friend class DJFSRoot;
	friend class DJFSDiskManager;
	friend class DJFSFile;
	friend class DJFSTable;
	
};



#endif


