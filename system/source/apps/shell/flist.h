#ifndef _SHELL_FLIST_H
#define _SHELL_FLIST_H


#define TOTAL_FUNCTIONS		29

typedef int (*func)(int, char**);


struct function_info
{
   char *summary;
   char *detailed;
   func fmain;
};


struct function_entry
{
   char *command;
   struct function_info *info;
};

extern struct function_entry functions[];

extern int function_call( char *command );

#endif

