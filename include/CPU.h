#ifndef CPU_H
#define CPU_H

#include<string>

#include "MainBus.h"

namespace nes {

    class CPU {
    public:

        enum FLAG : Byte {
            C = 0b1,
            Z = 0b10,
            I = 0b100,
            D = 0b1000,
            B = 0b10000,
            U = 0b100000,
            V = 0b1000000,
            N = 0b10000000,
        };

        struct INSTR {
            std::string name;
            Byte (CPU::*operate)(void) = nullptr;
            Byte (CPU::*addrMode)(void) = nullptr;
            Byte cycles = 0;
        };

        CPU();
        ~CPU();
        void connect(MainBus *_bus);
        void exe_instr();
        void clock();
        void reset();
        void irq();
        void nmi();

        inline void dma_cycles(Byte cc);

    private:
        inline bool fetch(Word addr, Byte &data);
        inline bool store(Word addr, Byte data);
        inline void setFlag(FLAG flag, bool test);

        //AddrMode() and Opcodes() should return 0 or 1 to indicate
        //whether there's an additional cycle;

        //Adressing Modes
        Byte IMP(); Byte IMM(); Byte ZP0(); Byte ZPX();
        Byte ZPY(); Byte REL(); Byte ABS(); Byte ABX();
        Byte ABY(); Byte IND(); Byte IZX(); Byte IZY();
        //accumulator addressing could be included in IMP();

        //Opcodes
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

        Byte XXX();//illegal opcodes catcher;
    private:
        Byte A;    //accumulator;
        Byte X, Y; //index regs x, y;
        Word PC;   //program counter;
        Byte S;    //stack pointer;
        //cpu status flags reg;
        union STATUS_FLAGS {
            struct {
                Byte C : 1;      //carry;              bit0
                Byte Z : 1;      //zero
                Byte I : 1;      //interrupt disable
                Byte D : 1;      //decimal mode
                Byte B : 1;      //break command
                Byte U : 1;      //unused
                Byte V : 1;      //overflow
                Byte N : 1;      //negative;           bit7
            };
            Byte val;
        } P;

        Byte fetch_buf;
        Byte addr_zp0;
        Word addr_abs;
        Word addr_rel;
        Byte cur_opcode;
        Byte cycles;

        struct INSTR instr_mtx[kMAX_INSTR_MTX_SIZE];

        MainBus *mainBus = nullptr;

    };

};//end nes

#endif