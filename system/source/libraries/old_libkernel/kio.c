#include <kernel.h>


/**  \defgroup IO  Input/Output  
 *
 */

/** @{ */

  int smk_request_io( int port )
  {
	  return _sysenter( (SYS_IO|SYS_ONE), 0, port,0,0,0 );
  }

  int smk_release_io( int port )
  {
	  return _sysenter( (SYS_IO|SYS_ONE), 1, port,0,0,0 );
  }

  int smk_request_iopriv()
  {
	  return _sysenter( (SYS_IO|SYS_TWO), 0,0,0,0,0 );
  }

  int smk_release_iopriv()
  {
	  return _sysenter( (SYS_IO|SYS_TWO), 1,0,0,0,0 );
  }



/** @} */


