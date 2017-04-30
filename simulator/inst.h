#ifndef INST_
#define INST_
#define bool int

//================================
char regTable[32][6];
char instTable[64][6];
char funcTable[64][6];

int * inst;
int reg[32];
int regStage[32];
unsigned int memory[256];

unsigned int SP;
unsigned int iPC;
unsigned int PC;
unsigned int HI;
unsigned int LO;

int index;
int m_hi;
int m_lo;
int _pc;
int _index;

int memNum;
int instNum;
int cycle;

bool errorList[5];
//================================
// terminate
bool end;
int halt;
// hazard
bool isStalled;// ID
bool isFlushed;

// forwarding
struct Fwd {
	int regIndex;
	char stageFrom[10];
	//
	bool isForward_rs;
	bool isForward_rt;
};
struct Fwd fwdToID;
struct Fwd fwdToEX;

// pipeline register
struct PipeReg {
	char instName[10];
	int inst;

	int opcode;
	int rs;
	int rt;
	int rd;
	int address;
	int immediate;
	int funct;
	int shamt;
	// reg
	int origDst;
	int result;
	int writeIndex;
	bool isWritten;
	// rs, rt
	bool rsNeed;
	bool rtNeed;
	// hi lo
	unsigned int _hi;
	unsigned int _lo;
	bool hilo;
};
//	IF
struct PipeReg next_IF_ID;
//	ID
struct PipeReg prev_IF_ID;
struct PipeReg next_ID_EX;
//	EX
struct PipeReg prev_ID_EX;
struct PipeReg next_EX_DM;
//	DM
struct PipeReg prev_EX_DM;
struct PipeReg next_DM_WB;
//	WB
struct PipeReg prev_DM_WB;
struct PipeReg next_WB_ID;
struct PipeReg final;

//================================

extern void readInst(void);

extern int isOverflow(int a, int b);

extern void printErr(void);

extern void printRpt(int index);

#endif