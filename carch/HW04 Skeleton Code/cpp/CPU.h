#ifndef CPU_H
#define CPU_H


#include <stdint.h>
#include "ALU.h"
#include "RF.h"
#include "MEM.h"
#include "CTRL.h"

class CPU {
public:
    CPU(); // Constructor
	void init(std::string inst_file);
    uint32_t tick(); // Run simulation
    ALU alu;
    RF rf;
    CTRL ctrl;
	MEM mem;
    uint32_t state;

	// Act like a storage element
	uint32_t PC;
    uint32_t ALUOut;
    uint32_t A;
    uint32_t B;
    uint32_t IR;
    CTRL::ParsedInst pasrsed_IR;
    uint32_t MDR;
};

#endif // CPU_H

