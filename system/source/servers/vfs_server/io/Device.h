#ifndef _VFS_DEVICE_H
#define _VFS_DEVICE_H


namespace VFS
{


class Device
{
	public:
		Device( const char *name, int pid, int port, int device_id );
		virtual ~Device();

		const char *getName();
		int 		getPid();
		int 		getPort();
		int 		getDeviceID();
		

	private:
		char *m_name;
		int m_pid;
		int m_port;
		int m_deviceID;
		
};

}

#endif

