#ifndef ID_
#define ID_


extern void stalled(int rs, int rt)
{
	if (prev_IF_ID.opcode == 0x04 || prev_IF_ID.opcode == 0x05 || prev_IF_ID.opcode == 0x07 ||
		prev_IF_ID.opcode == 0x02 || prev_IF_ID.opcode == 0x03 ||
		(prev_IF_ID.opcode == 0 && prev_IF_ID.funct == 0x08) ) {
		if (regStage[rs] == 1 || regStage[rt] == 1) {
			isStalled = 1;
		} else if (regStage[rs] == 2 || regStage[rt] == 2) {
			if (prev_EX_DM.opcode >= 0x20 && prev_EX_DM.opcode <= 0x25) {	// lw
				isStalled = 1;
			} else {
				// get fwd from EX-DM to ID
				isStalled = 0;
				fwdToID.isForward_rs = (regStage[rs] == 2) ? 1 : 0 ;
				fwdToID.isForward_rt = (regStage[rt] == 2) ? 1 : 0 ;
				strcpy(fwdToID.stageFrom, "EX-DM");
				fwdToID.regIndex = prev_EX_DM.writeIndex;
			}
		} else	isStalled = 0;
	} else {
		if (regStage[rs] == 1 || regStage[rt] == 1) {
			if (prev_ID_EX.opcode >= 0x20 && prev_ID_EX.opcode <= 0x25) {	// lw
				isStalled = 1;
			} else {
				isStalled = 0;
			}
		} else if (regStage[rs] == 2 || regStage[rt] == 2) {
			isStalled = 0;
		} else	isStalled = 0;
	}
}
extern void ID(void)
{
	if (strcmp(prev_IF_ID.instName, "NOP")==0) {
		next_ID_EX = prev_IF_ID;
		return;
	}
	//
	if ( prev_IF_ID.opcode == 0 ) {
		// R-type
		switch(prev_IF_ID.funct) {
			// default
			case 0x20 :		// add
			case 0x21 :		// addu
			case 0x22 :		// sub
		//=========================================================================
			case 0x24 :		// and
			case 0x25 :		// or
			case 0x26 :		// xor
			case 0x27 :		// nor
			case 0x28 :		// nand
			case 0x2a :		// slt
				prev_IF_ID.rsNeed = prev_IF_ID.rtNeed = 1;
				//
				stalled(prev_IF_ID.rs, prev_IF_ID.rt);
				regStage[prev_IF_ID.rd] = 1;
				prev_IF_ID.origDst = reg[prev_IF_ID.rd];
				prev_IF_ID.writeIndex = prev_IF_ID.rd;
				prev_IF_ID.isWritten = 1;
				break;
		//=========================================================================
			// shift
			case 0x00 :		// sll
			case 0x02 :		// srl
			case 0x03 :		// sra
				prev_IF_ID.rtNeed = 1;
				//
				stalled(0, prev_IF_ID.rt);
				regStage[prev_IF_ID.rd] = 1;
				prev_IF_ID.origDst = reg[prev_IF_ID.rd];
				prev_IF_ID.writeIndex = prev_IF_ID.rd;
				prev_IF_ID.isWritten = 1;
				break;
		//=========================================================================
			// jr
			case 0x08 :		// jr
				prev_IF_ID.rsNeed = 1 ;
				//
				/*printf("JR: %d\n", prev_IF_ID.rs);
				printf("ra: 0x%08X\n", reg[31]);*/
				stalled(prev_IF_ID.rs, 0);
				if (!isStalled) {
					//	jump
					_index = ( reg[prev_IF_ID.rs] - iPC ) / 4 - index - 1 ;
					_pc = reg[prev_IF_ID.rs] - PC - 4 ;
					// flush 1 cycle
					isFlushed = 1;
				}				
				break;
		//=========================================================================
			// mult, multu
			case 0x18 :		// mult
			case 0x19 :		// multu
				prev_IF_ID.rsNeed = prev_IF_ID.rtNeed = 1;
				//
				stalled(prev_IF_ID.rs, prev_IF_ID.rt);
				prev_IF_ID._hi = HI;
				prev_IF_ID._lo = LO;
				prev_IF_ID.hilo = 1;
				break;
		//=========================================================================
			// mfhi, mflo
			case 0x10 :		// mfhi
			case 0x12 :		// mflo
				regStage[prev_IF_ID.rd] = 1;
				prev_IF_ID.origDst = reg[prev_IF_ID.rd];
				prev_IF_ID.writeIndex = prev_IF_ID.rd;
				prev_IF_ID.isWritten = 1;
				break;
		//=========================================================================
		}
	} else {
		switch(prev_IF_ID.opcode) {
			// halt
			case 0x3f :		break;
			// J-type
			case 0x02 :		// j C
				// flush
				isFlushed = 1;
				// jump
				_index = ( prev_IF_ID.address * 4 - iPC ) / 4 - _index - 1 ;
				_pc = prev_IF_ID.address * 4 - PC - 4 ;
				break;
			case 0x03 :		// jal C
				// flush 1 cycle
				isFlushed = 1;
				// reg
				regStage[31] = 1;
				prev_IF_ID.origDst = reg[31];
				prev_IF_ID.writeIndex = 31;
				prev_IF_ID.isWritten = 1;
				// jump
				_index = ( prev_IF_ID.address * 4 - iPC ) / 4 - index - 1 ;
				reg[31] = PC;
				_pc =  prev_IF_ID.address * 4 - PC - 4 ;
				break;
			// I-type
			case 0x2b :		// sw
			case 0x29 :		// sh
			case 0x28 :		// sb
				prev_IF_ID.rsNeed = prev_IF_ID.rtNeed = 1;
				//
				stalled(prev_IF_ID.rs, prev_IF_ID.rt);
				break;
			case 0x08 :		// addi
			case 0x09 :		// addiu
		//=========================================================================
			case 0x23 :		// lw
			case 0x21 :		// lh
			case 0x25 :		// lhu
			case 0x20 :		// lb
			case 0x24 :		// lbu
		//=========================================================================
			case 0x0c :		// andi
			case 0x0d :		// ori
			case 0x0e :		// nori
			case 0x0a :		// slti
				prev_IF_ID.rsNeed = 1;
				//
				stalled(prev_IF_ID.rs, 0);
				regStage[prev_IF_ID.rt] = 1;
				prev_IF_ID.origDst = reg[prev_IF_ID.rt];
				prev_IF_ID.writeIndex = prev_IF_ID.rt;
				prev_IF_ID.isWritten = 1;
				break;
			case 0x0f :		// lui
				regStage[prev_IF_ID.rt] = 1;
				prev_IF_ID.origDst = reg[prev_IF_ID.rt];
				prev_IF_ID.writeIndex = prev_IF_ID.rt;
				prev_IF_ID.isWritten = 1;
				break;
		// =======================================================
			// branch
			case 0x04 :		// beq
				// reg
				stalled(prev_IF_ID.rs, prev_IF_ID.rt);
				// branch
				if (reg[prev_IF_ID.rs] == reg[prev_IF_ID.rt] && !isStalled) {
					_index = prev_IF_ID.immediate - 1 ;
					_pc = prev_IF_ID.immediate * 4 - 4 ;
					isFlushed = 1;
				}	break;
			case 0x05 :		// bne
				// reg
				stalled(prev_IF_ID.rs, prev_IF_ID.rt);
				// branch
				if (reg[prev_IF_ID.rs] != reg[prev_IF_ID.rt] && !isStalled) {
					_index = prev_IF_ID.immediate - 1 ;
					_pc = prev_IF_ID.immediate * 4 - 4 ;
					isFlushed = 1;
				}	break;
			case 0x07 :		// bgtz
				stalled(prev_IF_ID.rs, 0);
				// branch
				if (reg[prev_IF_ID.rs] > 0 && !isStalled) {
					_index = prev_IF_ID.immediate - 1 ;
					_pc = prev_IF_ID.immediate * 4 - 4 ;
					isFlushed = 1;
				}	break;
		}
	}
	// pass to next stage
	next_ID_EX = prev_IF_ID;
	if (isStalled)	strcpy(next_ID_EX.instName, "NOP");
}



#endif