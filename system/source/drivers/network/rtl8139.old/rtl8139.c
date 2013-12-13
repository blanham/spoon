/* 
** Copyright 2001, Travis Geiselbrecht. All rights reserved.
** Distributed under the terms of the NewOS License.
*/

#include "rtl8139_priv.h"

//struct rtl8139_fs {
//	fs_id id;
//	void *covered_vnode;
//	void *redir_vnode;
//	rtl8139 *rtl;
//	int root_vnode; // just a placeholder to return a pointer to
//};

/*
static int rtl8139_read(void *_fs, void *_vnode, void *_cookie, void *buf, off_t pos, size_t *len)
{
	struct rtl8139_fs *fs = _fs;
	int err;
	
	if(*len < 1500)
		return ERR_VFS_INSUFFICIENT_BUF;
	if(fs->rtl == NULL)
		return ERR_IO_ERROR;
	err = rtl8139_rx(fs->rtl, buf, *len);
	if(err < 0)
		return err;
	else
		*len = err;
	return 0;
}

static int rtl8139_write(void *_fs, void *_vnode, void *_cookie, const void *buf, off_t pos, size_t *len)
{
	struct rtl8139_fs *fs = _fs;

	if(*len > 1500)
		return ERR_VFS_INSUFFICIENT_BUF;
	if(fs->rtl == NULL)
		return ERR_IO_ERROR;

	rtl8139_xmit(fs->rtl, buf, *len);
	return 0;
}


static int rtl8139_ioctl(void *_fs, void *_vnode, void *_cookie, int op, void *buf, size_t len)
{
	struct rtl8139_fs *fs = _fs;
	int err;

	dprintf("rtl8139_ioctl: op %d, buf 0x%x, len %d\n", op, buf, len);

	switch(op) {
		case 10000: // get the ethernet MAC address
			if(len >= sizeof(fs->rtl->mac_addr)) {
				memcpy(buf, fs->rtl->mac_addr, sizeof(fs->rtl->mac_addr));
				err = 0;
			} else {
				err = ERR_VFS_INSUFFICIENT_BUF;
			}
			break;
		case 87912: // set the rtl pointer
			fs->rtl = buf;
			err = 0;
			break;
		default:
			err = ERR_INVALID_ARGS;
	}
	
	
	return err;
}
*/

int main(int argc, char *argv[])
{
	rtl8139 *rtl;
	int fd;
	

	// detect and setup the device
	if(rtl8139_detect(&rtl) < 0) {
		// no rtl8139 here
		printf("%s","rtl8139_dev_init: no device found\n");
		return 0;
	}
	
	rtl8139_init(rtl);

//	fd = sys_open("/dev/net/rtl8139/0", "", STREAM_TYPE_DEVICE);
//	if(fd < 0) {
//		panic("rtl8139_dev_init: could not open %s\n", "/dev/net/rtl8139/0");
//	}
//	sys_ioctl(fd, 87912, (void *)rtl, 4);
//	sys_close(fd);
	return 0;
}

