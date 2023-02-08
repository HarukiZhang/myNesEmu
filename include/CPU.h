#ifndef CPU_H
#define CPU_H

#include<string>
#include<map>

#include "MainBus.h"

namespace nes {

    class CPU {
    public:
        CPU();
        ~CPU();
        void connect(MainBus *_bus);
        void exe_instr();
        void clock();
        void reset();
        void irq();
        void nmi();

        bool complete();

        void dma_cycles(Byte cc);

        std::map<Word, std::string> disassemble(Word addr_start, Word addr_stop);

    private:
        bool fetch(Word addr, Byte &data);
        bool store(Word addr, Byte data);

        //AddrMode() and Opcodes() should return 0 or 1 to indicate
        //whether there's an additional cycle;

        //Adressing Modes
        Byte IMP(); Byte IMM(); Byte ZP0(); Byte ZPX();
        Byte ZPY(); Byte REL(); Byte ABS(); Byte ABX();
        Byte ABY(); Byte IND(); Byte IZX(); Byte IZY();
        //accumulator addressing could be included in IMP();

        //Official opcodes:
        Byte ADC(); Byte AND(); Byte ASL(); Byte BCC(); Byte BCS(); 
        Byte BEQ(); Byte BIT(); Byte BMI(); Byte BNE(); Byte BPL(); 
        Byte BRK(); Byte BVC(); Byte BVS(); Byte CLC(); Byte CLD(); 
        Byte CLI(); Byte CLV(); Byte CMP(); Byte CPX(); Byte CPY(); 
        Byte DEC(); Byte DEX(); Byte DEY(); Byte EOR(); Byte INC(); 
        Byte INX(); Byte INY(); Byte JMP(); Byte JSR(); Byte LDA(); 
        Byte LDX(); Byte LDY(); Byte LSR(); Byte NOP(); Byte ORA(); 
        Byte PHA(); Byte PHP(); Byte PLA(); Byte PLP(); Byte ROL(); 
        Byte ROR(); Byte RTI(); Byte RTS(); Byte SBC(); Byte SEC(); Byte SED(); 
        Byte SEI(); Byte STA(); Byte STX(); Byte STY(); Byte TAX(); 
        Byte TAY(); Byte TSX(); Byte TXA(); Byte TXS(); Byte TYA(); 

        Byte ASL_A(); //ASL {ACCUM}
        Byte LSR_A(); //LSR {ACCUM}
        Byte ROL_A(); //ROL {ACCUM}
        Byte ROR_A(); //ROR {ACCUM}

        //helper functions for easy implementation of opcodes:
        void set_flag(FLAG flag, bool test); //helper function for set flags;
        void adc(); //helper function for ADC() and SBC();
        void cmp(Byte reg); //helper function for CMP(), CPX(), CPY();
        void push(Byte val); //helper function for stack operation;
        void pull(Byte &val);
        void branch(bool test); //helper function for branch instrs;

        //Unofficial opcodes catcher:
        Byte XXX();

    public://temporarily set to public for dubugging;
        Byte A = 0;    //accumulator;
        Byte X = 0, Y = 0; //index regs x, y;
        Word PC = 0;   //program counter;
        Byte S = 0;    //stack pointer;
        STATUS_FLAGS P; //status flags;
    private:
        Counter cycles = 0;

        Byte fetch_buf = 0;
        Byte addr_zp0 = 0;
        Word addr_abs = 0;
        Word addr_rel = 0;
        Byte cur_opcode = 0;

        Word temp_word = 0;
        Byte temp_byte = 0;

        //CPU instruction entry;
        struct INSTR {
            std::string name;
            Byte(CPU::* operate)(void) = nullptr;
            Byte(CPU::* addrMode)(void) = nullptr;
            Byte cycles = 0;
        };
        INSTR instr_mtx[kMAX_INSTR_MTX_SIZE];

        MainBus *mainBus = nullptr;

    };

};//end nes

#endif