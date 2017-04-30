#ifndef EX_
#define EX_


extern void forwarding(int rs, int rt)
{
	if (regStage[rs] == 2 || regStage[rt] == 2) {
		if (prev_EX_DM.opcode >= 0x20 && prev_EX_DM.opcode <= 0x25) {	// lw
			// impossible
			printf("impossible\n");
		} else {	// fwd from EX-DM
			fwdToEX.isForward_rs = (regStage[rs] == 2) ? 1 : 0 ;
			fwdToEX.isForward_rt = (regStage[rt] == 2) ? 1 : 0 ;
			strcpy(fwdToEX.stageFrom, "EX-DM");
			fwdToEX.regIndex = prev_EX_DM.writeIndex;
		}
	} else if (regStage[rs] == 3 || regStage[rt] == 3) {
		// fed from DM-WB
		fwdToEX.isForward_rs = (regStage[rs] == 3) ? 1 : 0 ;
		fwdToEX.isForward_rt = (regStage[rt] == 3) ? 1 : 0 ;
		strcpy(fwdToEX.stageFrom, "DM-WB");
		fwdToEX.regIndex = prev_DM_WB.writeIndex;
	}
}
extern void EX(void)
{
	if (strcmp(prev_ID_EX.instName, "NOP")==0) {
		next_EX_DM = prev_ID_EX;
		return;
	}
	char bin[32];
	intToBin(prev_ID_EX.inst, bin, 32);
	int rs = prev_ID_EX.rs;
	int rt = prev_ID_EX.rt;
	int rd = prev_ID_EX.rd;
	int funct = prev_ID_EX.funct;
	int shamt = prev_ID_EX.shamt;
	int opcode = prev_ID_EX.opcode;
	int immediate = prev_ID_EX.immediate;
	unsigned int tempUns;
	long long mult;
// =================================================
	forwarding(prev_ID_EX.rsNeed?prev_ID_EX.rs:0, prev_ID_EX.rtNeed?prev_ID_EX.rt:0);
	if ( opcode == 0 ) {
		// R-type
		switch(funct) {
			// default
			case 0x20 :		// add
				errorList[4] = isOverflow(reg[rt], reg[rs]);
				reg[rd] = reg[rt] + reg[rs];
				break;
			case 0x21 :		// addu
				reg[rd] = reg[rt] + reg[rs];
				break;
			case 0x22 :		// sub
				errorList[4] = isOverflow(-reg[rt], reg[rs]);
				reg[rd] = reg[rs] - reg[rt];
				break;
		//=========================================================================
			case 0x24 :		// and
				reg[rd] = reg[rs] & reg[rt];
				break;
			case 0x25 :		// or
				reg[rd] = reg[rs] | reg[rt];
				break;
			case 0x26 :		// xor
				reg[rd] = reg[rs] ^ reg[rt];
				break;
			case 0x27 :		// nor
				reg[rd] = ~( reg[rs] | reg[rt] );
				break;
			case 0x28 :		// nand
				reg[rd] = ~( reg[rs] & reg[rt] );
				break;
			case 0x2a :		// slt
				// warning for sign comparison
				reg[rd] = reg[rs] < reg[rt] ? 1 : 0 ;
				break;
		//=========================================================================
			// shift
			case 0x00 :		// sll
				tempUns = (unsigned int)reg[rt];
				reg[rd] = tempUns << shamt;
				break;
			case 0x02 :		// srl
				tempUns = (unsigned int)reg[rt];
				reg[rd] = tempUns >> shamt;
				break;
			case 0x03 :		// sra
				reg[rd] = (int)reg[rt] >> shamt;
				break;
		//=========================================================================
			// jr
			case 0x08 :		break;
		//=========================================================================
			// mult, multu
			case 0x18 :		// mult
				printf("<< mult: rs: $%d, rt: $%d >>\n", rs, rt);
				printf("<< $%d: [%d], $%d: [%d] >>\n", rs, regStage[rs], rt, regStage[rt]);
				mult = reg[rs] * reg[rt];
				HI = ((unsigned long long)mult) >> 32;
				LO = mult & 0x00000000ffffffff;
				// err
				errorList[1] = m_hi && m_lo;
				m_hi = 1;	m_lo = 1;
				break;
			case 0x19 :		// multu
				mult = (long long)((unsigned int)reg[rs]) * ((unsigned int)reg[rt]);
				HI = mult >> 32;
				LO = mult & 0x00000000ffffffff;
				// err
				errorList[1] = m_hi && m_lo;
				m_hi = 1;	m_lo = 1;
				break;
		//=========================================================================
			// mfhi, mflo
			case 0x10 :		// mfhi
				reg[rd] = HI;
				m_hi = 0;
				break;
			case 0x12 :		// mflo
				reg[rd] = LO;
				m_lo = 0;
				break;
		}
	} else {
		switch(opcode) {
			// halt
			case 0x3f :		break;
			// J-type
			case 0x02 :		break;	// j C
			case 0x03 :		break;	// jal C
			// I-type
			case 0x08 :		// addi
				errorList[4] = isOverflow(reg[rs], immediate);
				reg[rt] = reg[rs] + immediate;
				break;
			case 0x09 :		// addiu
				reg[rt] = reg[rs] + immediate;
				break;
		// ============================================================
		// memory stuff in MEM
			case 0x23 :		// lw
			case 0x21 :		// lh
			case 0x25 :		// lhu
			case 0x20 :		// lb
			case 0x24 :		// lbu
			case 0x2b :		// sw
			case 0x29 :		// sh
			case 0x28 :		// sb
				errorList[4] = isOverflow(reg[rs], immediate);
				break;
		// ============================================================
			case 0x0f :		// lui
				reg[rt] = immediate << 16;
				break;
			case 0x0c :		// andi
				immediate = binToUnsInt(bin, 15, 0);
				reg[rt] = reg[rs] & immediate;
				break;
			case 0x0d :		// ori
				immediate = binToUnsInt(bin, 15, 0);
				reg[rt] = reg[rs] | immediate;
				break;
			case 0x0e :		// nori
				immediate = binToUnsInt(bin, 15, 0);
				reg[rt] = ~( reg[rs] | immediate );
				break;
			case 0x0a :		// slti
				// warning for sign comparison
				reg[rt] = reg[rs] < immediate ? 1 : 0 ;
				break;
			case 0x04 :		// beq
			case 0x05 :		// bne
			case 0x07 :		// bgtz
				errorList[4] = isOverflow(PC, immediate*4);
				break;
		}
	}
	// reg stage ++
	if (prev_ID_EX.isWritten) {
		prev_ID_EX.result = reg[prev_ID_EX.writeIndex];
		regStage[prev_ID_EX.writeIndex] = 1;
		//printf("ID->EX_%s_[%d]: [%d]\n", prev_ID_EX.instName, prev_ID_EX.writeIndex, regStage[prev_ID_EX.writeIndex]);
	}
	// pass to next stage
	next_EX_DM = prev_ID_EX;
}

extern int ALU(int A, int B, int fs);

#endif