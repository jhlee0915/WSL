#include <iostream>
#include <iomanip>
#include <bitset>
#include <stdint.h>
#include "ALU.h"
#include "globals.h"

using namespace std;

ALU::ALU() {}

void ALU::compute(uint32_t operand1, uint32_t operand2,
				uint32_t shamt, uint32_t aluop, uint32_t *alu_result) {
	switch (static_cast<ALUOp>(aluop))
	{
        case ALU_ADDU:
            *alu_result = operand1 + operand2;
            break;
        case ALU_AND:
            *alu_result = operand1 & operand2;
            break;
        case ALU_NOR:
            *alu_result = ~(operand1 | operand2);
            break;
        case ALU_OR:
            *alu_result = operand1 | operand2;
            break;
        case ALU_SLL:
            *alu_result = operand2 << shamt;
            break;
        case ALU_SRA:
            if(int(operand2) < 0) *alu_result = ~(~operand2 >> shamt);
            else *alu_result = operand2 >> shamt;
            break;
        case ALU_SRL:
            *alu_result = operand2 >> shamt;
            break;
        case ALU_SUBU:
            *alu_result = operand1 - operand2;
            break;
        case ALU_XOR:
            *alu_result = operand1 ^ operand2;
            break;
        case ALU_SLT:
            *alu_result = uint32_t(int(operand1) < int(operand2));
            break;
        case ALU_SLTU:
            *alu_result = operand1 < operand2;
            break;
        case ALU_EQ:
            *alu_result = operand1 == operand2;
            break;
        case ALU_NEQ:
            *alu_result = operand1 != operand2;
            break;
        case ALU_LUI:
            *alu_result = operand2 << 16;
            break;
		default:
            break;
	}
}

