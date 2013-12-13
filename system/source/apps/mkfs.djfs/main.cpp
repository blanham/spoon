#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spoon/storage/File.h>


#include "/spoon/system/source/servers/vfs_server/fs/djfs_defs.h"

void die(char *message)
{
	printf("%s\n",message);
	exit(-1);
}


int main( int argc, char *argv[] )
{
	if ( argc != 2 )
	{
		printf("%s\n","usage: mkfs.djfs filename");
		return -1;
	}
	

	struct djfs_superblock 	DJ_SUPER;
	struct djfs_file 		DJ_FILE;
	struct stat st;
	int *block_table;
	
	File *file = new File( argv[1] );
	if ( file->Open() < 0 )
	{
		printf("%s%s\n","unable to open file: ", argv[1]);
		delete file;
		return -1;
	}

		if ( file->Stat( &st ) != 0 )
		{
			file->Close();
			delete file;
			printf("%s%s\n","unable to stat file: ", argv[1]);
			return -1;
		}

		int total_blocks = (st.st_size / SECTOR_SIZE);
	
		if ( total_blocks <= 0 )
		{
			printf("%s\n","Bad stat. File not big enough." );
			file->Close();
			delete file;
			return -1;
		}
		
		printf("%s%u%s\n",
			   "Generating a DJFS volume with ", 
			   total_blocks,
			   " blocks.");
		
	// -------------------------------------------------------
	
		DJ_SUPER.magic 			= DJFS_MAGIC;
		DJ_SUPER.total_blocks 	= total_blocks;
		DJ_SUPER.free_blocks 	= total_blocks;
		strcpy(DJ_SUPER.label, "DJFS Volume.");

		block_table = (int*)(malloc( total_blocks * sizeof(int)) );
		for ( int i = 0; i < total_blocks; i++ )
			block_table[i] = -1;
		
	int block_count = ((total_blocks * sizeof(int)) / SECTOR_SIZE) + 1;

		printf("%s%i%s\n","Block table uses ", block_count, " blocks." );

	// -------------------------------------------------------

		DJ_FILE.table				=	-1;
		DJ_FILE.size				=	0;
		DJ_FILE.flags				=	DIRECTORY;
		DJ_FILE.location			=	block_count + 1;
		strcpy( DJ_FILE.filename, "DJFS.ROOT" );

		
	// -------------------------------------------------------
		for ( int i = 0; i < (block_count + 1); i++ )
		{
			block_table[i] = 0;
			DJ_SUPER.free_blocks -= 1;
		}

		block_table[ block_count + 1 ] = block_count + 1;
		DJ_SUPER.free_blocks -= 1;
		
	// -------------------------------------------------------
	
	
		file->Seek( 0 );
		if ( file->Write( &(DJ_SUPER), SUPER_SIZE ) != SUPER_SIZE )
				die("unable to write superblock");
		
		file->Seek( SECTOR_SIZE );
		if ( file->Write( block_table, (total_blocks * sizeof(int)) ) 
						!= (int)(total_blocks * sizeof(int)) )
				die("unable to write block table");
				
		file->Seek( SECTOR_SIZE * (block_count + 1) );
		if ( file->Write(  &(DJ_FILE), FILE_SIZE ) != FILE_SIZE )
				die("unable to write root file.");
	
	
	file->Close();
	delete file;

	return 0;
}










