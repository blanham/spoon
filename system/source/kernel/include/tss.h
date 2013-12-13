#ifndef _KERNEL_TSS_H_
#define _KERNEL_TSS_H_


struct TSS
{
   unsigned int previous	:16;
   unsigned int empty1		:16;
   unsigned int esp0		:32;
   unsigned int ss0			:16;
   unsigned int empty2		:16;   
   unsigned int esp1		:32;
   unsigned int ss1			:16;
   unsigned int empty3		:16;
   unsigned int esp2		:32;
   unsigned int ss2			:16;
   unsigned int empty4		:16;
   unsigned int cr3			:32;
   unsigned int eip			:32;
   unsigned int eflags		:32;
   unsigned int eax			:32;
   unsigned int ecx			:32;
   unsigned int edx			:32;
   unsigned int ebx			:32;
   unsigned int esp			:32;
   unsigned int ebp			:32;
   unsigned int esi			:32;
   unsigned int edi			:32;
   unsigned int es			:16;
   unsigned int empty5		:16;
   unsigned int cs			:16;
   unsigned int empty6		:16;
   unsigned int ss			:16;
   unsigned int empty7		:16;
   unsigned int ds			:16;
   unsigned int empty8		:16;
   unsigned int fs			:16;
   unsigned int empty9		:16;
   unsigned int gs			:16;
   unsigned int empty10		:16;
   unsigned int ldt			:16;
   unsigned int empty11		:16;   
  
   unsigned int trapflag	:16;
   unsigned int iomapbase	:16;

   unsigned char iobitmap[8193];

} __attribute__ ((packed));




struct TSS* new_cpu_tss(int stack_pages);





#endif



