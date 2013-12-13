#include <kernel.h>


char *screen;


int myirq( int irq, void *data )
{
	inb(0x60);
	screen[ 160 * 15 + smk_gettid() * 2 ]++;
	return 0;
}


int mythread( void *data )
{
	while (1==1) 
	{
		screen[ 160 * 15 + smk_gettid() * 2 ]++;
	}
}


int main( int argc, char *argv[])
{
	int i;
	int j;
	int tid;
	char *str;

	screen = (char*)smk_mem_map( (void*)0xB8000, 1 );
		

	/*
	for ( i = 0; i < argc; i++ )
	{
		str = argv[i];

		j = 0;
			
		while ( str[j] != 0 )
		{
			screen[ (i + 10) * 160 + j * 2 ] = str[j];
			j++;
		}
	}
	*/
		

	/*
	tid = smk_spawn_thread( mythread, "my thread", 0, NULL ); ;
	smk_resume_thread( tid );
	tid = smk_spawn_thread( mythread, "my thread", 0, NULL ); ;
	smk_resume_thread( tid );
	tid = smk_spawn_thread( mythread, "my thread", 0, NULL ); ;
	smk_resume_thread( tid );
	tid = smk_spawn_thread( mythread, "my thread", 0, NULL ); ;
	smk_resume_thread( tid );
	*/
	
	smk_request_irq( myirq, 1, "irq thread", NULL );
	
	while (1==1) 
	{
		screen[ 160 * 5 + 80 ]++;
	}
	
}



