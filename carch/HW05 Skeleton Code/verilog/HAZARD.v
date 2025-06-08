`timescale 1ns / 1ps
`include "GLOBAL.v"

module CTRL(
	// input
	input [4:0] rs,
	input [4:0] rt,
    input [5:0] opcode,
    input [5:0] ID_EX_RegWrite,
    input [5:0] EX_MEM_RegWrite,
    input [5:0] MEM_WB_RegWrite,

	// output
	output reg stall
    );

    wire users;
    wire usert;

	always @(*) begin
		stall = 0;
        users = 0;
        usert = 0;

        case (opcode)
            `OP_RTYPE:  users = 1; usert = 1;
            `OP_BEQ:    users = 1; usert = 1;
            `OP_BNE:    users = 1; usert = 1;
            `OP_LW:     users = 1;
            `OP_SW:     users = 1; usert = 1;
            `OP_SLTI:   users = 1;
            `OP_SLTIU:  users = 1;
            `OP_ORI:    users = 1;
            `OP_XORI:   users = 1;
            `OP_ANDI:   users = 1;
            `OP_ADDIU:  users = 1; usert = 1;
        endcase

	    if((rs == ID_EX.dest)&&(users)&&(ID_EX_RegWrite)) stall = 1;
	    if((rs == EX_MEM.wr_reg)&&(users)&&(EX_MEM_RegWrite)) stall = 1;
	    if((rs == MEM_WB.wr_reg)&&(users)&&(MEM_WB_RegWrite)) stall = 1;

	    if((rt == ID_EX.dest)&&(usert)&&(ID_EX_RegWrite)) stall = 1;
	    if((rt == EX_MEM.wr_reg)&&(usert)&&(EX_MEM_RegWrite)) stall = 1;
	    if((rt == MEM_WB.wr_reg)&&(usert)&&(MEM_WB_RegWrite)) stall = 1;
	end
endmodule
