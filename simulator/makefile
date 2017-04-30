CMP = gcc
EVM = -std=c99
SRC = *.c
OUT = single_cycle.out
CHK = diff
CPY = copy

DEL  = del
BIN  = *.bin
SNAP = snapshot.rpt
ERR  = error_dump.rpt

SMPPATH = ..\testcase

all: $(SRC)
	$(CMP) $(SRC) -o $(OUT) $(EVM)
	$(CPY) $(SMPPATH)\$(BIN) .
	$(OUT)
check: $(SNAP) $(ERR)
	$(CHK) $(SNAP) $(SMPPATH)
	$(CHK) $(ERR) $(SMPPATH)
clean: $(SNAP) $(BIN) $(ERR) $(OUT)
	$(DEL) $(BIN) $(SNAP) $(ERR) $(OUT)
