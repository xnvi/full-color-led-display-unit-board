/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

/*
基于 Keil 安装目录内 ".\Keil\ARM\Startup\Retarget.c" 进行修改
*/

#include <stdio.h>

#pragma import(__use_no_semihosting_swi)

// usart1.c 内的串口发送函数
extern void Usart1SendByte(unsigned char dat);

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	Usart1SendByte(ch);
	return ch;
}

int fgetc(FILE *f) {
	// 如有需要可以重定向到串口接收函数
	return 0;
}

int ferror(FILE *f) {
	/* Your implementation of ferror */
	return EOF;
}

void _ttywrch(int ch) {
	// 如有需要可以重定向到串口输出函数
}

void _sys_exit(int return_code) {
	while (1);    /* endless loop */
}
