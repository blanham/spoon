#include <types.h>
#include <spoon/interface/Container.h>



Container::Container()
{
}

Container::Container( const Container& container )
{
	Container *cont = (Container*)&container;
		
	cont->lock();

	for ( int i = 0; i < cont->countChildren(); i++ )
	{
		addChild( cont->getChild(i) );
	}
		
	cont->unlock();
}

Container::~Container()
{
}


		
bool  Container::addChild( View *view )
{
	lock();
		bool ans = m_list.add( view );
	unlock();
	return ans;
}

View* Container::getChild( int i )
{
	lock();
		View *view = (View*)m_list.get( i );
	unlock();
	return view;
}


bool Container::removeChild( View *view )
{
	lock();
		bool ans = (View*)m_list.remove( view );
	unlock();
	return ans;
}


int   Container::countChildren()
{
	lock();
		int count = m_list.count();
	unlock();
	return count;
}




