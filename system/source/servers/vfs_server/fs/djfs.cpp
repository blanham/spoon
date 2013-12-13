#include <types.h>
#include <stdio.h>

#include "djfs.h"
#include "djfs_defs.h"



// ---------------------------------------------------------------------

DJFSRoot::DJFSRoot( fs_djfs *parent, DJFSDiskManager *manager, int location )
{
	m_parent = parent;
	m_manager = manager;
	m_location = location;
	m_filemanager = new DJFSFileManager();
}

DJFSRoot::~DJFSRoot()
{
	delete m_filemanager;
}


int DJFSRoot::deleteFile( const char *name )
{
	char *parent;
	char *leaf;

	if ( pruneNode( (char*)name, &parent, &leaf ) != 0 )
	{
		printf("%s\n","unable to pruneNode");
		return -1;
	}

	// Is it open by someone else?
	if ( m_filemanager->fileOpen( name ) == true ) 
	{
		printf("%s%s\n", name, " already open. Unable to delete." );
		return -1;
	}

	DJFSFile *file = getFile( name );
	if ( file == NULL )
	{
		free( parent );
		free( leaf );
		return -1;
	}
	
	DJFSFile *parentFile = getFile( parent );
	if ( parentFile == NULL )
	{
		// Very very odd...
		printf("%s%s\n","Very odd thing occurred looking for: ", parent );
		free( parent );
		free( leaf );
		releaseFile( file );
		return -1;
	}	

	// Remove from the parent directory...

		struct djfs_entry *list = (struct djfs_entry*) malloc( parentFile->size() );
		int count = (parentFile->size() / ENTRY_SIZE);

		if ( parentFile->read( 0, list, parentFile->size() ) != parentFile->size() )
		{
			printf("%s\n","Unable to fileRead in deleteFile");
			free( list );
			free( parent );
			free( leaf );
			releaseFile( parentFile );
			releaseFile( file );
			return -1;
		}

		// Valid information. Time to scan the directory and remove the entry. 

		int found = 0;
		for ( int i = 0; i < count; i++ )
		{

			if ( found == 1 )
			{
				if ( i != 0 )
				{
					strcpy( list[i - 1].filename, list[i].filename );
					list[i - 1].location = list[i].location;
				}

				continue;
			}
			
			
			if ( strcmp( list[i].filename, leaf ) == 0 ) found = 1;
		}
	
		parentFile->setSize( parentFile->size() - sizeof( struct djfs_entry ) );

		// Write the new file.
		
		if ( parentFile->write( 0, list, parentFile->size() ) != parentFile->size() )
		{
			printf("%s\n","Unable to fileWrite in deleteFile");
			free( list );
			free( parent );
			free( leaf );
			releaseFile( parentFile );
			releaseFile( file );
			return -1;
		}

	
		free( list );
		
	// Remove the actual file.
	
	file->setSize(0);							// Zero size..
	int local = file->getLocation();
	releaseFile( file );
	m_manager->release( local );			 	// Zero record...
	
	free( parent );
	free( leaf );
	releaseFile( parentFile );

	return 0;
}


int DJFSRoot::createFile( const char *name )
{
	char *parent;
	char *leaf;

	if ( pruneNode( (char*)name, &parent, &leaf ) != 0 )
	{
		printf("%s\n","unable to pruneNode");
		return -1;
	}

	// Does it already exist?
	
	DJFSFile *check = getFile( name );
	if ( check != NULL )
	{
		printf("%s%s\n", name, " already exists." );
		releaseFile( check );
		return -1;
	}
	

	// ------
	DJFSFile *parentFile = getFile( parent );
	if ( parentFile == NULL )
	{
		// Very very odd...
		printf("%s%s\n","Unable to locate parent: ", parent );
		free( parent );
		free( leaf );
		return -1;
	}	

	// create an entry...
	struct djfs_entry entry;

	strncpy( entry.filename, leaf, 256 );
	entry.location = m_manager->get( -1 );
	if ( entry.location < 0 )
	{
		printf("%s\n","Unable to allocate file block");

		// Disk full!
		free( parent );
		free( leaf );
		releaseFile( parentFile );
		return -1;
	}


	// ----------------------------------
	
	// Attempt to make the real file on disk.
	DJFSFile *file = new DJFSFile( m_parent, m_manager, entry.location, leaf );
		if (  file->save() != 0 )
		{

			printf("%s\n","unable to save file");
			// DISK ERROR!
			m_manager->release( entry.location );
			free( leaf );
			free( parent );
			releaseFile( parentFile );
			delete file;
			return -1;
		}
	   delete file;


	// Now let the parent know that it exists.

	if ( parentFile->write( parentFile->size(), &entry, sizeof( struct djfs_entry ) ) !=
						sizeof( struct djfs_entry ) )
	{
		// Disk error or full!
		free( parent );
		free( leaf );
		releaseFile( parentFile );
		return -1;
	}

	releaseFile( parentFile );
	
	free( leaf );
	free( parent );
	return 0;
}


DJFSFile *DJFSRoot::getFile( const char *name )
{
    int prev = 0;
    int last = 0;
	
	// First, handle the root node.
	if ( (name == NULL) || (strcmp(name,"/") == 0) || (strcmp(name,"") == 0) ) 
	{
		if ( m_filemanager->fileOpen( "/" ) == true )
			return m_filemanager->getFile( "/" );

		// Open the root...
		DJFSFile *next_file = new DJFSFile( m_parent, m_manager, m_location );
				  next_file->load();
		m_filemanager->addFile( "/", next_file );
		return next_file;
	}
	
	
	// First, see if it's already open....
	if ( m_filemanager->fileOpen( name ) == true )
	{
		return m_filemanager->getFile( name );
	}
	
	// Load the root directory.
	DJFSFile *next_file = m_filemanager->getFile( "/" );
			if ( next_file == NULL )
			{
				next_file = new DJFSFile( m_parent, m_manager, m_location );
				next_file->load();
				m_filemanager->addFile( "/", next_file );
			}
				
	
	

	// Find the last "/" in the stream.
	for ( int i = strlen(name)-1; i > 0; i-- )
		if ( name[i] == '/' ) 
		{
			last = i+1;
			break;
		}


    for ( unsigned int i = 0; i <= strlen(name); i++ )
    {
      if ( (name[i] == '/') || (i == strlen(name)) ) 
      {
		// Try and locate it.
			char *fullName = (char*)strndup( name, i );
    	   	char *dn = (char*)strndup( name + prev, i - prev );
			DJFSFile *tmp = searchDirectory( dn, fullName, next_file );
			free(dn);
			free( fullName );

			m_filemanager->releaseFile( next_file );
			

			if ( tmp == NULL ) return NULL; // unable to find the requested file
			if ( prev == last ) return tmp;	// This is the file requested!

			// Prepare for next round.
			next_file = tmp;
			
			if ( next_file->isDirectory() == false  ) break; // This isn't a directory. Can't go deeper.

		prev = i + 1;	// go to next dir in path
	  }
    }

	m_filemanager->releaseFile( next_file );
	return NULL;
}


void DJFSRoot::releaseFile( DJFSFile *file )
{
	m_filemanager->releaseFile( file );
}


/** Reads the contents of a File, as an entry list, and searches for 
 * the requested name.
 */

DJFSFile* DJFSRoot::searchDirectory( char *name, char *fullName, DJFSFile *file )
{
	if ( file->size() == 0 ) return NULL;	// Empty directory. No point.
	

	struct djfs_entry *list = (struct djfs_entry*) malloc( file->size() );
	int count = (file->size() / ENTRY_SIZE);


	if ( file->read( 0, list, file->size() ) != file->size() )
	{
		printf("%s\n","Unable to fileRead in searchDirectory");
		free(list);
		return NULL;
	}

	// Valid information. Time to scan the directory. 

	for ( int i = 0; i < count; i++ )
	{
		if ( strcmp( list[i].filename, name ) != 0 ) continue;

		// This directory matches. See if it's already open.
		DJFSFile *result = m_filemanager->getFile( fullName );
		if ( result != NULL ) return result;
		
		// We have the requested file. Now we open it!
		result = new DJFSFile( m_parent, m_manager, list[i].location );
		if (  result->load() != 0 )
		{
			printf("%s\n","Unable to load the file specified in the directory.!");
			delete result;
			result = NULL;
		}
	
		m_filemanager->addFile( fullName, result );
		
		free( list );
		return result;
	}
	
	free( list );
	return NULL; 
}




int	DJFSRoot::pruneNode( char *node, char **parent, char **leaf )
{
	*parent = NULL;
	*leaf   = NULL;
		
	if ( node == NULL ) return -1;
	if ( strcmp(node,"") == 0 ) return -1;
	
	
	int len = strlen(node);
	int found = 0;
	
	for ( int i = (len - 1); i >= 0; i-- )
	{
		if ( (node[i] == '/') && (i != 0) && (i != (len-1)) )
		{
			*parent = strdup(node);
			(*parent)[i] = 0;
			*leaf = strdup(((*parent) + i + 1));
			found = 1;
			break;
		}
	}

	if ( found == 0 )
	{
		*parent = strdup("/");
		*leaf   = strdup( node );
	}

	//printf("%s: %s  %s\n", node, *parent, *leaf );
	
	return 0;
}




// ---------------------------------------------------------------------



DJFSDiskManager::DJFSDiskManager( fs_djfs *parent, int total, int location )
{
	m_parent = parent;
	m_total = total;
	m_location = location;

	m_blocks = (int*)malloc( sizeof(int) * m_total );

	for ( int i = 0; i < m_total; i++ )
		m_blocks[i] = -1;
}

DJFSDiskManager::~DJFSDiskManager()
{
	free( m_blocks );
}


int DJFSDiskManager::get( int owner )
{
	static int last_one = 0;

	for ( int i = 0; i < m_total; i++ )
	{
		int position = (last_one + i) % m_total;
			
		if ( m_blocks[position] == -1 )
		{
			if ( owner == -1 ) m_blocks[position] = position;
						else   m_blocks[position] = owner;

			last_one = (position + 1) % m_total;
			return position;
		}
			
	}

	return -1;
}

int DJFSDiskManager::release( int location )
{
	if ( location < 0 ) return -1;
	if ( location >= m_total ) return -1;
	m_blocks[location] = -1;
	return 0;
}


int DJFSDiskManager::save()
{
	int len = sizeof(int) * m_total;
	if ( m_parent->dev_write( m_location * SECTOR_SIZE, m_blocks,  len ) != len ) return -1;
	return  0;	
}

int DJFSDiskManager::load()
{
	int len = sizeof(int) * m_total;
	if ( m_parent->dev_read( m_location * SECTOR_SIZE, m_blocks, len ) != len ) return -1;
	return  0;	
}

// ---------------------------------------------------------------------

/** A DJFSFile class to make my life easier and abstract all the 
 * important things.
 */

DJFSFile::DJFSFile( fs_djfs *parent, DJFSDiskManager *manager, int location )
{
	m_parent = parent;
	m_manager = manager;
	m_location = location;
	m_dirty = 0;
}


DJFSFile::DJFSFile( fs_djfs *parent, DJFSDiskManager *manager, int location, const char *filename )
{
	m_parent = parent;
	m_manager = manager;
	m_location = location;

	strcpy( m_file.filename, filename );
	m_file.table = -1;
	m_file.size = 0;
	m_file.location = location;
	m_file.flags = 0;
	m_dirty = 1;
}


DJFSFile::~DJFSFile()
{
	while ( m_tables.count() > 0 )
	{
		DJFSTable *table = (DJFSTable*)m_tables.get(0);
		m_tables.remove( table );
		delete table;
	}
}


int DJFSFile::read( int64 position, void *buffer, int len )
{
	// Ensure that it's valid.
	if ( position < 0 ) return -1;
	if ( len == 0 ) return 0;
	if ( len < 0 ) return -1;
	if ( position >= size() ) return -1;

	// Shrink it if necessary.
	if ( (position + len) > size() ) len = size() - position;
	

	// Read the information...
	char *data = (char*)buffer;
	int bytesRead = 0;
	

	// Work out the blocks we are working with.

	int block_start = (position / SECTOR_SIZE);					// starting sector.
	int block_end = ((position+len-1) / SECTOR_SIZE) + 1;		// ending sector.

	
	// For each block...
	for ( int block = block_start; block < block_end; block++ )
	{
		int table_num = ( block / TABLE_ENTRIES );	// Inside table #
		int block_num = ( block % TABLE_ENTRIES );  // Position in table.

		DJFSTable *table = (DJFSTable*)m_tables.get( table_num ); 

			int block_location = table->get( block_num ); // Location of sector.
			if ( block_location == -1 )
			{
				/// \todo Blocks should actually have been allocated during setSize!
				int new_loc = m_manager->get( m_location );
				if ( new_loc < 0 )
				{
					/// \todo problem!! Freak out.
					break;
				}

				// zero out the memory ---------.
				char tmpData[ SECTOR_SIZE ];
				for ( int i = 0; i < SECTOR_SIZE; i++ ) tmpData[i] = 0;
				m_parent->dev_write( new_loc * SECTOR_SIZE, tmpData, SECTOR_SIZE );	// Save it to disk.

				// Record the new block.
				table->set( block_num, new_loc );
				block_location = new_loc;
			}
		
			
		int64 pos_start = block * SECTOR_SIZE;
		int64 pos_end   = (block + 1) * SECTOR_SIZE;
							// makes sense.
		
		if ( pos_start < position ) pos_start = position;
		if ( pos_end > (position + len) ) pos_end = position + len;

		// Work out how much work it has to do.
		int64 difference = pos_end - pos_start;
		int64 offset = pos_start % SECTOR_SIZE;

		if ( m_parent->dev_read( ( block_location * SECTOR_SIZE + offset ), 
								   ( data + bytesRead ), 
								   difference ) != difference ) break;	// Unable to read.

		bytesRead += difference;
	}
	
	// And return...
	return bytesRead;
}


int DJFSFile::write( int64 position, void *buffer, int len )
{
	// Ensure that it's valid.
	if ( position < 0 ) return -1;
	if ( len == 0 ) return 0;
	if ( len < 0 ) return -1;
	
	// Make sure the file is big enough.
	int new_size = (int)position + len; 
	
	if ( new_size > size() )
		if ( setSize( new_size ) != 0 )
		{
			// FileSystem must be full.
			return -1;
		}
	

	// This file is now dirty...
	m_dirty = 1;

	// Write the information...
	
	char *data = (char*)buffer;
	int bytesWritten = 0;
	

	// Work out the blocks we are working with.

	int block_start = (position / SECTOR_SIZE);
	int block_end = ((position+len-1) / SECTOR_SIZE) + 1;

	
	for ( int block = block_start; block < block_end; block++ )
	{
		int table_num = ( block / TABLE_ENTRIES );
		int block_num = ( block % TABLE_ENTRIES );

		DJFSTable *table = (DJFSTable*)m_tables.get( table_num ); 

			int block_location = table->get( block_num );
			if ( block_location == -1 )
			{
				int new_loc = m_manager->get( m_location );
				if ( new_loc < 0 )
				{
					/// \todo These should be allocated in setSize.
					break;
				}
				
				table->set( block_num, new_loc );
				block_location = new_loc;
			}
		
			
		int64 pos_start = block * SECTOR_SIZE;
		int64 pos_end   = (block + 1) * SECTOR_SIZE;
		
		if ( pos_start < position ) pos_start = position;
		if ( pos_end > (position + len) ) pos_end = position + len;

		// How much work to do?
		int64 difference = pos_end - pos_start;
		int64 offset = pos_start % SECTOR_SIZE;

		if ( m_parent->dev_write( ( block_location * SECTOR_SIZE + offset ), 
								   ( data + bytesWritten ), 
								   difference ) != difference ) break;

		bytesWritten += difference;
	}
	
	return bytesWritten;
}


int DJFSFile::setSize( int new_size )
{
	if ( new_size < 0 ) return -1;
	if ( size() == new_size ) return 0;

	// blocks needed....
	int blocks_needed = new_size / SECTOR_SIZE + 1;
	if ( (new_size % SECTOR_SIZE) == 0 ) blocks_needed -= 1; 

	// tables needed.....
	int tables_needed = blocks_needed / TABLE_ENTRIES + 1;
	if ( (blocks_needed % TABLE_ENTRIES) == 0 ) tables_needed -= 1;

	// Take note of the original amount of tables.
	
	int rollback_tables = m_tables.count();
	int rollback_event  = 0;
	
	// .................... now adjust as required.
	

	m_dirty = 1;
	
	if ( m_tables.count() == tables_needed ) // already good. yeah! 
	{
		m_file.size = new_size;
		return 0;
	}
	
	if ( m_tables.count() < tables_needed ) // too few.
	{
		while ( m_tables.count() < tables_needed )
		{
			int new_loc = m_manager->get( m_location ); 
			if ( new_loc < 0 ) 
			{
				// DISK FULL!
				// Rollback required!! Break out of it.
				rollback_event = 1;
				tables_needed = rollback_tables; // Prepare the next algorithm
				break;
			}
			
			DJFSTable *table = new DJFSTable( m_parent, new_loc );

			// If it's the very first one, take note in the file structure.
			if ( m_tables.count() == 0 )
			{
				m_file.table = new_loc;
			}
			else
			{
				DJFSTable *prev = (DJFSTable*)m_tables.get( m_tables.count() - 1 );
						   prev->setNext( new_loc );
			}

			m_tables.add( table );
		}


		if ( rollback_event == 0 ) // Success!!
		{
			m_file.size = new_size;
			return 0;
		}
	}

	
	
	// Need to drop a few. This is also a potential rollback from a failed growth. 
	while ( m_tables.count() > tables_needed )
	{
		// Remove the last one.
		DJFSTable *last = (DJFSTable*)m_tables.get( m_tables.count() - 1 );

		m_tables.remove( last );

		if ( m_tables.count() == 0 )
		{
			// If we have removed all of them, make note in the file structure.
			m_file.table = -1;
		}
		else
		{
			DJFSTable *prev = (DJFSTable*)m_tables.get( m_tables.count() - 1 );
					   prev->setNext(-1);
		}

			
		// Release everything held by the table.
			for ( int i = 0; i < TABLE_ENTRIES; i++ )
			{
				int old_loc = last->get(i); 
				if ( old_loc >= 0 )
				{
					m_manager->release( old_loc );
				}
			}
			// Finally, release the table proper and delete it...
			m_manager->release( last->getLocation() );
			delete last;
	}
	
	
	// ...................
	
	if ( rollback_event != 0 ) return -1; 	// As with my life, it too failed some time ago.

	m_file.size = new_size;
	return 0;
}

int DJFSFile::size()
{
	return m_file.size;
}

bool DJFSFile::setDirectory()
{
  m_file.flags |= DIRECTORY; 
  m_dirty = 1;
  return true;
}

bool DJFSFile::isDirectory()
{
	return ( (m_file.flags & DIRECTORY) == DIRECTORY );
}

int DJFSFile::save()
{
	if ( m_dirty == 0 ) return 0;
		
	if ( m_parent->dev_write( m_location * SECTOR_SIZE, &m_file, FILE_SIZE ) != FILE_SIZE ) return -1;


	for ( int i = 0; i < m_tables.count(); i++ )
	{
		DJFSTable *table = (DJFSTable*)m_tables.get(i);

		if ( table->save() != 0 )
		{
			printf("%s%i\n","WARNING: DJFSFile::save() unable to save: ", table->getLocation() );
			/// \todo freak out.
		}
	}
	
	
	m_dirty = 0;
	return  0;	
}
	
int DJFSFile::load()
{
	if ( m_parent->dev_read( m_location * SECTOR_SIZE, &m_file, FILE_SIZE ) != FILE_SIZE ) return -1;

	int location = m_file.table;

	while ( location != -1 )
	{
		DJFSTable *table = new DJFSTable( m_parent, location );

		if ( table->load() != 0 )
		{
			printf("%s%i\n","WARNING: DJFSFile::load() unable to load: ", location );
			/// \todo freak out.
		}

		m_tables.add( table );

		location = table->next();
	}
	
	
	m_dirty = 0;
	return  0;	
}



int DJFSFile::getLocation()
{
	return m_location;
}


// ---------------------------------------------------------------------

/** A DJFSTable class to make my table life easier.
 */
DJFSTable::DJFSTable( fs_djfs *parent, int location )
{
	m_parent = parent;
	m_location = location;

	m_table.next = -1;
	for ( int i = 0; i < TABLE_ENTRIES; i++ )
		m_table.list[i] = -1;
}

DJFSTable::~DJFSTable()
{
}

int DJFSTable::getLocation()
{
	return m_location;
}

int DJFSTable::get( int i )
{
	return m_table.list[i];
}

int DJFSTable::set( int i, int location )
{
	m_table.list[i] = location;
	return 0;
}

int DJFSTable::next()
{
	return m_table.next;
}

int DJFSTable::setNext( int next )
{
	m_table.next = next;
	return 0;
}

int DJFSTable::save()
{
	if ( m_parent->dev_write( m_location * SECTOR_SIZE, &m_table, TABLE_BLOCK_SIZE ) != TABLE_BLOCK_SIZE ) return -1;
	return  0;	
}

int DJFSTable::load()
{
	if ( m_parent->dev_read( m_location * SECTOR_SIZE, &m_table, TABLE_BLOCK_SIZE ) != TABLE_BLOCK_SIZE ) return -1;
	return  0;	
}


// *****************************************************

int djfs_hash( void *data )
{
	struct djfs_fm *fa = (struct djfs_fm*)data;

	if ( fa->node == NULL ) return 0;

	int key = 0;
	
	for ( unsigned int i = 0; i < strlen(fa->node); i++ )
		key = key * fa->node[i] + fa->node[i];

	return key;
}

int djfs_compare( void *a, void *b )
{
	struct djfs_fm *fa = (struct djfs_fm*)a;
	struct djfs_fm *fb = (struct djfs_fm*)b;

	if ( (fa->node == NULL) || (fb->node == NULL) )
	{
		if ( fa->file == fb->file ) return 0;
		return -1;
	}
	
	return strcmp( fa->node, fb->node );
}





DJFSFileManager::DJFSFileManager()
: HashTable( djfs_hash, djfs_compare )
{
}

DJFSFileManager::~DJFSFileManager()
{
	while ( count() > 0 )
	{
		struct djfs_fm *f = (struct djfs_fm*)get(0);
		remove(f);

		f->file->save();
		delete f->file;
		free( f->node );
		delete f ;
	}
}

bool DJFSFileManager::addFile( const char *node, DJFSFile* file )
{
	struct djfs_fm *tmp = new struct djfs_fm;
				    tmp->node = strdup(node);
					tmp->file = file;
					tmp->usage = 1;
	
	return add( tmp );
}

DJFSFile *DJFSFileManager::getFile( const char *node )
{
	struct djfs_fm tmp;
				   tmp.node = (char*)node;

	struct djfs_fm *ans = (struct djfs_fm*)retrieve( &tmp );
	if ( ans == NULL ) return NULL;

	ans->usage += 1;

	return ans->file;
}

bool DJFSFileManager::releaseFile( DJFSFile* file )
{
	struct djfs_fm tmp;
				   tmp.node = NULL;
				   tmp.file = file;

	struct djfs_fm *ans = (struct djfs_fm*)retrieve( &tmp );
	if ( ans == NULL ) return false;

	
	ans->usage -= 1;

	if ( ans->usage == 0 )
	{
		remove(ans);
		
		ans->file->save();
		delete ans->file;
		free( ans->node );
		delete ans;
	}
	
	
	return true;
}


bool DJFSFileManager::fileOpen( const char *node )
{
	struct djfs_fm tmp;
				   tmp.node = (char*)node;

	struct djfs_fm *ans = (struct djfs_fm*)retrieve( &tmp );
	if ( ans == NULL ) return false;

	return true;
}




// *****************************************************


FileSystem *create_djfs( VFS::Device *dev )
{
  return new fs_djfs( dev );
}


// *****************************************************

fs_djfs::fs_djfs( VFS::Device *dev )
: FileSystem( "djfs",
	          "djfs file system, simple and fat-like with a bit of ext2",
	           dev )
{
}

fs_djfs::~fs_djfs()
{
}

// ****************************************************



int fs_djfs::Init()
{
	if ( getSuperblock() == NULL ) return -1;

	int sectors = (sizeof(int) * pvd.total_blocks) / SECTOR_SIZE + 1;
	
	int root_sector = 1 + sectors;

		printf("%s%i,%i\n","SUPER_SIZE = ", SUPER_SIZE, sizeof(struct djfs_superblock));
		printf("%s%i,%i\n","FILE_SIZE = ", FILE_SIZE, sizeof(struct djfs_file));
		printf("%s%i,%i\n","TABLE_BLOCK_SIZE = ", TABLE_BLOCK_SIZE,  sizeof(struct djfs_table_block));
		printf("%s%i,%i\n","ENTRY_SIZE = ", ENTRY_SIZE, sizeof(struct djfs_entry));
		printf("%s%i\n",   "TABLE_ENTRIES = ", TABLE_ENTRIES );


	m_manager = new DJFSDiskManager( this, pvd.total_blocks, TABLE_SECTOR ); 
	m_manager->load();
	
	m_root = new DJFSRoot( this, m_manager, root_sector );
	
	return 0;
}

int fs_djfs::Shutdown()
{
	delete m_root;

	m_manager->save();
	delete m_manager;
	
	printf("%s\n", "fs_djfs::Shutdown() complete" );
	return 0;
}

int fs_djfs::Test()
{
	if ( getSuperblock() == NULL ) return FS_UNSUPPORTED;
	return FS_EXACT;
}


// ****************************************************

int fs_djfs::Create( char *node )
{
	return m_root->createFile( node );
}


// ****************************************************

void *fs_djfs::Open( const char *node )
{
	return m_root->getFile( node );
}

int   fs_djfs::Read( int64 pos, void *buffer, int len, void *data )
{
	DJFSFile *file = (DJFSFile*)data;
	return file->read( pos, buffer, len );
}

int   fs_djfs::Write( int64 pos, void *buffer, int len, void *data )
{
	DJFSFile *file = (DJFSFile*)data;
	return file->write( pos, buffer, len );
}

void  fs_djfs::Close( void *data )
{
	DJFSFile *file = (DJFSFile*)data;
	m_root->releaseFile( file );
}

// *****************************************************

int   fs_djfs::Delete( char *node )
{
	return m_root->deleteFile( node );
}




// ****************************************************

int fs_djfs::Stat( struct stat *st, void *data )
{
	DJFSFile* file = (DJFSFile*)data;
	
		st->st_dev  = GetDevice()->getDeviceID();
		st->st_size = file->size();
		st->st_mode = 0;

	if (file->isDirectory()) st->st_mode |= S_IFDIR;
	return 0;
}

int fs_djfs::List( char *node, Message **msg )
{
	*msg = NULL;
	
	DJFSFile *file = m_root->getFile( node );
	if ( file == NULL ) return -1; 
	

	if ( file->size() == 0 )
	{
		*msg = new Message(OK);
		m_root->releaseFile( file );
		return 0;
	}
	

	int count = (file->size()/ENTRY_SIZE);
	struct djfs_entry *list = (struct djfs_entry*)malloc(file->size());

		if ( file->read( 0, list, file->size() ) != file->size() )
		{
			m_root->releaseFile( file );
			printf("%s\n","Unable to fileRead");
			return -1;
		}

		*msg = new Message(OK);
		for ( int i = 0 ; i < count; i++ )
		{
			(*msg)->AddString("entry", list[i].filename );
		}

	m_root->releaseFile( file );
	return 0;
}


int fs_djfs::MakeDirectory( char *node )
{

	if ( Create(node) != 0 ) return -1;

		DJFSFile *file = m_root->getFile( node );
		if ( file == NULL ) 
		{
			printf("%s\n","created file but can't get it!");
			return -1;
		}
			
	file->setDirectory();
	m_root->releaseFile( file );
	return 0;
}

int fs_djfs::DeleteDirectory( char *node )
{

	DJFSFile *file = m_root->getFile( node );
	if ( file == NULL ) return -1;

	if ( file->size() != 0 )
	{
		printf("%s\n","Directory not empty!");
		m_root->releaseFile( file );
		return -1;
	}

	m_root->releaseFile( file );
	return Delete( node );
}


// -------------------------------------------------------------



// -------------------------------------------------------------


struct djfs_superblock *fs_djfs::getSuperblock()
{
	if ( dev_read( SUPERBLOCK_SECTOR * SECTOR_SIZE, &pvd, SUPER_SIZE ) 
					!= SUPER_SIZE )
						return NULL;

	if ( pvd.magic != DJFS_MAGIC ) return NULL;
	return &pvd;
}








