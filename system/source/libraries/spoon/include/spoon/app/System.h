#ifndef _SPOON_APP_SYSTEM_H
#define _SPOON_APP_SYSTEM_H



/** \ingroup app
 *
 * The System class just abstracts some system operations (such as 
 * executing programs, etc) into a nice class to make things 
 * easier.  Eventually, it's going to communicate with app_server's
 * vfs_servers, etc to do a system-wide collaboration of stuff.
 * Yeah, you heard me. stuff.
 *
 */


class System
{

   public:
     System() {};
    ~System() {}; 

     static int Execute( const char *process_name,
					 	 void *data, int size, 
		     			 const char *parameters,
						 int console_id,
		     			 bool wait = true );



     static int Execute( const char *filename, 
     			 const char *parameters,
			 int console_id,
     			 bool wait = true );

     static int Execute( const char *filename, 
     			 const char *parameters = NULL,
     			 bool wait = true );

};

#endif

