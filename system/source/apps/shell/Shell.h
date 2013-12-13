#include <types.h>
#include <spoon/app/ConsoleApplication.h>


#ifndef _SHELL_H
#define _SHELL_H


class Shell : public ConsoleApplication
{
	public:
		Shell();
		~Shell();

		int main( int argc, char *argv[] );

};


extern Shell *shell;

#endif

