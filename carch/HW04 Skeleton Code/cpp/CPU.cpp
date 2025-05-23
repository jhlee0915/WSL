#include <iomanip>
#include <iostream>
#include "CPU.h"
#include "globals.h"

#define VERBOSE 0

using namespace std;

#define debug 1

CPU::CPU() {}

// Reset stateful modules
void CPU::init(string inst_file) {
	// Initialize the register file
	rf.init(false);
	// Load the instructions from the memory
	mem.load(inst_file);
	// Reset the program counter
	PC = 0;
	ALUOut = 0;
	IR = 0;

	pasrsed_IR.funct = 0;
	pasrsed_IR.opcode = 0;
	MDR = 0;


	// Set the debugging status
	status = CONTINUE;
	state = 0;
}

// This is a cycle-accurate simulation
uint32_t CPU::tick() {
	// These are just one of the implementations ...

	// parsed & control signals (wire)
	CTRL::Controls controls;
	uint32_t ext_imm;

	// Default wires and control signals
	uint32_t operand1;
	uint32_t operand2;


	// You can declare your own wires (if you want ...)
	uint32_t alu_result;
	//...
	if(debug) cout<<"state: "<<state<<endl;
	if(debug) cout<<"PC: "<<hex<<PC<<endl;
	

	ctrl.controlSignal(pasrsed_IR.opcode, pasrsed_IR.funct, &state, &controls);

	// Access the memory
	if(controls.IorD == 0) mem.memAccess(PC, &IR, B, controls.MemRead, controls.MemWrite, controls.IorD);
	else mem.memAccess(ALUOut, &MDR, B, controls.MemRead, controls.MemWrite, controls.IorD);
	if(debug) cout<<"inst: "<<hex<<IR<<endl;
	if(debug) cout<<"MDR: "<<hex<<MDR<<endl;

	if (status != CONTINUE) return 0;

	// Split the instruction
	if(controls.IRWrite) ctrl.splitInst(IR, &pasrsed_IR);
	ctrl.signExtend(pasrsed_IR.immi, controls.SignExtend, &ext_imm);
	if (status != CONTINUE) return 0;


	rf.read(pasrsed_IR.rs, pasrsed_IR.rt, &A, &B);
	if(controls.SavePC) rf.write(31, PC, controls.RegWrite);
	rf.write(controls.RegDst ? pasrsed_IR.rd : pasrsed_IR.rt, controls.MemtoReg ? MDR : ALUOut, controls.RegWrite);

	if(controls.ALUSrcA == 0) operand1 = PC;
	else if(controls.ALUSrcA == 1) operand1 = A;
	if(controls.ALUSrcB == 0) operand2 = B;
	else if(controls.ALUSrcB == 1) operand2 = 4;
	else if(controls.ALUSrcB == 2) operand2 = ext_imm;
	else if(controls.ALUSrcB == 3) operand2 = ext_imm<<2;

	alu.compute(operand1, operand2, pasrsed_IR.shamt, controls.ALUOp, &alu_result);
	if(debug) cout<<"o1: "<<hex<<operand1<<endl;
	if(debug) cout<<"o2: "<<hex<<operand2<<endl;
	if(debug) cout<<"alu: "<<hex<<alu_result<<endl;
	if(debug) cout<<"aluout: "<<hex<<ALUOut<<endl;
	if (status != CONTINUE) return 0;

	if(controls.PCWrite || (controls.PCWriteCond && alu_result)){
		if(controls.PCSource == 0) PC = alu_result;
		else if(controls.PCSource == 1) PC = ALUOut;
		else if(controls.PCSource == 2) PC = (PC & 0xF0000000) | (pasrsed_IR.immj<<2);
		else if(controls.PCSource == 3) PC = A;
	}
	ALUOut = alu_result;
	if(debug) cout<<endl;
	return 1;
}

