#include <types.h>

#ifndef _CARDS_H
#define _CARDS_H


typedef struct rtl8139 {
	int irq;
//	addr phys_base;
//	addr phys_size;
//	addr virt_base;
	uint16 io_port;
//	region_id region;
	uint8 mac_addr[6];
	int txbn;
//	int last_txbn;
//	region_id rxbuf_region;
//	addr rxbuf;
	void* rxbuf;
//	sem_id rx_sem;
//	region_id txbuf_region;
//	addr txbuf;
	void *txbuf;
//	sem_id tx_sem;
//	mutex lock;
} rtl8139;

int rtl8139_detect(rtl8139 **rtl);
int rtl8139_init(rtl8139 *rtl);
void rtl8139_xmit(rtl8139 *rtl, const char *ptr, int len);
int rtl8139_rx(rtl8139 *rtl, char *buf, int buf_len);


#endif

