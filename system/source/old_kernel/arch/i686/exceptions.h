#ifndef _KERNEL_EXCEPTIONS_H
#define _KERNEL_EXCEPTIONS_H

#include "tss.h"
#include "lengths.h"


void init_exceptions();


int real_exception(uint32 num);

// ************************
//  See jump.s for the implementations of these
//  declarations.

void __exception_handler0();
void __exception_handler1();
void __exception_handler2();
void __exception_handler3();
void __exception_handler4();
void __exception_handler5();
void __exception_handler6();
void __exception_handler7();
void __exception_handler8();
void __exception_handler9();
void __exception_handlerA();
void __exception_handlerB();
void __exception_handlerC();
void __exception_handlerD();
void __exception_handlerE();
void __exception_handlerF();
void __exception_handler10();
void __exception_handler11();
void __exception_handler12();
void __exception_handler13();
void __exception_handler14();
void __exception_handler15();
void __exception_handler16();
void __exception_handler17();
void __exception_handler18();
void __exception_handler19();
void __exception_handler1A();
void __exception_handler1B();
void __exception_handler1C();
void __exception_handler1D();
void __exception_handler1E();
void __exception_handler1F();



void device_not_available();
void double_falt();

void __loop();

#endif


