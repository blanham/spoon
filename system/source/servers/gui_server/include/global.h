#ifndef _GUISERVER_GLOBAL_H
#define _GUISERVER_GLOBAL_H


class Message;

int unknown_message( Message *msg );
int msg_error( Message *msg, char *error, bool del = false );

#endif

