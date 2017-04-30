#ifndef SCAN_
#define SCAN_

#include <stdio.h>
#include <stdlib.h>
#include "trans.h"
#include "inst.h"

extern void scanInst(void)
{
	FILE * file = fopen("iimage.bin", "r");
	if (!file) { printf("No iimage.bin"); exit(1); }
	unsigned char ch[4];
	unsigned int _pc;
	char buffer[32];
//= get PC address    ========================================
	for (int i = 0; i < 4; i++) {
		fscanf(file, "%c", &ch[i]);
		intToBin((unsigned int)ch[i], &buffer[24 - i * 8], 8);
	}	PC = iPC = binToUnsInt(buffer, 31, 0);
//= get instructions number ==================================
	for (int i = 0; i < 4; i++) {
		fscanf(file, "%c", &ch[i]);
		intToBin((unsigned int)ch[i], &buffer[24 - i * 8], 8);
	}	instNum = binToUnsInt(buffer, 31, 0);
	inst = malloc( sizeof(unsigned int) * instNum );
//= translate to MIPS ========================================
	for (int i = 0; i < instNum; i++) {
		for (int j = 0; j < 4; j++) {
			fscanf(file, "%c", &ch[j]);
			intToBin((unsigned int)ch[j], &buffer[24 - j * 8], 8);
		}	inst[i] = binToUnsInt(buffer, 31, 0);
	}	fclose(file);
}
extern void scanMem(void)
{
	FILE * file = fopen("dimage.bin", "r");
	if (!file) { printf("No dimage.bin"); exit(1); }
	unsigned char ch[4];
	char buffer[32];
//= get stack pointer ========================================
	//for (int i = 0; i < 32; i++)	reg[i] = 0;	// global
	for (int i = 0; i < 4; i++) {
		fscanf(file, "%c", &ch[i]);
		intToBin((unsigned int)ch[i], &buffer[24 - i * 8], 8);
	}	SP = binToUnsInt(buffer, 31, 0);
//= get memory number ========================================
	for (int i = 0; i < 4; i++) {
		fscanf(file, "%c", &ch[i]);
		intToBin((unsigned int)ch[i], &buffer[24 - i * 8], 8);
	}	memNum = binToUnsInt(buffer, 31, 0);
//= get memory[1024]  ========================================
	for (int i = 0; i < memNum; i++) {
		for (int j = 0; j < 4; j++) {
			fscanf(file, "%c", &ch[j]);
			intToBin((unsigned int)ch[j], &buffer[24 - j * 8], 8);
		}	memory[i] = binToUnsInt(buffer, 31, 0);	// beware of Memory overflow
	}	fclose(file);
}

#endif