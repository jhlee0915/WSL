#ifndef CPU_H
#define CPU_H


#include <stdint.h>
#include "ALU.h"
#include "RF.h"
#include "MEM.h"
#include "CTRL.h"

struct IF_ID_Latch {
	uint32_t PC;
    uint32_t inst;
};
struct ID_EX_Latch {
	CTRL::Controls controls;
	uint32_t PC;
    uint32_t dest; // for hazard detection
    uint32_t rs_data, rt_data, immi, immj, shamt;
    uint32_t rs, rd, rt; // for hazard detection
    uint32_t inst; //for debug

};
struct EX_MEM_Latch {
	CTRL::Controls controls;
    uint32_t alu_result;
    uint32_t wr_data;
    uint32_t wr_reg;
    uint32_t branch_addr;
	uint32_t PC; // for JAL
    uint32_t rs, rd, rt; // for hazard detection
    uint32_t inst; //for debug
};
struct MEM_WB_Latch {
	CTRL::Controls controls;
	uint32_t PC; // for JAL
    uint32_t mem_data;
    uint32_t alu_result;
    uint32_t wr_reg;
    uint32_t rs, rd, rt; // for hazard detection
    uint32_t inst; //for debug
};


class CPU {
public:
    CPU(); // Constructor
	void init(std::string inst_file);
    void jumpflush();
    void branchflush();
    int stall(CTRL::ParsedInst);
    uint32_t tick(); // Run simulation
    ALU alu;
    RF rf;
    CTRL ctrl;
	MEM mem;
    int sta;
    CTRL::Controls zero; // for init and flush
    IF_ID_Latch IF_ID;
	ID_EX_Latch ID_EX;
	EX_MEM_Latch EX_MEM;
	MEM_WB_Latch MEM_WB;

	// Act like a storage element
	uint32_t PC;
};

#endif // CPU_H

