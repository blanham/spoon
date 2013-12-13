#include "list.h"
#include "iso9660.h"
#include "ext2fs.h"
#include "djfs.h"

#define TOTAL_FS	3




struct fs_info list[TOTAL_FS] =
{ 
  { "iso9660",  create_iso9660 },
  { "ext2fs",  create_ext2fs },
  { "djfs",  create_djfs }
};


FileSystem *new_fs( char *code, VFS::Device *dev )
{

  if ( strcmp( code, "auto" ) == 0 ) 
   {
     printf("%s%s\n","auto_testing: device ", dev->getName() );
     
 	  for ( int i = 0; i < TOTAL_FS; i++ )
	   {
	     printf("%s%s\n","auto_testing:    ", list[i].code );
	     FileSystem *tester = (list[i].func)( dev );
	     if ( tester->Test() != FS_UNSUPPORTED ) return tester;
	     delete tester;  
	   }
	   
      return NULL;	   
   }


  for ( int i = 0; i < TOTAL_FS; i++ )
   {
     if ( strcmp( list[i].code, code ) == 0 )
      return (list[i].func)( dev );
   }

  return NULL;
}

