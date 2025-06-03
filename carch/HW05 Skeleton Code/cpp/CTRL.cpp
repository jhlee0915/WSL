#include <iostream>
#include "CTRL.h"
#include "ALU.h"
#include "globals.h"


CTRL::CTRL() {}

void CTRL::controlSignal(uint32_t opcode, uint32_t funct, Controls *controls, int stall) {
	// FILLME
	controls->RegDst = (opcode == OP_RTYPE);
	controls->JR = (opcode == OP_RTYPE) && (funct == FUNCT_JR);
	controls->Jump = (opcode == OP_J) || (opcode == OP_JAL);
	controls->ALUSrc = (opcode != OP_RTYPE) && (opcode != OP_BEQ) && (opcode != OP_BNE);
	controls->MemtoReg = (opcode == OP_LW);
	controls->RegWrite = (opcode != OP_SW) && (opcode != OP_BEQ) && (opcode != OP_BNE) && (opcode!=OP_J) && !((opcode == OP_RTYPE) && (funct == FUNCT_JR));
	controls->MemRead = (opcode == OP_LW);
	controls->MemWrite = (opcode == OP_SW);
	controls->Branch = (opcode == OP_BEQ) || (opcode == OP_BNE);
	controls->SavePC = (opcode == OP_JAL);
	controls->SignExtend = (opcode == OP_ADDIU) || (opcode == OP_SLTI) || (opcode == OP_SLTIU) || (opcode == OP_LW) || (opcode == OP_SW) ||  (opcode == OP_BEQ) || (opcode == OP_BNE);
	if(opcode == OP_RTYPE){
		switch (funct) {
			case FUNCT_ADDU:
				controls->ALUOp = ALU_ADDU;
				break;
			case FUNCT_SUBU:
				controls->ALUOp = ALU_SUBU;
				break;
			case FUNCT_AND:
				controls->ALUOp = ALU_AND;
				break;
			case FUNCT_OR:
				controls->ALUOp = ALU_OR;
				break;
			case FUNCT_XOR:
				controls->ALUOp = ALU_XOR;
				break;
			case FUNCT_NOR:
				controls->ALUOp = ALU_NOR;
				break;
			case FUNCT_SLT:
				controls->ALUOp = ALU_SLT;
				break;
			case FUNCT_SLTU:
				controls->ALUOp = ALU_SLTU;
				break;
			case FUNCT_SLL:
				controls->ALUOp = ALU_SLL;
				break;
			case FUNCT_SRL:
				controls->ALUOp = ALU_SRL;
				break;
			case FUNCT_SRA:
				controls->ALUOp = ALU_SRA;
				break;
			default:
				break;
		}
	}
	else{
		switch (opcode)
		{
		case OP_BEQ:
			controls->ALUOp = ALU_EQ;
			break;
		case OP_BNE:
			controls->ALUOp = ALU_NEQ;
			break;
		case OP_LUI:
			controls->ALUOp = ALU_LUI;
			break;
		case OP_LW:
			controls->ALUOp = ALU_ADDU;
			break;
		case OP_SW:
			controls->ALUOp = ALU_ADDU;
			break;
		case OP_SLTI:
			controls->ALUOp = ALU_SLT;
			break;
		case OP_SLTIU:
			controls->ALUOp = ALU_SLTU;
			break;
		case OP_ORI:
			controls->ALUOp = ALU_OR;
			break;
		case OP_XORI:
			controls->ALUOp = ALU_XOR;
			break;
		case OP_ANDI:
			controls->ALUOp = ALU_AND;
			break;
		case OP_ADDIU:
			controls->ALUOp = ALU_ADDU;
			break;
		default:
			break;
		}
	}
	if(stall){
		controls->RegDst = 0;
		controls->JR = 0;
		controls->Jump = 0;
		controls->ALUSrc = 0;
		controls->MemtoReg = 0;
		controls->RegWrite = 0;
		controls->MemRead = 0;
		controls->MemWrite = 0;
		controls->Branch = 0;
		controls->SavePC = 0;
		controls->SignExtend = 0;
	}
	//SavePC SignExtend

}

void CTRL::splitInst(uint32_t inst, ParsedInst *parsed_inst) {
	// FILLME
	parsed_inst->opcode = inst>>26;
	parsed_inst->rs = (inst<<6)>>27;
	parsed_inst->rt = (inst<<11)>>27;
	parsed_inst->rd = (inst<<16)>>27;
	parsed_inst->shamt = (inst<<21)>>27;
	parsed_inst->funct = (inst<<26)>>26;
	parsed_inst->immi = (inst<<16)>>16;
	parsed_inst->immj = (inst<<6)>>6;
}

// Sign extension using bitwise shift
void CTRL::signExtend(uint32_t immi, uint32_t SignExtend, uint32_t *ext_imm) {
	// FILLME
	 if (SignExtend) {
        int32_t signed_imm = (int16_t)(immi);
        *ext_imm = (uint32_t)signed_imm;
    } else {
        *ext_imm = immi;
    }
}
