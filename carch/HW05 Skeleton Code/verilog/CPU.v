`timescale 1ns / 1ps


module CPU(
	input		clk,
	input		rst,
	output 		halt
	);

	// MEM/WB pipeline registers
	reg [1:0]  MEM_WB_RegDst;
	reg [1:0]  MEM_WB_Jump;
	reg [1:0]  MEM_WB_Branch;
	reg [1:0]  MEM_WB_JR;
	reg [1:0]  MEM_WB_MemRead;
	reg [1:0]  MEM_WB_MemtoReg;
	reg [1:0]  MEM_WB_MemWrite;
	reg [1:0]  MEM_WB_ALUSrc;
	reg [1:0]  MEM_WB_SignExtend;
	reg [1:0]  MEM_WB_RegWrite;
	reg [1:0]  MEM_WB_ALUOp;
	reg [1:0]  MEM_WB_SavePC;

	reg [31:0] MEM_WB_PC;
	reg [31:0] MEM_WB_mem_data;
	reg [31:0] MEM_WB_alu_result;
	reg [31:0] MEM_WB_wr_reg;
	reg [31:0] MEM_WB_rs;
	reg [31:0] MEM_WB_rd;
	reg [31:0] MEM_WB_rt;
	reg [31:0] MEM_WB_inst;

	// EX/MEM pipeline registers
	reg [1:0]  EX_MEM_RegDst;
	reg [1:0]  EX_MEM_Jump;
	reg [1:0]  EX_MEM_Branch;
	reg [1:0]  EX_MEM_JR;
	reg [1:0]  EX_MEM_MemRead;
	reg [1:0]  EX_MEM_MemtoReg;
	reg [1:0]  EX_MEM_MemWrite;
	reg [1:0]  EX_MEM_ALUSrc;
	reg [1:0]  EX_MEM_SignExtend;
	reg [1:0]  EX_MEM_RegWrite;
	reg [1:0]  EX_MEM_ALUOp;
	reg [1:0]  EX_MEM_SavePC;

	reg [31:0] EX_MEM_alu_result;
	reg [31:0] EX_MEM_wr_data;
	reg [31:0] EX_MEM_wr_reg;
	reg [31:0] EX_MEM_branch_addr;
	reg [31:0] EX_MEM_PC;
	reg [31:0] EX_MEM_rs;
	reg [31:0] EX_MEM_rd;
	reg [31:0] EX_MEM_rt;
	reg [31:0] EX_MEM_inst;

	// ID/EX pipeline registers
	reg [1:0]  ID_EX_RegDst;
	reg [1:0]  ID_EX_Jump;
	reg [1:0]  ID_EX_Branch;
	reg [1:0]  ID_EX_JR;
	reg [1:0]  ID_EX_MemRead;
	reg [1:0]  ID_EX_MemtoReg;
	reg [1:0]  ID_EX_MemWrite;
	reg [1:0]  ID_EX_ALUSrc;
	reg [1:0]  ID_EX_SignExtend;
	reg [1:0]  ID_EX_RegWrite;
	reg [1:0]  ID_EX_ALUOp;
	reg [1:0]  ID_EX_SavePC;

	reg [31:0] ID_EX_PC;
	reg [31:0] ID_EX_dest;
	reg [31:0] ID_EX_rs_data;
	reg [31:0] ID_EX_rt_data;
	reg [31:0] ID_EX_rs;
	reg [31:0] ID_EX_rd;
	reg [31:0] ID_EX_rt;
	reg [31:0] ID_EX_immi;
	reg [31:0] ID_EX_immj;
	reg [31:0] ID_EX_shamt;
	reg [31:0] ID_EX_inst;

	// IF/ID pipeline registers
	reg [31:0] IF_ID_PC;
	reg [31:0] IF_ID_inst;

	
	// Split the instructions
	// Instruction-related wires
	wire [31:0]		inst;
	wire [5:0]		opcode;
	wire [4:0]		rs;
	wire [4:0]		rt;
	wire [4:0]		rd;
	wire [4:0]		shamt;
	wire [5:0]		funct;
	wire [15:0]		immi;
	wire [25:0]		immj;

	// Control-related wires
	wire			RegDst;
	wire			Jump;
	wire 			Branch;
	wire 			JR;
	wire			MemRead;
	wire			MemtoReg;
	wire 			MemWrite;
	wire			ALUSrc;
	wire			SignExtend;
	wire			RegWrite;
	wire [3:0]		ALUOp;
	wire			SavePC;
	wire			stall;

	// Sign extend the immediate
	wire [31:0]		ext_imm;

	// RF-related wires
	wire [4:0]		rd_addr1;
	wire [4:0]		rd_addr2;
	wire [31:0]		rd_data1;
	wire [31:0]		rd_data2;
	reg [4:0]		wr_addr;
	reg [31:0]		wr_data;

	// MEM-related wires
	wire [31:0]		mem_addr;
	wire [31:0]		mem_write_data;
	wire [31:0]		mem_read_data;

	// ALU-related wires
	wire [31:0]		operand1;
	wire [31:0]		operand2;
	wire [31:0]		alu_result;

	// Define PC
	reg [31:0]	PC;
	reg [31:0]	PC_next;

	// Define the wires

	assign halt				= (inst == 32'b0);

	assign operand1			= ID_EX_rs_data;
	assign operand2			= ID_EX_ALUSrc ? ID_EX_immi : ID_EX_rt_data;
	assign wr_data			= MEM_WB_MemtoReg ? MEM_WB_mem_data : (MEM_WB_SavePC ? MEM_WB_PC : MEM_WB_alu_result);
	assign wr_addr			= MEM_WB_wr_reg;

	always @(*) begin

		//ID
		opcode = IF_ID_inst>>26;
		rs = (IF_ID_inst<<6)>>27;
		rt = (IF_ID_inst<<11)>>27;
		rd = (IF_ID_inst<<16)>>27;
		shamt = (IF_ID_inst<<21)>>27;
		funct = (IF_ID_inst<<26)>>26;
		immi = (IF_ID_inst<<16)>>16;
		immj = (IF_ID_inst<<6)>>6;

		if (SignExtend) begin
    	    ext_imm = {{16{immi[15]}}, immi};
   		end else begin
	        ext_imm = {16'b0, immi};
    	end

	end


	// Update the Clock
	always @(posedge clk) begin
		if (rst)	PC <= 0;
		else begin
			PC <= PC_next;
		end
	end
	

	CTRL ctrl (clk, rst, opcode, funct);

	RF rf ();

	MEM mem ();
	
	ALU alu ();
	
endmodule
