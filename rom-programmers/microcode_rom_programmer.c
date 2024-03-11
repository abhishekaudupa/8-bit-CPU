#include <stdio.h>
#include <string.h>

/*
 * This is the program to generate the control code image for the Instruction Decoder(ID) ROM that controls the peripheral lines of our 8-bit CPU. The ID determines which control lines to manipulate and how  * to manipulate them using the CPU flags, instruction code and the micro-instruction step as its inputs. These inputs - 3 CPU flags, 4 instruction code bits and 3 micro-instruction step bits, totaling to 10 * bits determine the input for the ROM (address) for which the ROM spits out a control word stored at that address, which control turn on/off the 16 control lines.
 *
 * Let's take the LDI instruction as an example. This instruction loads an immediate value (the operand) to the A-Register and this is how it goes:
 * Clock-cycle 1: Output contents of program counter to bus and let the memory address register read it (PO | MI).
 * Clock-cycle 2: Output contents of RAM to bus and let the instruction register read it. Also increment the program counter for the next instruction (RO | II | PE).
 * Clock-cycle 3: Output the contents of the instruction register to bus (the four LSB) and let the A-Register read it (IO | AI).
 * Clock-cycle 4: Idle.
 * Clock-cycle 5: Idle.
 *
 * So, we'd want the aboe to steps to happen regardless of the status flags and when the instruction is LDI (0001), each step corresponding to each micro-instruction. So we'd have to program the ROM as follows:
 * NF | CF | ZF | INSTR | STEP |    ROM ADDRESS     |     CONTROL WORD    |
 *  0 |  0 |  0 | 0001  | 000  | 0000001000 = 0x008 | (PO|MI)    = 0x0050 |
 *  0 |  0 |  0 | 0001  | 010  | 0000001010 = 0x00A | (RO|II|PE) = 0x008C |
 *  0 |  0 |  0 | 0001  | 011  | 0000001011 = 0x00B | (IO|AI)    = 0x4002 |
 *
 * we also have to make copies of this for all possible combinations of the 3 flags. Therefore that's a total of 8 copies of the above instruction at different addresses.
 *
 * Program is self-explanatory.
 */

typedef unsigned short ctrl_word;
typedef unsigned short rom_addr;

#define ROM_ADDR_SIZE 		10
#define ROM_SIZE 		(1 << ROM_ADDR_SIZE)
#define INST_CYCLE_SIZE 	5

#define ST 	0x8000		//halt clock line on.
#define AI 	0x4000		//A-Register input line on.
#define AO 	0x2000		//A-Register output line on.
#define BI 	0x1000		//B-Register input line on.
#define DO	0x0800		//Display register input line on.
#define EO	0x0400		//ALU output line on.
#define SU	0x0200		//Subtract enable line on.
#define RI	0x0100		//RAM input line on.
#define RO	0x0080		//RAM output line on.
#define MI	0x0040		//Memory Address Register input line on.
#define PI	0x0020		//Program counter input line on.
#define PO	0x0010		//Program counter output line on.
#define PE	0x0008		//Program counter count enable line on.
#define II	0x0004		//Instruction Register input line on.
#define IO	0x0002		//Instruction Register output line on.
#define FI	0x0001		//CPU Flags register input line on.
#define IDLE	0X0000		//All lines off.

#define LDI 	1
#define LDA 	2
#define STA 	3
#define ADD 	4
#define SUB 	5
#define HLT 	6
#define JMP 	7
#define NOP 	8
#define JC 	9
#define JZ 	10
#define JN 	11
#define OUT	12

#define FETCH 	MI | PO, 	\
    		RO | II | PE 	\

int main() {

    //instruction 1, load immediate value to A-Register.
    ctrl_word ldi[INST_CYCLE_SIZE] = {
	FETCH,
	IO | AI,
	IDLE,
	IDLE
    };

    //instr. 2, load RAM stored value to A-Register.
    ctrl_word lda[INST_CYCLE_SIZE] = {
	FETCH,
	IO | MI,
	RO | AI,
	IDLE
    };

    //instr. 3, store value in A-Register to RAM.
    ctrl_word sta[INST_CYCLE_SIZE] = {
	FETCH,
	IO | MI,
	AO | RI,
	IDLE
    };

    //instr. 4, add RAM stored value to A-Register.
    ctrl_word add[INST_CYCLE_SIZE] = {
	FETCH,
	IO | MI,
	RO | BI,
	EO | AI | FI
    };
    
    //instr. 5, subtract RAM stored value from A-Register.
    ctrl_word sub[INST_CYCLE_SIZE] = {
	FETCH,
	IO | MI,
	RO | BI,
	SU | EO | AI | FI
    };

    //instr. 6, computer halt.
    ctrl_word hlt[INST_CYCLE_SIZE] = {
	FETCH,
	ST,
	IDLE,
	IDLE
    };

    //instr. 7, unconditional jump.
    ctrl_word jmp[INST_CYCLE_SIZE] = {
	FETCH,
	IO | PI,
	IDLE,
	IDLE
    };

    //instr. 8, no operation/idle.
    ctrl_word nop[INST_CYCLE_SIZE] = {
	FETCH,
	IDLE,
	IDLE,
	IDLE
    };

    //instr. 9, output A-Register to display register.
    ctrl_word out[INST_CYCLE_SIZE] = {
	FETCH,
	AO | DO,
	IDLE,
	IDLE
    };

    //instr. 10, jump if carry.
    ctrl_word jc[INST_CYCLE_SIZE];

    //instr. 11, jump if zero.
    ctrl_word jz[INST_CYCLE_SIZE];

    //instr. 12, jump if negative.
    ctrl_word jn[INST_CYCLE_SIZE];

    //conditional jump and uniconditional jump  micro-instructions are identical.
    memcpy(jc, jmp, sizeof(jmp));
    memcpy(jz, jmp, sizeof(jmp));
    memcpy(jn, jmp, sizeof(jmp));

    //instruction decoder ROM image.
    ctrl_word rom[ROM_SIZE];

    //zero out the ROM image.
    memset(rom, IDLE, sizeof(rom));

    //instruction list array.
    ctrl_word *insts[] = {
	ldi,
	lda,
	sta,
	add,
	sub,
	hlt,
	jmp,
	nop,
	jc,
	jz,
	jn,
	out
    };

    //iterate for all possible combinations of CPU flags.
    for(unsigned char NF = 0; NF <= 1; ++NF) {
	for(unsigned char CF = 0; CF <= 1; ++CF) {
	    for(unsigned char ZF = 0; ZF <= 1; ++ZF) {

		//iterate thru instruction 1(LDI) to 12(OUT).
		for(unsigned char instr = LDI; instr <= OUT; ++instr) {

		    //iterate thru each micro-instruction step.
		    for(unsigned char step = 0; step < INST_CYCLE_SIZE; ++step){

			//ROM address is NF|CF|ZF|INSTRUCTION(4 bits)|STEP(3 bits), for a total of 10 bits.
			rom_addr addr = (NF << 9) | (CF << 8) | (ZF << 7) | (instr << 3) | step;

			//control word at the said address, 16-bit wide, controlling 16 peripheral lines.
			ctrl_word c_word = insts[instr - 1][step];

			//if micro-instruction step 2...
			if(step == 2) {

			    //render conditional jump instructions as NOPs if corresponding flags aren't set.
			    if(CF == 0 && instr == JC)
				c_word = IDLE;
			    if(ZF == 0 && instr == JZ)
				c_word = IDLE;
			    if(NF == 0 && instr == JN)
				c_word = IDLE;
			}
			
			//set the control word at the appropriate address in the ROM image.
			rom[addr] = c_word;
		    }
		}

	    }
	}
    }

    //first two address of the instruction decoder ROM is the fetch cycle.
    rom[0] = MI | PO;
    rom[1] = RO | II | PE;

    //write the ROM image out to a new text file.
    FILE *hex_out = fopen("mc_rom.txt", "wb");

    //file header.
    fprintf(hex_out, "v2.0 raw\n");

    //write out all bytes.
    for(rom_addr i = 0; i < ROM_SIZE; ++i) {
	if(i && i % 16 == 0)
	    fprintf(hex_out, "\n");
	fprintf(hex_out, "%04x ", rom[i]);
    }

    //close the file.
    fclose(hex_out);
}
