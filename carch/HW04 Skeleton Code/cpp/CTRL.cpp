#include <iostream>
#include "CTRL.h"
#include "ALU.h"
#include "globals.h"

#define IF 0
#define ID 1
#define EX 2
#define ME 3
#define WB 4

CTRL::CTRL() {}

void CTRL::controlSignal(uint32_t opcode, uint32_t funct, uint32_t *state, Controls *controls) {
	// FILLME

	//IF
	if(*state == IF){
		controls->RegDst = 0; //R type인지 아닌지 보고 어떤 레지스터에 쓸지 판단
		controls->MemRead = 1; // 메모리 읽을건지
		controls->MemtoReg = 0; // mem 값과 alu_result 중 뭘 쓸건지
		controls->MemWrite = 0; // 메모리 쓸건지
		controls->ALUSrcA = 0; // operand1 정하기, 0은 PC, 1은 rs_data
		controls->ALUSrcB = 1; // operand2 정하기, 0은 rt_data, 1은 pc+4, 2는 sign extend, 3은 branch 
		controls->RegWrite = 0; //레지스터에 쓰는지
		controls->ALUOp = ALU_ADDU; //무슨 연산 하는지
		controls->PCWriteCond = 0; // branch 조건 만족할 때만 pc 업데이트
		controls->PCWrite = 1; // 무조건 pc 업데이트
		controls->IorD = 0; // mem에서 가지고 올게 명령어인지, 데이터인지
		controls->PCSource = 0; // pc에 쓸 값 정하기, 0은 alu_result, 1은 ALUOut, 2는 jump
		controls->IRWrite = 1; // IR에 쓸지 말지
		controls->SavePC = 0;
		controls->SignExtend = 0;
		*state = ID;
	}
	else if(*state == ID){
		//controls->RegDst; //R type인지 아닌지 보고 어떤 레지스터에 쓸지 판단
		controls->MemRead = 0; // 메모리 읽을건지
		//controls->MemtoReg; // mem 값과 alu_result 중 뭘 쓸건지
		controls->MemWrite = 0; // 메모리 쓸건지
		controls->ALUSrcA = 0; // operand1 정하기, 0은 PC, 1은 rs_data
		controls->ALUSrcB = 3; // operand2 정하기, 0은 rt_data, 1은 pc+4, 2는 sign extend, 3은 branch 
		controls->RegWrite = 0; //레지스터에 쓰는지
		controls->ALUOp = ALU_ADDU; //무슨 연산 하는지
		controls->PCWriteCond = 0; // branch 조건 만족할 때만 pc 업데이트
		controls->PCWrite = 0; // 무조건 pc 업데이트
		//controls->IorD; // mem에서 가지고 올게 명령어인지, 데이터인지
		//controls->PCSource; // pc에 쓸 값 정하기, 0은 alu_result, 1은 ALUOut, 2는 jump
		controls->IRWrite = 0; // IR에 쓸지 말지
		controls->SavePC = 0;
		controls->SignExtend = (opcode == OP_BEQ) || (opcode == OP_BNE);
		*state = EX;
	}
	else if(*state == EX){
		//controls->RegDst; //R type인지 아닌지 보고 어떤 레지스터에 쓸지 판단
		controls->MemRead = 0; // 메모리 읽을건지
		//controls->MemtoReg; // mem 값과 alu_result 중 뭘 쓸건지
		controls->MemWrite = 0; // 메모리 쓸건지
		controls->ALUSrcA = 1; // operand1 정하기, 0은 PC, 1은 rs_data
		if((opcode == OP_RTYPE) || (opcode == OP_BEQ) || (opcode == OP_BNE)) controls->ALUSrcB = 0; // operand2 정하기, 0은 rt_data, 1은 pc+4, 2는 sign extend, 3은 branch 
		else controls->ALUSrcB = 2; // operand2 정하기, 0은 rt_data, 1은 pc+4, 2는 sign extend, 3은 branch 
		controls->RegWrite = (opcode == OP_JAL); //레지스터에 쓰는지
		controls->PCWriteCond = ((opcode == OP_BEQ) || (opcode == OP_BNE)); // branch 조건 만족할 때만 pc 업데이트
		controls->PCWrite = ((opcode == OP_J) || (opcode == OP_JAL) || ((opcode == OP_RTYPE) && (funct == FUNCT_JR))); // 무조건 pc 업데이트
		//controls->IorD; // mem에서 가지고 올게 명령어인지, 데이터인지
		if((opcode == OP_BEQ) || (opcode == OP_BNE)) controls->PCSource = 1; // pc에 쓸 값 정하기, 0은 alu_result, 1은 ALUOut, 2는 jump
		else if((opcode == OP_J) || (opcode == OP_JAL)) controls->PCSource = 2; // pc에 쓸 값 정하기, 0은 alu_result, 1은 ALUOut, 2는 jump
		else if((opcode == OP_RTYPE) && (funct == FUNCT_JR)) controls->PCSource = 3;
		controls->IRWrite = 0; // IR에 쓸지 말지
		controls->SavePC = (opcode == OP_JAL);
		controls->SignExtend = (opcode == OP_ADDIU) || (opcode == OP_SLTI) || (opcode == OP_SLTIU) ||(opcode == OP_LW) || (opcode == OP_SW);
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
		if(opcode == OP_LW) *state = ME;
		else if(opcode == OP_SW) *state = ME;
		else if((opcode == OP_BEQ) || (opcode == OP_BNE) || (opcode == OP_J) || ((opcode == OP_RTYPE) && (funct == FUNCT_JR))) *state = IF;
		else *state = WB;
	}
	else if(*state == ME){
		//controls->RegDst; //R type인지 아닌지 보고 어떤 레지스터에 쓸지 판단
		controls->MemRead = (opcode == OP_LW); // 메모리 읽을건지
		//controls->MemtoReg; // mem 값과 alu_result 중 뭘 쓸건지
		controls->MemWrite = (opcode == OP_SW); // 메모리 쓸건지
		//controls->ALUSrcA; // operand1 정하기, 0은 PC, 1은 rs_data
		//controls->ALUSrcB; // operand2 정하기, 0은 rt_data, 1은 pc+4, 2는 sign extend, 3은 branch 
		controls->RegWrite = 0; //레지스터에 쓰는지
		//controls->ALUOp; //무슨 연산 하는지
		controls->PCWriteCond = 0; // branch 조건 만족할 때만 pc 업데이트
		controls->PCWrite = 0; // 무조건 pc 업데이트
		controls->IorD = 1; // mem에서 가지고 올게 명령어인지, 데이터인지
		//controls->PCSource; // pc에 쓸 값 정하기, 0은 alu_result, 1은 ALUOut, 2는 jump
		controls->IRWrite = 0; // IR에 쓸지 말지
		controls->SavePC = 0;
		controls->SignExtend = 0;
		if(opcode == OP_SW) *state = IF;
		if(opcode == OP_LW) *state = WB;
	}
	else if(*state == WB){
		controls->RegDst = (opcode == OP_RTYPE); //R type인지 아닌지 보고 어떤 레지스터에 쓸지 판단
		controls->MemRead = 0; // 메모리 읽을건지
		controls->MemtoReg = (opcode == OP_LW); // mem 값과 ALUOut 중 뭘 쓸건지
		controls->MemWrite = 0; // 메모리 쓸건지
		//controls->ALUSrcA; // operand1 정하기, 0은 PC, 1은 rs_data
		//controls->ALUSrcB; // operand2 정하기, 0은 rt_data, 1은 pc+4, 2는 sign extend, 3은 branch 
		controls->RegWrite = 1; //레지스터에 쓰는지
		//controls->ALUOp; //무슨 연산 하는지
		controls->PCWriteCond = 0; // branch 조건 만족할 때만 pc 업데이트
		controls->PCWrite = 0; // 무조건 pc 업데이트
		//controls->IorD; // mem에서 가지고 올게 명령어인지, 데이터인지
		//controls->PCSource; // pc에 쓸 값 정하기, 0은 alu_result, 1은 ALUOut, 2는 jump
		controls->IRWrite = 0; // IR에 쓸지 말지
		controls->SavePC = 0;
		controls->SignExtend = 0;
		*state = IF;
	}
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
