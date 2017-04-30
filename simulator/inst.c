#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trans.h"
#include "inst.h"

// ID first half
void readInst(void)	// four bytes
{
	if (index > instNum) {
		next_IF_ID.inst = 0;
		strcpy(next_IF_ID.instName, "NOP");
		return;
	}
	if (PC < iPC) {
		//printRpt(555, 555, 555);
		// NOP
		strcpy(next_IF_ID.instName, "NOP");
		return;
	}
	if (isFlushed) {
		strcpy(next_IF_ID.instName, "NOP");
		return;
	}
	char bin[32];
	intToBin(inst[index], bin, 32);
	next_IF_ID.inst = inst[index];
//===============================================
	next_IF_ID.opcode 	= binToUnsInt(bin, 31, 26);
	// R-type
	next_IF_ID.rs 		= binToUnsInt(bin, 25, 21);
	next_IF_ID.rt 		= binToUnsInt(bin, 20, 16);
	next_IF_ID.rd 		= binToUnsInt(bin, 15, 11);
	next_IF_ID.shamt 	= binToUnsInt(bin, 10,  6);
	next_IF_ID.funct	= binToUnsInt(bin,  5,  0);
	// I-type, J-type
	next_IF_ID.immediate = binToInt(bin, 15, 0);	// warning for the extention from 16bit to 32bit (signed)
	next_IF_ID.address   = binToUnsInt(bin, 25, 0);	//
//===============================================
	if ( next_IF_ID.opcode == 0 ) {
		// R-type
		switch(next_IF_ID.funct) {
			// default
			case 0x20 :		strcpy(next_IF_ID.instName, "ADD");		break;	// add
			case 0x21 :		strcpy(next_IF_ID.instName, "ADDU");	break;	// addu
			case 0x22 :		strcpy(next_IF_ID.instName, "SUB");		break;	// sub
		//=========================================================================
			case 0x24 :		strcpy(next_IF_ID.instName, "AND");		break;	// and
			case 0x25 :		strcpy(next_IF_ID.instName, "OR");		break;	// or
			case 0x26 :		strcpy(next_IF_ID.instName, "XOR");		break;	// xor
			case 0x27 :		strcpy(next_IF_ID.instName, "NOR");		break;	// nor
			case 0x28 :		strcpy(next_IF_ID.instName, "NAND");	break;	// nand
			case 0x2a :		strcpy(next_IF_ID.instName, "SLT");		break;	// slt
		//=========================================================================
			// shift
			case 0x00 :			// sll
				if (next_IF_ID.rt == 0 && next_IF_ID.rd == 0 && next_IF_ID.shamt == 0)
					strcpy(next_IF_ID.instName, "NOP");
				else	strcpy(next_IF_ID.instName, "SLL");
				break;
			case 0x02 :		strcpy(next_IF_ID.instName, "SRL");		break;	// srl
			case 0x03 :		strcpy(next_IF_ID.instName, "SRA");		break;	// sra
		//=========================================================================
			// jr
			case 0x08 :		strcpy(next_IF_ID.instName, "JR");		break;	// jr
		//=========================================================================
			// mult, multu
			case 0x18 :		strcpy(next_IF_ID.instName, "MULT");	break;	// mult
			case 0x19 :		strcpy(next_IF_ID.instName, "MULTU");	break;	// multu
		//=========================================================================
			// mfhi, mflo
			case 0x10 :		strcpy(next_IF_ID.instName, "MFHI");	break;	// mfhi
			case 0x12 :		strcpy(next_IF_ID.instName, "MFLO");	break;	// mflo
		//=========================================================================
			default :
				printf("illegal instruction found at 0x%08X\n", PC);
				end = 1;
		}
	} else {
		switch(next_IF_ID.opcode) {
			// halt
			case 0x3f :		strcpy(next_IF_ID.instName, "HALT");	break;
			// J-type
			case 0x02 :		strcpy(next_IF_ID.instName, "J");		break;	// j C
			case 0x03 :		strcpy(next_IF_ID.instName, "JAL");		break;	// jal C
			// I-type
			case 0x08 :		strcpy(next_IF_ID.instName, "ADDI");	break;	// addi
			case 0x09 :		strcpy(next_IF_ID.instName, "ADDIU");	break;	// addiu
		//=========================================================================
			case 0x23 :		strcpy(next_IF_ID.instName, "LW");		break;	// lw
			case 0x21 :		strcpy(next_IF_ID.instName, "LH");		break;	// lh
			case 0x25 :		strcpy(next_IF_ID.instName, "LHU");		break;	// lhu
			case 0x20 :		strcpy(next_IF_ID.instName, "LB");		break;	// lb
			case 0x24 :		strcpy(next_IF_ID.instName, "LBU");		break;	// lbu
			case 0x2b :		strcpy(next_IF_ID.instName, "SW");		break;	// sw
			case 0x29 :		strcpy(next_IF_ID.instName, "SH");		break;	// sh
			case 0x28 :		strcpy(next_IF_ID.instName, "SB");		break;	// sb
		//=========================================================================
			case 0x0f :		strcpy(next_IF_ID.instName, "LUI");		break;	// lui
			case 0x0c :		strcpy(next_IF_ID.instName, "ANDI");	break;	// andi
			case 0x0d :		strcpy(next_IF_ID.instName, "ORI");		break;	// ori
			case 0x0e :		strcpy(next_IF_ID.instName, "NORI");	break;	// nori
			case 0x0a :		strcpy(next_IF_ID.instName, "SLTI");	break;	// slti
			case 0x04 :		strcpy(next_IF_ID.instName, "BEQ");		break;	// beq
			case 0x05 :		strcpy(next_IF_ID.instName, "BNE");		break;	// bne
			case 0x07 :		strcpy(next_IF_ID.instName, "BGTZ");	break;	// bgtz
			default :
				printf("illegal instruction found at 0x%08X\n", PC);
				end = 1;
		}
	}
}
int isOverflow(int a, int b)
{
	if (a > 0 && b > 0)
		return a + b < 0 ? 1 : 0 ;
	else if (a < 0 && b < 0)
		return a + b >= 0 ? 1 : 0 ;
	else return 0;
}
void printErr(void)
{
	FILE * file = fopen("error_dump.rpt", "a+");
	for (int i = 0; i < 5; i++)
		if (errorList[i] == 1)
			switch(i) {
				case 0:	fprintf(file, "In cycle %d: Write $0 Error\n", cycle);				break;
				case 1:	fprintf(file, "In cycle %d: Overwrite HI-LO registers\n", cycle);	break;
				// todo check for max of the memory address
				case 2:	fprintf(file, "In cycle %d: Address Overflow\n", cycle);			break;
				case 3:	fprintf(file, "In cycle %d: Misalignment Error\n", cycle);			break;
				case 4:	fprintf(file, "In cycle %d: Number Overflow\n", cycle);				break;
			}
	fclose(file);
}
void printRpt(int index)
{
	FILE * file = fopen("snapshot.rpt", "a+");
	if (cycle > 0)	fprintf(file, "cycle %d\n", cycle);
	// write reg
	if (final.isWritten)
		if (final.result != final.origDst)
			fprintf(file, "$%02d: 0x%08X\n", final.writeIndex, final.result);
	if (prev_EX_DM.hilo) {
		if (prev_EX_DM._hi != HI)	fprintf(file, "$HI: 0x%08X\n", HI);
		if (prev_EX_DM._lo != LO)	fprintf(file, "$LO: 0x%08X\n", LO);
	}	// stage
	fprintf(file, "PC: 0x%08X\n", PC);
	// IF
	fprintf(file, "IF: 0x%08X", inst[index]);
		// IF stalled, flushed
		if (isStalled)		fprintf(file, " to_be_stalled");
		if (isFlushed)		fprintf(file, " to_be_flushed");
		fprintf(file, "\n");
	// ID
	fprintf(file, "ID: %s", prev_IF_ID.instName);
		// ID stalled, forwarding
		if (isStalled)		fprintf(file, " to_be_stalled");
		if (fwdToID.isForward_rs)
			fprintf(file, " fwd_%s_rs_$%d", fwdToID.stageFrom, fwdToID.regIndex);
		if (fwdToID.isForward_rt)
			fprintf(file, " fwd_%s_rt_$%d", fwdToID.stageFrom, fwdToID.regIndex);
		fprintf(file, "\n");
	fprintf(file, "EX: %s", prev_ID_EX.instName);
		// EX forwarding
		if (fwdToEX.isForward_rs)
			fprintf(file, " fwd_%s_rs_$%d", fwdToEX.stageFrom, fwdToEX.regIndex);
		if (fwdToEX.isForward_rt)
			fprintf(file, " fwd_%s_rt_$%d", fwdToEX.stageFrom, fwdToEX.regIndex);
		fprintf(file, "\n");
	// DM
	fprintf(file, "DM: %s\n", prev_EX_DM.instName);
	fprintf(file, "WB: %s\n\n\n", prev_DM_WB.instName);
	fclose(file);
// ==================================
	// fed init
	fwdToID.isForward_rs = 0;
	fwdToID.isForward_rt = 0;
	fwdToEX.isForward_rs = 0;
	fwdToEX.isForward_rt = 0;
	// $zero
	regStage[0] = 0;
	reg[0] = 0;
	// final
	if (final.isWritten) {
		regStage[final.writeIndex] = 0;
	}
}