#include <iomanip>
#include <iostream>
#include "CPU.h"
#include "globals.h"

#define VERBOSE 0

using namespace std;

CPU::CPU() {}

// Reset stateful modules
void CPU::init(string inst_file) {
	// Initialize the register file
	rf.init(false);
	// Load the instructions from the memory
	mem.load(inst_file);
	// Reset the program counter
	PC = 0;

	zero.ALUOp = 0;
	zero.ALUSrc = 0;
	zero.Branch = 0;
	zero.JR = 0;
	zero.Jump = 0;
	zero.MemRead = 0;
	zero.MemtoReg = 0;
	zero.MemWrite = 0;
	zero.RegDst = 0;
	zero.RegWrite = 0;
	zero.SavePC = 0;
	zero.SignExtend = 0;

	MEM_WB.controls = zero;
	MEM_WB.PC = 0;
	MEM_WB.mem_data = 0;
	MEM_WB.alu_result = 0;
	MEM_WB.wr_reg = 0;
	MEM_WB.rs = 0;
	MEM_WB.rd = 0;
	MEM_WB.rt = 0;

	EX_MEM.controls = zero;
	EX_MEM.alu_result = 0;
	EX_MEM.wr_data = 0;
	EX_MEM.wr_reg = 0;
	EX_MEM.branch_addr = 0;
	EX_MEM.PC = 0;
	EX_MEM.rs = 0;
	EX_MEM.rd = 0;
	EX_MEM.rt = 0;

	ID_EX.controls = zero;
	ID_EX.PC = 0;
	ID_EX.rs_data = 0;
	ID_EX.rt_data = 0;
	ID_EX.rs = 0;
	ID_EX.rd = 0;
	ID_EX.rt = 0;
	ID_EX.immi = 0;
	ID_EX.immj = 0;
	ID_EX.shamt = 0;

	IF_ID.PC = 0;
	IF_ID.inst = 0;

	// Set the debugging status
	status = CONTINUE;
}

void CPU::flush(){
	ID_EX.controls = zero;
	ID_EX.PC = 0;
	ID_EX.rs_data = 0;
	ID_EX.rt_data = 0;
	ID_EX.rs = 0;
	ID_EX.rd = 0;
	ID_EX.rt = 0;
	ID_EX.immi = 0;
	ID_EX.immj = 0;
	ID_EX.shamt = 0;

	IF_ID.PC = 0;
	IF_ID.inst = 0;
}

// This is a cycle-accurate simulation
uint32_t CPU::tick() {

	// wire for instruction
	uint32_t inst;

	// parsed & control signals (wire)
	CTRL::ParsedInst parsed_inst;
	CTRL::Controls controls;
	uint32_t ext_imm;

	// Default wires and control signals
	uint32_t rs_data, rt_data;
	uint32_t wr_addr;
	uint32_t wr_data;
	uint32_t operand1;
	uint32_t operand2;
	uint32_t alu_result;

	uint32_t mem_data;
	//...


	// IF
	mem.imemAccess(PC, &inst);
	if(EX_MEM.controls.Branch && EX_MEM.alu_result){
		PC = EX_MEM.branch_addr;
		flush();
	}
	else if(ID_EX.controls.Jump) PC = (PC & 0xF0000000) | (ID_EX.immj<<2);
	else if(ID_EX.controls.JR) PC = ID_EX.rs_data;
	else PC = PC + 4;
	if (status != CONTINUE) return 0;


	// ID
	ctrl.splitInst(IF_ID.inst, &parsed_inst);
	ctrl.controlSignal(parsed_inst.opcode, parsed_inst.funct, &controls);
	ctrl.signExtend(parsed_inst.immi, controls.SignExtend, &ext_imm);

	if (status != CONTINUE) return 0;

	rf.read(parsed_inst.rs, parsed_inst.rt, &rs_data, &rt_data);


	// EX
	operand1 = ID_EX.rs_data;
	if(ID_EX.controls.ALUSrc) operand2 = ID_EX.immi;
	else operand2 = ID_EX.rt_data;

	alu.compute(operand1, operand2, ID_EX.shamt, ID_EX.controls.ALUOp, &alu_result);
	if (status != CONTINUE) return 0;


	// MEM
	mem.dmemAccess(EX_MEM.alu_result, &mem_data, EX_MEM.wr_data, EX_MEM.controls.MemRead, EX_MEM.controls.MemWrite);
	if (status != CONTINUE) return 0;


	// WB
	if(MEM_WB.controls.MemtoReg) wr_data = MEM_WB.mem_data;
	else if(MEM_WB.controls.SavePC) wr_data = MEM_WB.PC;
	else wr_data = MEM_WB.alu_result;

	rf.write(MEM_WB.wr_reg, wr_data, MEM_WB.controls.RegWrite);



	// Latch update
	MEM_WB.controls = EX_MEM.controls;
	MEM_WB.PC = EX_MEM.PC;
	MEM_WB.mem_data = mem_data;
	MEM_WB.alu_result = EX_MEM.alu_result;
	MEM_WB.wr_reg = EX_MEM.wr_reg;
	MEM_WB.rs = EX_MEM.rs;
	MEM_WB.rd = EX_MEM.rd;
	MEM_WB.rt = EX_MEM.rt;

	EX_MEM.controls = ID_EX.controls;
	EX_MEM.alu_result = alu_result;
	EX_MEM.wr_data = ID_EX.rt_data;
	if(ID_EX.controls.RegDst) EX_MEM.wr_reg = parsed_inst.rd;
	else if(ID_EX.controls.SavePC) EX_MEM.wr_reg = 31;
	else EX_MEM.wr_reg = parsed_inst.rt;
	EX_MEM.branch_addr = ID_EX.PC + ID_EX.immi<<2;
	EX_MEM.PC = ID_EX.PC;
	EX_MEM.rs = ID_EX.rs;
	EX_MEM.rd = ID_EX.rd;
	EX_MEM.rt = ID_EX.rt;

	ID_EX.controls = controls;
	ID_EX.PC = IF_ID.PC;
	ID_EX.rs_data = rs_data;
	ID_EX.rt_data = rt_data;
	ID_EX.rs = parsed_inst.rs;
	ID_EX.rd = parsed_inst.rd;
	ID_EX.rt = parsed_inst.rt;
	ID_EX.immi = ext_imm;
	ID_EX.immj = parsed_inst.immj;
	ID_EX.shamt = parsed_inst.shamt;

	IF_ID.PC = PC;
	IF_ID.inst = inst;

	return 1;
}

//TODO: data hazard detection, branch flush