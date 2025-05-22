#ifndef CTRL_H
#define CTRL_H

#include <stdint.h>



class CTRL {
public:
    CTRL();
	// You can fix these if you want ...
	struct Controls {
		uint32_t RegDst; //R type인지 아닌지 보고 어떤 레지스터에 쓸지 판단
		//uint32_t Jump;
		//uint32_t Branch;
		//uint32_t JR;
		uint32_t MemRead; // 메모리 읽을건지
		uint32_t MemtoReg; // mem 값과 alu_result 중 뭘 쓸건지
		uint32_t MemWrite; // 메모리 쓸건지
		uint32_t ALUSrcA; // operand1 정하기, 0은 PC, 1은 rs_data
		uint32_t ALUSrcB; // operand2 정하기, 0은 rt_data, 1은 pc+4, 2는 sign extend, 3은 branch 
		uint32_t SignExtend;
		uint32_t RegWrite; //레지스터에 쓰는지
		uint32_t ALUOp; //무슨 연산 하는지
		uint32_t SavePC;
		uint32_t PCWriteCond; // branch 조건 만족할 때만 pc 업데이트
		uint32_t PCWrite; // 무조건 pc 업데이트
		uint32_t IorD; // mem에서 가지고 올게 명령어인지, 데이터인지
		uint32_t PCSource; // pc에 쓸 값 정하기, 0은 alu_result, 1은 ALUOut, 2는 jump, 3은 JR
		uint32_t IRWrite; // IR에 쓸지 말지
	};
	struct ParsedInst {
		uint32_t opcode;
		uint32_t rs;
		uint32_t rt;
		uint32_t rd;
		uint32_t shamt;
		uint32_t funct;
		uint32_t immi;
		uint32_t immj;
	};
	void controlSignal(uint32_t opcode, uint32_t funct, uint32_t *state, Controls *controls);
	void splitInst(uint32_t inst, ParsedInst *parsed_inst);
	void signExtend(uint32_t immi, uint32_t SignExtend, uint32_t *ext_imm);
};



#endif // CTRL_H
