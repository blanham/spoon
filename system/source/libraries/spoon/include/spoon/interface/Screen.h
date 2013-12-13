#ifndef _SPOON_INTERFACE_SCREEN_H
#define _SPOON_INTERFACE_SCREEN_H


#define CURRENT_SCREEN		0

#include <spoon/interface/Rect.h>

/** \ingroup interface 
 *
 */
class Screen
{
   public:
     Screen();
     ~Screen();

     static Rect Frame( int _screen_id = CURRENT_SCREEN );

     
};

#endif


