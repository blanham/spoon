#include "iso9660.h"
#include "iso9660_defs.h"


FileSystem *create_iso9660( VFS::Device *dev )
{
  return new fs_iso9660( dev );
}


fs_iso9660::fs_iso9660( VFS::Device *dev )
: FileSystem( "iso9660",
	      "iso9660 file system for cdroms",
	       dev )
{
}

fs_iso9660::~fs_iso9660()
{
}

// ****************************************************

int fs_iso9660::Init()
{
  struct primary_volume_descriptor *p = getPrimary();
  if ( p == NULL ) return -1;
  
  pvd = *p;
  free( p );
  return 0;
}

int fs_iso9660::Shutdown()
{
	return 0;
}

// ****************************************************

int fs_iso9660::Test()
{
   char *block = (char*)malloc(2048);
   struct volume_descriptor *vd = (struct volume_descriptor*)block;
  
   printf("%s\n","fs_iso9660::Test beginning test.");
  
   if ( dev_read( 0x8000, block, 2048 ) != 2048 ) 
   {
	free( block );
   	return FS_UNSUPPORTED;
   }

   printf("%s\n","fs_iso9660::Test read the 16th sector.");

   if ( memcmp( (vd->identifier), "CD001", 5 ) != 0 ) 
   {
	free( block );
   	return FS_UNSUPPORTED;
   }

   printf("%s\n","fs_iso9660::Test has determined that it's compatible.");

   free( block );
   return FS_EXACT;
}

// ----------------------------------------------------

void *fs_iso9660::Open( const char *node )
{
	return findFile( node );
}

int fs_iso9660::Read( int64 pos, void *buffer, int len, void *data )
{
  struct directory_record *dent = (struct directory_record*)data;

  int req = dent->data_length_msb - (int)pos;
  if ( len < req ) req = len;

  int64 new_pos = dent->location_extent_msb * 2048 + pos;

        int ans = dev_read( new_pos, buffer, req );

  return ans;
}

int fs_iso9660::Write( int64 pos, void *buffer, int len, void *data )
{
	return -1;
}

void fs_iso9660::Close( void *data )
{
	free( data );
}


int fs_iso9660::Stat( struct stat *st, void *data )
{
  struct directory_record *dent = (struct directory_record*)data;

	st->st_dev  = GetDevice()->getDeviceID();
	st->st_size = dent->data_length_msb;
	st->st_mode = 0;

	if ( (dent->flags & DIR_FLAG) == DIR_FLAG ) st->st_mode |= S_IFDIR;

  return 0;
}



int fs_iso9660::List( char *node, Message **list )
{
   struct directory_record *dir;
   struct directory_record *tmp;
   bool root = true;
       *list = NULL;
   
   
   if ( strcmp(node,"") == 0 ) 
   	dir = (struct directory_record*) (&(pvd.root_directory_record) );
	   else 
	     {
		dir = findFile( node );
		root = false;
	     }

   if ( dir == NULL ) return -1;

   int size = dir->data_length_msb;
   int position = 0;
   
   Message *msg = new Message(OK);
   
   char *data = (char*)malloc( size );

    if ( dev_read( dir->location_extent_msb * 2048, data, size ) == size ) 
    {
      int count = 0;
      while ( position <= size )
      {
        tmp = (struct directory_record*)(data + position);
	if ( tmp->length == 0 ) break;

        char *name = (char*)strndup( (char*)tmp->file_id, tmp->file_id_len );
	  

	  for ( unsigned int i = 0; i < strlen(name); i++ )
	  	if ( name[i] == ';' ) 
		{
			name[i] = 0;
			break;
		}

	  // 12-02-2005 added quick support for files which don't have extensions
	  // and yet, whatever application created the CD, now magically do. Probably
	  // mkisofs

	  int len = strlen(name);
	  for ( int i = len - 1; i >= 0; i-- )
	    {
		if ( name[i] == '.' ) { name[i] = 0; break; }
		if ( (name[i] != '.') && (name[i] != ' ') ) break;
	    }

	  // finished support...  this might cause problems later on.
	  

	if ( count == 0 ) {
			     free( name );
			     name = strdup(".");
	  	  	   }
	if ( count == 1 ) {
	  		     free( name );
			     name = strdup("..");
			  }
	count++;

	msg->AddString("entry", name );

	free( name );

	position += tmp->length;
      }
    }
    else msg->what = ERROR;

   free( data );

   if ( ! root ) free(dir);

   *list = msg;

   return 0;
}


// --------------------------------------------------------


struct directory_record* fs_iso9660::findFile( const char *name )
{
    int prev = 0;
    int last = 0;

	for ( int i = strlen(name)-1; i > 0; i-- )
		if ( name[i] == '/' ) 
		{
			last = i+1;
			break;
		}

    struct directory_record *dir = 
    	(struct directory_record *)&(pvd.root_directory_record); 

    for ( unsigned int i = 0; i <= strlen(name); i++ )
    {
      if ( (name[i] == '/') || (i == strlen(name)) ) 
      {
        char *dn = (char*)strndup( name + prev, i - prev );

		 struct directory_record *old = dir;
		 dir = run_dirs( dn, dir->location_extent_msb, i,
		 		     dir->data_length_msb );

	 if ( prev != 0 ) free( old );
	 free( dn );
		 
	 if ( dir == NULL )   return NULL;	// didn't find
	 if ( prev == last )  return dir;	// found it
 	 if (  (dir->flags & DIR_FLAG) != DIR_FLAG ) return NULL;
	 		// we can't go further

	prev = i + 1;	// go to next dir
      }
    }

    return NULL;
}

struct primary_volume_descriptor* fs_iso9660::getPrimary()
{
    struct volume_descriptor *p = 
	    (struct volume_descriptor *)
 		    malloc( sizeof(struct volume_descriptor)) ;

    int offset = 0;
    while ( dev_read( 0x8000 + offset, p, 2048 ) == 2048 ) 
    {
       if ( p->type == VD_TYPE_PRIMARY ) 
         return (struct primary_volume_descriptor*)p;
       if ( p->type == VD_TYPE_TERMINATOR ) break;

       offset += 2048;	// ok, next sector;
       
       if ( offset > (2048 * 300) ) break;	// i dunno.
    }

    free( p );
    return NULL;
}


struct directory_record *fs_iso9660::run_dirs(  char *match,
         					int block, 
						int offset, 
						int size )
{
	int position = 0;
	char *data = (char*)malloc(size);

	if ( dev_read( block * 2048, data, size ) != size )
	{
		free( data );
		return NULL;
	}
        struct directory_record *dir = (struct directory_record*)data;

	int count = 0;
	while ( (position < size) )
	{
	       dir = (struct directory_record*)(data + position);
	       if ( dir->length == 0 ) break;
	       position += dir->length;

	       if (count++ < 2) continue;

	       char *name = 
	          (char*)strndup( (char*)dir->file_id, dir->file_id_len );

		  for ( unsigned int i = 0; i < strlen(name); i++ )
		  	if ( name[i] == ';' ) 
			{
				name[i] = 0;
				break;
			}

		  	  // 12-02-2005 added quick support for files which don't have extensions
			  // and yet, whatever application created the CD, now magically do. Probably
			  // mkisofs

			  int len = strlen(name);
			  for ( int i = len - 1; i >= 0; i-- )
			    {
				if ( name[i] == '.' ) { name[i] = 0; break; }
				if ( (name[i] != '.') && (name[i] != ' ') ) break;
			    }

			  // finished support...  this might cause problems later on.
	
		  
		  
		 if ( insensitive_match( name, match ) == 0 )
		  {
		    free( name );
		    struct directory_record *de = (struct directory_record*)
		    	malloc( sizeof(struct directory_record) );
		    memcpy( de, dir, dir->length );
		    free( data );
		    return de;
		  }

	       free( name );
	}
	

	free( data );
	return NULL;
}



int fs_iso9660::insensitive_match( char *src, char *dest )
{
   unsigned int i;
   int ans;
   char *a = strdup( src );
   char *b = strdup( dest );

	for ( i = 0; i < strlen(a); i++ ) 
	  a[i] = tolower( a[i] );

	for ( i = 0; i < strlen(b); i++ ) 
	  b[i] = tolower( b[i] );

	ans = strcmp( a, b );

   free( a );
   free( b );
   return ans;
}


