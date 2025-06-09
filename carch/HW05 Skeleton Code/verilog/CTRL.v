`timescale 1ns / 1ps
`include "GLOBAL.v"

module CTRL(
	// input opcode and funct
	input [5:0] opcode,
	input [5:0] funct,
	input 		stall,

	// output various ports
	output reg RegDst,
	output reg Jump,
	output reg Branch,
	output reg JR,
	output reg MemRead,
	output reg MemtoReg,
	output reg MemWrite,
	output reg ALUSrc,
	output reg SignExtend,
	output reg RegWrite,
	output reg [3:0] ALUOp,
	output reg SavePC
    );

	always @(*) begin
        // 기본 NOP
        RegDst     = 1'b0;
        Jump       = 1'b0;
        Branch     = 1'b0;
        JR         = 1'b0;
        MemRead    = 1'b0;
        MemtoReg   = 1'b0;
        MemWrite   = 1'b0;
        ALUSrc     = 1'b0;
        SignExtend = 1'b0;
        RegWrite   = 1'b0;
        SavePC     = 1'b0;
        ALUOp      = `ALU_ADDU;   // 디폴트 add

        if (!stall) begin
            RegDst = (opcode == `OP_RTYPE);
			JR = (opcode == `OP_RTYPE) && (funct == `FUNCT_JR);
			Jump = (opcode == `OP_J) || (opcode == `OP_JAL);
			ALUSrc = (opcode != `OP_RTYPE) && (opcode != `OP_BEQ) && (opcode != `OP_BNE);
			MemtoReg = (opcode == `OP_LW);
			RegWrite = (opcode != `OP_SW) && (opcode != `OP_BEQ) && (opcode != `OP_BNE) && (opcode!=`OP_J) && !((opcode == `OP_RTYPE) && (funct == `FUNCT_JR));
			MemRead = (opcode == `OP_LW);
			MemWrite = (opcode == `OP_SW);
			Branch = (opcode == `OP_BEQ) || (opcode == `OP_BNE);
			SavePC = (opcode == `OP_JAL);
			SignExtend = (opcode == `OP_ADDIU) || (opcode == `OP_SLTI) || (opcode == `OP_SLTIU) || (opcode == `OP_LW) || (opcode == `OP_SW) ||  (opcode == `OP_BEQ) || (opcode == `OP_BNE);
			if(opcode == `OP_RTYPE) begin
				case (funct)
					`FUNCT_ADDU: ALUOp = `ALU_ADDU;
					`FUNCT_SUBU: ALUOp = `ALU_SUBU;
					`FUNCT_AND: ALUOp = `ALU_AND;
					`FUNCT_OR: ALUOp = `ALU_OR;
					`FUNCT_XOR: ALUOp = `ALU_XOR;
					`FUNCT_NOR: ALUOp = `ALU_NOR;
					`FUNCT_SLT:		ALUOp = `ALU_SLT;
					`FUNCT_SLTU:	ALUOp = `ALU_SLTU;
					`FUNCT_SLL: 	ALUOp = `ALU_SLL;
					`FUNCT_SRL:		ALUOp = `ALU_SRL;
					`FUNCT_SRA:		ALUOp = `ALU_SRA;
				endcase
			end
			else begin
				case (opcode)
				`OP_BEQ: ALUOp = `ALU_EQ;
				`OP_BNE: ALUOp = `ALU_NEQ;
				`OP_LUI: ALUOp = `ALU_LUI;
				`OP_LW: ALUOp = `ALU_ADDU;
				`OP_SW: ALUOp = `ALU_ADDU;
				`OP_SLTI: ALUOp = `ALU_SLT;
				`OP_SLTIU: ALUOp = `ALU_SLTU;
				`OP_ORI: ALUOp = `ALU_OR;
				`OP_XORI: ALUOp = `ALU_XOR;
				`OP_ANDI: ALUOp = `ALU_AND;
				`OP_ADDIU: ALUOp = `ALU_ADDU;
				endcase
			end
        end
	end
endmodule
