#ifndef _SPOON_INTERFACE_SCROLLABLE_H
#define _SPOON_INTERFACE_SCROLLABLE_H

class ScrollBar;

/** \ingroup interface 
 *
 */
class Scrollable
{
   public:
     Scrollable()
      : _vertScrollBar(NULL), _horizScrollBar(NULL)
      {}

   
     virtual int ScrollTo( int value, bool vertical ) { return 0; };
     virtual int ScrollBy( int diff, bool vertical ) { return 0; };

     ScrollBar* VerticalScrollBar() { return _vertScrollBar; }
     ScrollBar* HorizontalScrollBar() { return _horizScrollBar; }

   private:
     friend class ScrollBar; 

     ScrollBar* _vertScrollBar;
     ScrollBar* _horizScrollBar;
     

};

#endif

