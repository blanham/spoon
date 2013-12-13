#include <types.h>
#include <spoon/interface/ListItem.h>

ListItem::ListItem()
{
	m_selected = false;
	m_enabled = true;
}


ListItem::~ListItem()
{
}

 

bool ListItem::isEnabled()
{
	return m_enabled;
}

bool ListItem::Enable( bool enable )
{
	bool old = m_enabled;
	m_enabled = enable;
	return old;
}
	
bool ListItem::isSelected()
{
	return m_selected;
}

bool ListItem::Select( bool select )
{
	bool old = m_selected;
	m_selected = select;
	return old;
}




