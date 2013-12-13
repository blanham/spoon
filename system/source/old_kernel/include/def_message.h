#ifndef _KERNEL_DEF_MESSAGE_H
#define _KERNEL_DEF_MESSAGE_H


struct message
{
  int source_pid;
  int source_port;
  int dest_port;
  int bytes;
  struct message *next;
  void *data;
} __attribute__ ((packed));



#endif

