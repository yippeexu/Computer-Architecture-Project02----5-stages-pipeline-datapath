#ifndef DM_
#define DM_

extern void DM(void)
{
	if (strcmp(prev_EX_DM.instName, "NOP")==0) {
		next_DM_WB = prev_EX_DM;
		return;
	}
	char buffer[32];
	int rs = prev_EX_DM.rs;
	int rt = prev_EX_DM.rt;
	int rd = prev_EX_DM.rd;
	int opcode = prev_EX_DM.opcode;
	int immediate = prev_EX_DM.immediate;
	unsigned int tempUns;
// =================================================
	switch(opcode) {
		// memory stuff in MEM
		case 0x23 :		// lw
			errorList[2] = ( (reg[rs]+immediate) > 1020 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			errorList[3] = (reg[rs]+immediate)%4 != 0 ? 1 : 0 ;
			// op
			if (!errorList[2]&&!errorList[3]) {
				reg[rt] = memory[ (reg[rs]+immediate)/4 ];
			}	break;
		case 0x21 :		// lh
			errorList[2] = ( (reg[rs]+immediate) > 1022 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			errorList[3] = (reg[rs]+immediate)%2 != 0 ? 1 : 0 ;
			// op
			if (!errorList[2]&&!errorList[3]) {
				reg[rt] = memory[ (reg[rs]+immediate)/4 ];
				intToBin(reg[rt], buffer, 32);
				reg[rt] = immediate%4 == 2 ? binToInt(buffer, 15, 0) : binToInt(buffer, 31, 16);
			}	break;
		case 0x25 :		// lhu
			errorList[2] = ( (reg[rs]+immediate) > 1022 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			errorList[3] = (reg[rs]+immediate)%2 != 0 ? 1 : 0 ;
			// op
			if (!errorList[2]&&!errorList[3]) {
				reg[rt] = memory[ (reg[rs]+immediate)/4 ];
				intToBin(reg[rt], buffer, 32);
				reg[rt] = immediate%4 == 2 ? binToUnsInt(buffer, 15, 0) : binToUnsInt(buffer, 31, 16);
			}	break;
		case 0x20 :		// lb
			errorList[2] = ( (reg[rs]+immediate) > 1023 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			// op
			if (!errorList[2]) {
				reg[rt] = memory[ (reg[rs]+immediate)/4 ];
				intToBin(reg[rt], buffer, 32);
				switch (immediate%4) {
					case 0 :	reg[rt] = binToInt(buffer, 31, 24);	break;
					case 1 :	reg[rt] = binToInt(buffer, 23, 16);	break;
					case 2 :	reg[rt] = binToInt(buffer, 15,  8);	break;
					case 3 :	reg[rt] = binToInt(buffer,  7,  0);	break;
				}
			}	break;
		case 0x24 :		// lbu
			errorList[2] = ( (reg[rs]+immediate) > 1023 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			// op
			if (!errorList[2]) {
				reg[rt] = memory[ (reg[rs]+immediate)/4 ];
				intToBin(reg[rt], buffer, 32);
				switch (immediate%4) {
					case 0 :	reg[rt] = binToUnsInt(buffer, 31, 24);	break;
					case 1 :	reg[rt] = binToUnsInt(buffer, 23, 16);	break;
					case 2 :	reg[rt] = binToUnsInt(buffer, 15,  8);	break;
					case 3 :	reg[rt] = binToUnsInt(buffer,  7,  0);	break;
				}
			}	break;
		case 0x2b :		// sw
			errorList[2] = ( (reg[rs]+immediate) > 1020 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			errorList[3] = (reg[rs]+immediate)%4 != 0 ? 1 : 0 ;
			// op
			if (!errorList[2]&&!errorList[3]) {
				memory[ (reg[rs]+immediate)/4 ] = reg[rt];
			}	break;
		case 0x29 :		// sh
			errorList[2] = ( (reg[rs]+immediate) > 1022 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			errorList[3] = (reg[rs] + immediate)%2 != 0 ? 1 : 0 ;
			// op
			if (!errorList[2]&&!errorList[3]) {
				tempUns = memory[ (reg[rs]+immediate)/4 ];
				memory[ (reg[rs]+immediate)/4 ] = ( tempUns & 0x0000FFFF ) | ( (reg[rt] & 0x0000FFFF) << 16 ) ;
			}	break;
		case 0x28 :		// sb
			errorList[2] = ( (reg[rs]+immediate) > 1023 || (reg[rs]+immediate) < 0 ) ? 1 : 0 ;
			// op
			if (!errorList[2]) {
				tempUns = memory[ (reg[rs]+immediate)/4 ];
				memory[ (reg[rs]+immediate)/4 ] = ( tempUns & 0x00FFFFFF ) | ( reg[rt] & 0x000000FF << 24 ) ;
			}	break;
	}
	// reg stage ++
	if (prev_EX_DM.isWritten) {
		regStage[prev_EX_DM.writeIndex] = 2;
		//printf("DM->WB_%s_[%d]: [%d]\n", prev_EX_DM.instName, prev_EX_DM.writeIndex, regStage[prev_EX_DM.writeIndex]);
	}
	// pass to next stage
	next_DM_WB = prev_EX_DM;
}
extern void WB(void)
{
	if (prev_DM_WB.isWritten && strcmp(prev_DM_WB.instName, "NOP") != 0) {
		if (prev_DM_WB.opcode >= 0x20 && prev_DM_WB.opcode <= 0x25)
			prev_DM_WB.result = reg[prev_DM_WB.writeIndex];
		regStage[prev_DM_WB.writeIndex] = 3;
		if (prev_DM_WB.writeIndex == 0)	errorList[0] = 1;
	}
	if (strcmp(prev_DM_WB.instName, "HALT") == 0)
		end = 1 ;
	next_WB_ID = prev_DM_WB;
}


#endif