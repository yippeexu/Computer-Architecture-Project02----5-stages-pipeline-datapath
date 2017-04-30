#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scan.h"
#include "inst.h"
#include "ID.h"
#include "EX.h"
#include "DM.h"

int main(void)
{
	scanMem();
	scanInst();
	FILE * file = fopen("error_dump.rpt", "w");
	HI = 0;	LO = 0;	m_hi = 0; m_lo = 0;	fclose(file);
//==========================================	
	file = fopen("snapshot.rpt", "w");
	index = 0;
	cycle = 0;
	PC = iPC;
	reg[29] = SP;
	//
	fprintf(file, "cycle 0\n");
	for (int i = 0; i < 32; i++) {
		fprintf(file, "$%02d: 0x%08X\n", i, reg[i]);
	}
	fprintf(file, "$HI: 0x%08X\n", HI);
	fprintf(file, "$LO: 0x%08X\n", LO);
	// stage name init
	strcpy(prev_IF_ID.instName, "NOP");
	strcpy(prev_ID_EX.instName, "NOP");
	strcpy(prev_EX_DM.instName, "NOP");
	strcpy(prev_DM_WB.instName, "NOP");
	fclose(file);
	// main loop
	while (index <= instNum && !end) {
		// WB
		WB();
		// DM
		DM();
		// EX						// ALU, branch and jump index handle
		EX();
		// ID						// note reg stage, ALU input, 
		ID();
		// IF (first half of ID)	// fetch (and parse), index+4
		readInst();

		printRpt(index);
		printErr();
		/*printf("cycle %d: end = %d\n", cycle, end);
		printf("cycle %d: index = %d\n", cycle, index);
		printf("cycle %d: instNum = %d\n", cycle, instNum);*/
		// pipe reg
		if (!isStalled)	prev_IF_ID = next_IF_ID;
		//if (isFlushed)	strcpy(prev_IF_ID.instName, "NOP");
		prev_ID_EX = next_ID_EX;
		prev_EX_DM = next_EX_DM;
		prev_DM_WB = next_DM_WB;
		final = next_WB_ID;
		// pc
		
		if (!isStalled) {
			if (PC >= iPC)	index += ( 1 + _index );
			PC += ( 4 + _pc );
		}	++cycle;
		// flush = 0
		isFlushed = 0;
		// jump
		_pc = 0;
		_index = 0;
	}
//==========================================
	free(inst);
	return 0;
}