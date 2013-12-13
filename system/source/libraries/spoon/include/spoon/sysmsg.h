#ifndef _SPOON_SYSMSG_H
#define _SPOON_SYSMSG_H



#define		KEY_UP			0xd00d0001ul
#define		KEY_DOWN		0xd00d0002ul

#define		MOUSE_UP		0xd00d0003ul
#define		MOUSE_DOWN		0xd00d0004ul
#define		MOUSE_MOVED		0xd00d0005ul


#define		QUIT_REQUESTED		0xd00f0001ul


#define		REGISTER		0xf00d0001ul
#define		DEREGISTER		0xf00d0002ul
#define		UPDATE			0xf00d0003ul
#define		REQUEST			0xf00d0004ul
#define		INSTALL			0xf00d0005ul
#define		UNINSTALL		0xf00d0006ul
#define		EVENT			0xf00d0007ul
#define		INFO			0xf00d0008ul
#define		SCREEN			0xf00d0009ul
#define		SET				0xf00d000Aul
#define		GET				0xf00d000Bul

#define		SHOW			0xf00d1000ul
#define		HIDE			0xf00d1001ul
#define		FOCUS			0xf00d1002ul
#define		DRAW			0xf00d1003ul
#define		MOVE			0xf00d1004ul
#define		SYNC			0xf00d1005ul
#define		RESIZE			0xf00d1006ul


#define		PULSE			0xf00d1100ul

#define		READ			0xf00d2001ul
#define		WRITE			0xf00d2002ul
#define		OPEN			0xf00d2003ul
#define		CLOSE			0xf00d2004ul
#define		SEEK			0xf00d2005ul
#define		STAT			0xf00d2006ul
#define		LIST			0xf00d2007ul
#define		MKDIR			0xf00d2008ul
#define		RMDIR			0xf00d2009ul
#define		DELETE			0xf00d200Aul
#define		CREATE			0xf00d200Bul
#define		EJECT			0xf00d2010ul
#define		LOADED			0xf00d2011ul
#define		MOUNT			0xf00d2012ul
#define		UNMOUNT			0xf00d2013ul


#define		NOP						0x0ul
#define		OK						0x0ul
#define		ERROR					0xfffffffful
#define		MESSAGE_NOT_UNDERSTOOD	0xfffffffeul


#define		REPLY			0xf00d6660ul
#define		NO_REPLY		0xf00d6661ul
#define		TIMEOUT			0xf00d6662ul

#define		IGNORE			0xf00d6666ul

#endif


