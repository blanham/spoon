

/**  \defgroup STRINGS  Strings  
 *
 */

/** @{ */


char *process_state[] = 
{
    "system",
    "running",
    "stopped",
    "finishing",
    "finished",
    "crashing",
    "crashed"
};

char *thread_state[] =
{
	"system",
	"running",
	"suspended",
	"dormant",
	"stopped",
	"semaphore",
	"sleeping",
	"waiting",
	"finished",
	"crashed",
	"irq"
};






char *exception_strings[] = 
{
    "Divide-by-zero",			//	0
    "Debug Exception",			//	1
    "NMI",				//	2
    "Breakpoint",			//	3
    "INTO",				//	4
    "BOUNDS",				//	5
    "Invalid Opcode",			//	6
    "Device Not Available",		//	7
    "Double-fault",			//	8
    "Coprocessor segment overrun",	//	9
    "Invalid TSS fault",		//	10
    "Segment Not Present",		//	11
    "Stack Exception",			//	12
    "General Protection",		//	13
    "Page Fault",			//	14
    "*reserved*",			//	15
    "Floating-point error",		//	16
    "Alignment Check",			//	17
    "Machine Check",			//	18
    "*reserved*", 			//	19
    "*reserved*", 			//	20
    "*reserved*", 			//	21
    "*reserved*", 			//	22
    "*reserved*", 			//	23
    "*reserved*", 			//	24
    "*reserved*", 			//	25
    "*reserved*", 			//	26
    "*reserved*", 			//	27
    "*reserved*", 			//	28
    "*reserved*", 			//	29
    "*reserved*", 			//	30
    "*reserved*" 			//	31
};           

void* 	kmemcpy(void* s, const void* ct, int n)
{
	char *dest,*src;
	int i;

	dest = (char*)s;
	src = (char*)ct;
	for ( i = 0; i < n; i++)
		dest[i] = src[i];
	
	return s;
}

void* 	kmemset(void* s, int c, int n)
{
	int i;
	for ( i = 0; i < n ; i++)
		((char*)s)[i] = c;
	
	return s;
}


char* 	kstrcpy(char* s, const char* ct)
{
	int i = 0;
	do { s[i] = ct[i]; } 
		while (ct[i++] != '\0');
	return s;
}

 
 
int 	kstrlen(const char* cs)
{
	int ans = 0;
	while ( cs[ans++] != '\0' ) {};
	return (ans-1);
}


/** @} */

