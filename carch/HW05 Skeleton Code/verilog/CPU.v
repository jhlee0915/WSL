`timescale 1ns / 1ps


module CPU(
	input		clk,
	input		rst,
	output 		halt
	);

	//===============================
	// MEM/WB pipeline registers
	//===============================
	reg        MEM_WB_RegDst;
	reg        MEM_WB_Jump;
	reg        MEM_WB_Branch;
	reg        MEM_WB_JR;
	reg        MEM_WB_MemRead;
	reg        MEM_WB_MemtoReg;
	reg        MEM_WB_MemWrite;
	reg        MEM_WB_ALUSrc;
	reg        MEM_WB_SignExtend;
	reg        MEM_WB_RegWrite;
	reg [4:0]  MEM_WB_ALUOp;
	reg        MEM_WB_SavePC;

	reg [31:0] MEM_WB_PC;
	reg [31:0] MEM_WB_mem_data;
	reg [31:0] MEM_WB_alu_result;
	reg [4:0] MEM_WB_wr_reg;
	reg [4:0] MEM_WB_rs;
	reg [4:0] MEM_WB_rd;
	reg [4:0] MEM_WB_rt;
	reg [31:0] MEM_WB_inst;

	//===============================
	// EX/MEM pipeline registers
	//===============================
	reg        EX_MEM_RegDst;
	reg        EX_MEM_Jump;
	reg        EX_MEM_Branch;
	reg        EX_MEM_JR;
	reg        EX_MEM_MemRead;
	reg        EX_MEM_MemtoReg;
	reg        EX_MEM_MemWrite;
	reg        EX_MEM_ALUSrc;
	reg        EX_MEM_SignExtend;
	reg        EX_MEM_RegWrite;
	reg [4:0]  EX_MEM_ALUOp;
	reg        EX_MEM_SavePC;

	reg [31:0] EX_MEM_alu_result;
	reg [31:0] EX_MEM_wr_data;
	reg [4:0] EX_MEM_wr_reg;
	reg [31:0] EX_MEM_branch_addr;
	reg [31:0] EX_MEM_PC;
	reg [4:0] EX_MEM_rs;
	reg [4:0] EX_MEM_rd;
	reg [4:0] EX_MEM_rt;
	reg [31:0] EX_MEM_inst;

	//===============================
	// ID/EX pipeline registers
	//===============================
	reg        ID_EX_RegDst;
	reg        ID_EX_Jump;
	reg        ID_EX_Branch;
	reg        ID_EX_JR;
	reg        ID_EX_MemRead;
	reg        ID_EX_MemtoReg;
	reg        ID_EX_MemWrite;
	reg        ID_EX_ALUSrc;
	reg        ID_EX_SignExtend;
	reg        ID_EX_RegWrite;
	reg [5:0]  ID_EX_ALUOp;
	reg        ID_EX_SavePC;

	reg [31:0] ID_EX_PC;
	reg [4:0] ID_EX_dest;
	reg [31:0] ID_EX_rs_data;
	reg [31:0] ID_EX_rt_data;
	reg [4:0] ID_EX_rs;
	reg [4:0] ID_EX_rd;
	reg [4:0] ID_EX_rt;
	reg [31:0] ID_EX_immi;
	reg [31:0] ID_EX_immj;
	reg [4:0] ID_EX_shamt;
	reg [31:0] ID_EX_inst;

	//===============================
	// IF/ID pipeline registers
	//===============================
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
	wire [4:0]		wr_addr;
	wire [31:0]		wr_data;

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
	wire [4:0]	dest;

	assign halt				= (inst == 32'b0) && (IF_ID_inst == 32'b0) && (ID_EX_inst == 32'b0) && (EX_MEM_inst == 32'b0) && (MEM_WB_inst == 32'b0);

	assign operand1			= ID_EX_rs_data;
	assign operand2			= ID_EX_ALUSrc ? ID_EX_immi : ID_EX_rt_data;
	assign wr_data			= MEM_WB_MemtoReg ? MEM_WB_mem_data : (MEM_WB_SavePC ? MEM_WB_PC : MEM_WB_alu_result);
	assign wr_addr			= MEM_WB_wr_reg;
	assign dest				= RegDst ? rd : (SavePC ? 31 : rt);
	assign mem_write_data	= EX_MEM_wr_data;
	assign mem_addr			= EX_MEM_alu_result;

			//ID
	assign	opcode = IF_ID_inst>>26;
	assign	rs = (IF_ID_inst<<6)>>27;
	assign	rt = (IF_ID_inst<<11)>>27;
	assign	rd = (IF_ID_inst<<16)>>27;
	assign	shamt = (IF_ID_inst<<21)>>27;
	assign	funct = (IF_ID_inst<<26)>>26;
	assign	immi = (IF_ID_inst<<16)>>16;
	assign	immj = (IF_ID_inst<<6)>>6;
	assign  ext_imm = SignExtend ? {{16{immi[15]}}, immi} :  {16'b0, immi};

	assign	rd_addr1 = rs;
	assign	rd_addr2 = rt;

	always @(*) begin
	end


	// Update the Clock
	always @(posedge clk) begin
		if (rst) begin	
			PC = 0;
			IF_ID_inst = 32'b0;
			EX_MEM_Branch = '0;
			EX_MEM_alu_result = 0;
			ID_EX_Jump = '0;
			ID_EX_JR = '0;
			ID_EX_Branch = '0;
		end
		else begin
			//$display("IF: %h %h	    ID: %h %h     EX: %h %h      MEM: %h %h      WB: %h %h     STALL : %d", PC, inst, IF_ID_PC, IF_ID_inst, ID_EX_PC, ID_EX_inst, EX_MEM_PC, EX_MEM_inst, MEM_WB_PC, MEM_WB_inst, stall);
			if (ID_EX_Jump) begin
				
				PC			= { PC[31:28], ID_EX_immj, 2'b00 };
				//$display("jump %h", PC);
				ID_EX_RegDst     <= 1'b0;
				ID_EX_Jump       <= 1'b0;
				ID_EX_Branch     <= 1'b0;
				ID_EX_JR         <= 1'b0;
				ID_EX_MemRead    <= 1'b0;
				ID_EX_MemtoReg   <= 1'b0;
				ID_EX_MemWrite   <= 1'b0;
				ID_EX_ALUSrc     <= 1'b0;
				ID_EX_SignExtend <= 1'b0;
				ID_EX_RegWrite   <= 1'b0;
				ID_EX_ALUOp      <= 1'b0;
				ID_EX_SavePC     <= 1'b0;

				ID_EX_PC         <= 32'b0;
				ID_EX_dest       <= 32'b0;
				ID_EX_rs_data    <= 32'b0;
				ID_EX_rt_data    <= 32'b0;
				ID_EX_rs         <= 32'b0;
				ID_EX_rd         <= 32'b0;
				ID_EX_rt         <= 32'b0;
				ID_EX_immi       <= 32'b0;
				ID_EX_immj       <= 32'b0;
				ID_EX_shamt      <= 32'b0;
				ID_EX_inst       <= 32'b0;

		        IF_ID_PC         <= 32'b0;
        		IF_ID_inst       <= 32'b0;
			end
			else if (ID_EX_JR) begin
				PC 			= ID_EX_rs_data;
				//$display("jump %h", PC);
				ID_EX_RegDst     <= 1'b0;
				ID_EX_Jump       <= 1'b0;
				ID_EX_Branch     <= 1'b0;
				ID_EX_JR         <= 1'b0;
				ID_EX_MemRead    <= 1'b0;
				ID_EX_MemtoReg   <= 1'b0;
				ID_EX_MemWrite   <= 1'b0;
				ID_EX_ALUSrc     <= 1'b0;
				ID_EX_SignExtend <= 1'b0;
				ID_EX_RegWrite   <= 1'b0;
				ID_EX_ALUOp      <= 1'b0;
				ID_EX_SavePC     <= 1'b0;

				ID_EX_PC         <= 32'b0;
				ID_EX_dest       <= 32'b0;
				ID_EX_rs_data    <= 32'b0;
				ID_EX_rt_data    <= 32'b0;
				ID_EX_rs         <= 32'b0;
				ID_EX_rd         <= 32'b0;
				ID_EX_rt         <= 32'b0;
				ID_EX_immi       <= 32'b0;
				ID_EX_immj       <= 32'b0;
				ID_EX_shamt      <= 32'b0;
				ID_EX_inst       <= 32'b0;

		        IF_ID_PC         <= 32'b0;
        		IF_ID_inst       <= 32'b0;

			end
    		else if (EX_MEM_Branch && EX_MEM_alu_result) begin
				PC 			= EX_MEM_branch_addr;

				EX_MEM_Branch <= 0;
				EX_MEM_MemWrite <= 0;
				EX_MEM_RegWrite <= 0;

				ID_EX_RegDst     <= 1'b0;
				ID_EX_Jump       <= 1'b0;
				ID_EX_Branch     <= 1'b0;
				ID_EX_JR         <= 1'b0;
				ID_EX_MemRead    <= 1'b0;
				ID_EX_MemtoReg   <= 1'b0;
				ID_EX_MemWrite   <= 1'b0;
				ID_EX_ALUSrc     <= 1'b0;
				ID_EX_SignExtend <= 1'b0;
				ID_EX_RegWrite   <= 1'b0;
				ID_EX_ALUOp      <= 1'b0;
				ID_EX_SavePC     <= 1'b0;

				ID_EX_PC         <= 32'b0;
				ID_EX_dest       <= 32'b0;
				ID_EX_rs_data    <= 32'b0;
				ID_EX_rt_data    <= 32'b0;
				ID_EX_rs         <= 32'b0;
				ID_EX_rd         <= 32'b0;
				ID_EX_rt         <= 32'b0;
				ID_EX_immi       <= 32'b0;
				ID_EX_immj       <= 32'b0;
				ID_EX_shamt      <= 32'b0;
				ID_EX_inst       <= 32'b0;

		        IF_ID_PC         <= 32'b0;
        		IF_ID_inst       <= 32'b0;
			end
			else if (stall);
			else PC = PC + 4;
			//------------------------------------------------
			// MEM/WB <= EX/MEM
			//------------------------------------------------
			MEM_WB_RegDst     <= EX_MEM_RegDst;
			MEM_WB_Jump       <= EX_MEM_Jump;
			MEM_WB_Branch     <= EX_MEM_Branch;
			MEM_WB_JR         <= EX_MEM_JR;
			MEM_WB_MemRead    <= EX_MEM_MemRead;
			MEM_WB_MemtoReg   <= EX_MEM_MemtoReg;
			MEM_WB_MemWrite   <= EX_MEM_MemWrite;
			MEM_WB_ALUSrc     <= EX_MEM_ALUSrc;
			MEM_WB_SignExtend <= EX_MEM_SignExtend;
			MEM_WB_RegWrite   <= EX_MEM_RegWrite;
			MEM_WB_ALUOp      <= EX_MEM_ALUOp;
			MEM_WB_SavePC     <= EX_MEM_SavePC;

			MEM_WB_PC         <= EX_MEM_PC;
			MEM_WB_mem_data   <= mem_read_data;          // Data-memory read value
			MEM_WB_alu_result <= EX_MEM_alu_result;
			MEM_WB_wr_reg     <= EX_MEM_wr_reg;
			MEM_WB_rs         <= EX_MEM_rs;
			MEM_WB_rd         <= EX_MEM_rd;
			MEM_WB_rt         <= EX_MEM_rt;
			MEM_WB_inst       <= EX_MEM_inst;
			if(!(EX_MEM_Branch && EX_MEM_alu_result)) begin
			//------------------------------------------------
			// EX/MEM <= ID/EX
			//------------------------------------------------
			EX_MEM_RegDst     <= ID_EX_RegDst;
			EX_MEM_Jump       <= ID_EX_Jump;
			EX_MEM_Branch     <= ID_EX_Branch;
			EX_MEM_JR         <= ID_EX_JR;
			EX_MEM_MemRead    <= ID_EX_MemRead;
			EX_MEM_MemtoReg   <= ID_EX_MemtoReg;
			EX_MEM_MemWrite   <= ID_EX_MemWrite;
			EX_MEM_ALUSrc     <= ID_EX_ALUSrc;
			EX_MEM_SignExtend <= ID_EX_SignExtend;
			EX_MEM_RegWrite   <= ID_EX_RegWrite;
			EX_MEM_ALUOp      <= ID_EX_ALUOp;
			EX_MEM_SavePC     <= ID_EX_SavePC;

			EX_MEM_alu_result <= alu_result;
			EX_MEM_wr_data    <= ID_EX_rt_data;
			EX_MEM_wr_reg     <= ID_EX_dest;
			EX_MEM_branch_addr<= ID_EX_PC + (ID_EX_immi << 2);
			EX_MEM_PC         <= ID_EX_PC;
			EX_MEM_rs         <= ID_EX_rs;
			EX_MEM_rd         <= ID_EX_rd;
			EX_MEM_rt         <= ID_EX_rt;
			EX_MEM_inst       <= ID_EX_inst;
			
			if((!ID_EX_Jump)&&(!ID_EX_JR)) begin 
			
			//------------------------------------------------
			// ID/EX <= IF/ID (및 제어 유닛)
			//------------------------------------------------
			ID_EX_RegDst      <= RegDst;
			ID_EX_Jump        <= Jump;
			ID_EX_Branch      <= Branch;
			ID_EX_JR          <= JR;
			ID_EX_MemRead     <= MemRead;
			ID_EX_MemtoReg    <= MemtoReg;
			ID_EX_MemWrite    <= MemWrite;
			ID_EX_ALUSrc      <= ALUSrc;
			ID_EX_SignExtend  <= SignExtend;
			ID_EX_RegWrite    <= RegWrite;
			ID_EX_ALUOp       <= ALUOp;
			ID_EX_SavePC      <= SavePC;

			ID_EX_PC          <= IF_ID_PC;
			ID_EX_rs_data     <= rd_data1;
			ID_EX_rt_data     <= rd_data2;
			ID_EX_rs          <= rs;
			ID_EX_rd          <= rd;
			ID_EX_rt          <= rt;
			ID_EX_immi        <= ext_imm;
			ID_EX_immj        <= immj;
			ID_EX_shamt       <= shamt;
			ID_EX_inst        <= IF_ID_inst;
			ID_EX_dest        <= dest;
			if(!stall) begin
				//------------------------------------------------
				// IF/ID <= Fetch stage
				//------------------------------------------------
				IF_ID_PC          <= PC;
				IF_ID_inst        <= inst;
			end
			end
			end
		end
	end
	

	CTRL ctrl (
    .opcode    (opcode),
    .funct     (funct),
	.stall		(stall),

    .RegDst    (RegDst),
    .Jump      (Jump),
    .Branch    (Branch),
    .JR        (JR),
    .MemRead   (MemRead),
    .MemtoReg  (MemtoReg),
    .MemWrite  (MemWrite),
    .ALUSrc    (ALUSrc),
    .SignExtend(SignExtend),
    .RegWrite  (RegWrite),
    .ALUOp     (ALUOp),
    .SavePC    (SavePC)
	);

	RF rf (
    .clk      (clk),
    .rst      (rst),

    .rd_addr1 (rd_addr1),
    .rd_addr2 (rd_addr2),
    .rd_data1 (rd_data1),
    .rd_data2 (rd_data2),

    .RegWrite (MEM_WB_RegWrite),
    .wr_addr  (MEM_WB_wr_reg),
    .wr_data  (wr_data)
	);

	MEM mem (
    .clk            (clk),
    .rst            (rst),

    .inst_addr      (PC),          
    .inst           (inst),

    .mem_addr       (mem_addr),
    .MemWrite       (EX_MEM_MemWrite),
    .mem_write_data (mem_write_data),
    .mem_read_data  (mem_read_data)
	);
	
	
	ALU alu (
    .operand1   	(operand1),
    .operand2  	 	(operand2),
    .shamt  		(ID_EX_shamt),
    .funct 			(ID_EX_ALUOp),
    .alu_result 	(alu_result)
	);

	
	HAZARD hazard_unit (
	.rs              (rs),
	.rt              (rt),

	.opcode			(opcode),

	.ID_EX_dest      (ID_EX_dest),
	.ID_EX_RegWrite  (ID_EX_RegWrite),

	.EX_MEM_wr_reg   (EX_MEM_wr_reg),
	.EX_MEM_RegWrite (EX_MEM_RegWrite),

	.MEM_WB_wr_reg   (MEM_WB_wr_reg),
	.MEM_WB_RegWrite (MEM_WB_RegWrite),

	.stall           (stall)
	);

	
endmodule
