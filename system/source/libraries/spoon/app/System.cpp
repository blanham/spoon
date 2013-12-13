#include <types.h>
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spoon/storage/File.h>
#include <spoon/app/System.h>



int System::Execute( const char *process_name,
					 void *data, int size, 
				     const char *parameters,
				     int console_id,
				     bool wait )
{
	// Build up the command line parameters

	int total_len = strlen( process_name );
	if ( parameters != NULL ) total_len += strlen( parameters );
				  total_len += 1;
	    

   char *cmd_line = (char*)malloc( total_len );

	strcpy( cmd_line, process_name );
	if ( parameters != NULL ) 
	{
		strcat( cmd_line, " " );
		strcat( cmd_line, parameters );
	}
        
	// prepare the process image.

     int pid = smk_mem_exec( process_name , 
     			(uint32)data, ((uint32)data + size ),
     			cmd_line );

	free( cmd_line );

	//printf("%s%i\n","pid = ", pid );

	if ( pid <= 0 ) return -1;

	if ( smk_switch_console( pid, console_id ) != 0 ) 
	{
	   /// \todo  problem. PID exists but it's not in the correct console..... headless zombie.
	   return -1;
	}

	int old_pid = smk_active_pid( console_id );

	// Let it ride...
        if ( wait == true ) smk_set_active_pid( console_id, pid  );
        smk_start_process( pid );
		if ( wait == false ) return pid;

	// We are requested to wait for the pid to return.

       int rc = -1;
       int ans = smk_wait_process( pid, &rc );

       	// return active_pid to the console.
       smk_set_active_pid( console_id, old_pid );

       if ( ans != 0 ) return ans;	// failure to wait.

   return rc;
}



int System::Execute( const char *filename, 
		     const char *parameters,
		     int console_id,
		     bool wait )
{

    File *file = new File( (char*)filename );
    if ( file->Open() < 0 )
     {
       delete file;
       //printf("%s\n","Unable to open file");
       return -1;
     }

     int size = file->Size();

     char *buffer = (char*)malloc(size);
     if ( file->Read( buffer, size ) != size )
      {
        free( buffer );
		file->Close();
		delete file;
        //printf("%s\n","Unable to read file.");
		return -1;
      }
      
     file->Close();
     delete file;
	//  We have the file image in memory now.
  
  	//  Get the process name.
     char *process_name = NULL;
       
        for ( int i = strlen(filename) - 1; i >= 0; i-- )
	    {
	      if ( filename[i] == '/' )
	       { 
	         process_name = strdup( filename + i + 1 );
			 break;
	       }
	    }

	   if ( process_name == NULL ) process_name = strdup(filename); 
       
        //printf("%s%s\n","process name = ", process_name);
   
	// Build up the command line parameters

	int total_len = strlen( filename );
	if ( parameters != NULL ) total_len += strlen( parameters );
				  total_len += 1;
	    

   char *cmd_line = (char*)malloc( total_len );

	strcpy( cmd_line, filename );
	if ( parameters != NULL ) 
	{
		strcat( cmd_line, " " );
		strcat( cmd_line, parameters );
	}
        
	//printf("%s%s\n","cmd_line = ", cmd_line);
	
	// prepare the process image.

     int pid = smk_mem_exec( process_name , 
     			(uint32)buffer, ((uint32)buffer + size ),
     			cmd_line );

	free( cmd_line );
	free( process_name );
	free( buffer );

	//printf("%s%i\n","pid = ", pid );

	if ( pid <= 0 ) return -1;

	if ( smk_switch_console( pid, console_id ) != 0 ) 
	{
	   /// \todo  problem. PID exists but it's not in the correct console..... headless zombie.
	   return -1;
	}

	int old_pid = smk_active_pid( console_id );

	// Let it ride...
        if ( wait == true ) smk_set_active_pid( console_id, pid  );
        smk_start_process( pid );
		if ( wait == false ) return pid;

	// We are requested to wait for the pid to return.

       int rc = -1;
       int ans = smk_wait_process( pid, &rc );

       	// return active_pid to the console.
       smk_set_active_pid( console_id, old_pid );

       if ( ans != 0 ) return ans;	// failure to wait.

   return rc;
}



int System::Execute( const char *filename, 
		     const char *parameters,
		     bool wait )
{

    File *file = new File( (char*)filename );
    if ( file->Open() < 0 )
     {
       delete file;
       //printf("%s\n","Unable to open file");
       return -1;
     }

     int size = file->Size();

     char *buffer = (char*)malloc(size);
     if ( file->Read( buffer, size ) != size )
      {
        free( buffer );
	file->Close();
	delete file;
        //printf("%s\n","Unable to read file.");
	return -1;
      }
      
     file->Close();
     delete file;
	//  We have the file image in memory now.
  
  	//  Get the process name.
     char *process_name = NULL;
       
           for ( int i = strlen(filename) - 1; i >= 0; i-- )
	    {
	      if ( filename[i] == '/' )
	       { 
	         process_name = strdup( filename + i + 1 );
		 break;
	       }
	    }

	   if ( process_name == NULL ) process_name = strdup(filename); 
       
        //printf("%s%s\n","process name = ", process_name);
   
	// Build up the command line parameters

	int total_len = strlen( filename );
	if ( parameters != NULL ) total_len += strlen( parameters );
				  total_len += 1;
	    

   char *cmd_line = (char*)malloc( total_len );

	strcpy( cmd_line, filename );
	if ( parameters != NULL ) 
	{
		strcat( cmd_line, " " );
		strcat( cmd_line, parameters );
	}
        
	//printf("%s%s\n","cmd_line = ", cmd_line);
	
	// prepare the process image.

     int pid = smk_mem_exec( process_name , 
     			(uint32)buffer, ((uint32)buffer + size ),
     			cmd_line );

	free( cmd_line );
	free( process_name );
	free( buffer );

	//printf("%s%i\n","pid = ", pid );

	if ( pid <= 0 ) return -1;

	// Let it ride...
        if ( wait == true ) smk_set_active_pid( smk_get_console( pid ), pid  );
        smk_start_process( pid );
		if ( wait == false ) return pid;

	// We are requested to wait for the pid to return.

       /// \todo wait on pid.
       int rc = -1;
       int ans = smk_wait_process( pid, &rc );

       	// return active_pid to this app.
       smk_set_active_pid( smk_get_console( smk_getpid() ), smk_getpid() );

	//printf("%s%i\n","ans = ", ans );
	
       if ( ans != 0 ) return ans;	// failure to wait.

   return rc;
}


