#ifndef CPU_H
#define CPU_H

#include<string>
#include<map>

#define S_MODE

#include "MainBus.h"

namespace nes {

    class CPU {
    public:
        using ADDR_MODE = Byte(CPU::*)(void);
        using OPERATION = Byte(CPU::*)(void);

        CPU();
        ~CPU();
        void connect(MainBus *_bus);
        void exe_instr();
        void clock();
        void reset(bool soft);
        void irq();
        void nmi();

        bool complete();

        void dma_cycles(Word cc);

        std::map<Word, std::string> disassemble(Word addr_start, Word addr_stop);

    private:
        bool fetch(Word addr, Byte& data);
        bool store(Word addr, Byte data);

        void detect_interrupt();
        void interrupt_sequence_start();
        void interrupt_sequence_end();
        void interrupt_sequence_clear();

        //AddrMode() and Opcodes() should return 0 or 1 to indicate
        //whether there's an additional cycle;

        //Adressing Modes
        Byte IMP(); Byte IMM(); Byte ZP0(); Byte ZPX();
        Byte ZPY(); Byte REL(); Byte ABS(); Byte ABX();
        Byte ABY(); Byte IND(); Byte IZX(); Byte IZY();
        //accumulator addressing could be included in IMP();

        //help to calculate extra cycle independently;
        Byte ABX_L();//absolute x indexed mode for load instructions only;
        Byte ABY_L();//absolute y indexed mode for load instructions only;
        Byte IZY_L();//indirect indexed mode for load instructions only;
        Byte IMP_B();//only used for BRK();

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

        //Unofficial opcodes catcher:
        Byte XXX();

        //Unofficail opcodes:
        //Byte AHX(); Byte ALR(); Byte ANC(); Byte ARR();
        //Byte AXS(); Byte DCP(); Byte ISC(); Byte LAS();
        //Byte LAX(); Byte NOP(); Byte RLA(); Byte RRA();
        //Byte SAX(); Byte SHX(); Byte SHY(); 
        Byte SLO();
        //Byte SRE(); Byte STP(); Byte TAS(); Byte XAA();

        //helper functions for easy implementation of opcodes:
        void set_flag(FLAG flag, bool test); //helper function for set flags;
        void adc(); //helper function for ADC() and SBC();
        void cmp(Byte reg); //helper function for CMP(), CPX(), CPY();
        void push(Byte val); //helper function for stack operation;
        void pull(Byte &val);
        void branch(bool test); //helper function for branch instrs;
        bool check_branch_cross();

    public://temporarily set to public for dubugging;
        Byte A = 0;    //accumulator;
        Byte X = 0, Y = 0; //index regs x, y;
        Word PC = 0;   //program counter;
        Byte S = 0;    //stack pointer;
        STATUS_FLAGS P; //status flags;
    private:
        Counter cycles = 0;
        Counter instr_counter = 0;

        Byte fetch_buf = 0;
        Byte addr_zp0 = 0;
        Word addr_abs = 0;
        Word addr_rel = 0;
        Byte cur_opcode = 0;
        
        Instr_Phase phase = Instr_Phase::instr_fetch;

        bool irq_pending = false;//internal signal for irq, which also indicates irq line is low at least starting one preceding cycle;
        bool irq_need = false;

        bool nmi_pending = false;//internal signal for nmi;
        bool nmi_need = false;
        bool prev_nmi_input = false;

        Word temp_word = 0;
        Byte temp_byte = 0;

        //CPU instruction entry;
        //struct INSTR {
        //    std::string name;
        //    Byte(CPU::* operate)(void) = nullptr;
        //    Byte(CPU::* addrMode)(void) = nullptr;
        //    Byte cycles = 0;
        //};
        //INSTR instr_mtx[kMAX_INSTR_MTX_SIZE];

        MainBus* mainBus = nullptr;

        static constexpr ADDR_MODE addr_mode_mtx[kMAX_INSTR_MTX_SIZE] = {
#ifdef S_MODE
            & CPU::IMP_B,
#else
            & CPU::IMP,
#endif
            //  0           1           2          3        4           5           6         7         8           9           A           B           C           D           E           F
                       & CPU::IZX,  & CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::ABS,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // 0
            & CPU::REL,& CPU::IZY_L,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY_L,& CPU::IMP,& CPU::ABY,& CPU::ABX_L,& CPU::ABX_L,& CPU::ABX,  & CPU::ABX, // 1
            & CPU::ABS,& CPU::IZX,  & CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::ABS,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // 2
            & CPU::REL,& CPU::IZY_L,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY_L,& CPU::IMP,& CPU::ABY,& CPU::ABX_L,& CPU::ABX_L,& CPU::ABX,  & CPU::ABX, // 3
            & CPU::IMP,& CPU::IZX,  & CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::ABS,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // 4
            & CPU::REL,& CPU::IZY_L,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY_L,& CPU::IMP,& CPU::ABY,& CPU::ABX_L,& CPU::ABX_L,& CPU::ABX,  & CPU::ABX, // 5
            & CPU::IMP,& CPU::IZX,  & CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::IND,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // 6
            & CPU::REL,& CPU::IZY_L,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY_L,& CPU::IMP,& CPU::ABY,& CPU::ABX_L,& CPU::ABX_L,& CPU::ABX,  & CPU::ABX, // 7
            & CPU::IMM,& CPU::IZX,  & CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::ABS,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // 8
            & CPU::REL,& CPU::IZY,  & CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPY,& CPU::ZPY,& CPU::IMP,& CPU::ABY,  & CPU::IMP,& CPU::ABY,& CPU::ABX,  & CPU::ABX,  & CPU::ABY,  & CPU::ABY, // 9
            & CPU::IMM,& CPU::IZX,  & CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::ABS,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // A
            & CPU::REL,& CPU::IZY_L,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPY,& CPU::ZPY,& CPU::IMP,& CPU::ABY_L,& CPU::IMP,& CPU::ABY,& CPU::ABX_L,& CPU::ABX_L,& CPU::ABY_L,& CPU::ABY, // B
            & CPU::IMM,& CPU::IZX,  & CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::ABS,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // C
            & CPU::REL,& CPU::IZY_L,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY_L,& CPU::IMP,& CPU::ABY,& CPU::ABX_L,& CPU::ABX_L,& CPU::ABX,  & CPU::ABX, // D
            & CPU::IMM,& CPU::IZX,  & CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,  & CPU::IMP,& CPU::IMM,& CPU::ABS,  & CPU::ABS,  & CPU::ABS,  & CPU::ABS, // E
            & CPU::REL,& CPU::IZY_L,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY_L,& CPU::IMP,& CPU::ABY,& CPU::ABX_L,& CPU::ABX_L,& CPU::ABX,  & CPU::ABX, // F
        };
        static constexpr OPERATION operation_mtx[kMAX_INSTR_MTX_SIZE] = {
            //  0           1           2          3        4           5           6         7         8           9        A           B           C           D           E       F
            & CPU::BRK,& CPU::ORA,& CPU::XXX,& CPU::SLO,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::SLO,& CPU::PHP,& CPU::ORA,& CPU::ASL_A, & CPU::XXX,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::SLO,
            & CPU::BPL,& CPU::ORA,& CPU::XXX,& CPU::SLO,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::SLO,& CPU::CLC,& CPU::ORA,& CPU::XXX,   & CPU::SLO,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::SLO,
            & CPU::JSR,& CPU::AND,& CPU::XXX,& CPU::XXX,& CPU::BIT,& CPU::AND,& CPU::ROL,& CPU::XXX,& CPU::PLP,& CPU::AND,& CPU::ROL_A, & CPU::XXX,& CPU::BIT,& CPU::AND,& CPU::ROL,& CPU::XXX,
            & CPU::BMI,& CPU::AND,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::AND,& CPU::ROL,& CPU::XXX,& CPU::SEC,& CPU::AND,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::AND,& CPU::ROL,& CPU::XXX,
            & CPU::RTI,& CPU::EOR,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::EOR,& CPU::LSR,& CPU::XXX,& CPU::PHA,& CPU::EOR,& CPU::LSR_A, & CPU::XXX,& CPU::JMP,& CPU::EOR,& CPU::LSR,& CPU::XXX,
            & CPU::BVC,& CPU::EOR,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::EOR,& CPU::LSR,& CPU::XXX,& CPU::CLI,& CPU::EOR,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::EOR,& CPU::LSR,& CPU::XXX,
            & CPU::RTS,& CPU::ADC,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::ADC,& CPU::ROR,& CPU::XXX,& CPU::PLA,& CPU::ADC,& CPU::ROR_A, & CPU::XXX,& CPU::JMP,& CPU::ADC,& CPU::ROR,& CPU::XXX,
            & CPU::BVS,& CPU::ADC,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::ADC,& CPU::ROR,& CPU::XXX,& CPU::SEI,& CPU::ADC,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::ADC,& CPU::ROR,& CPU::XXX,
            & CPU::XXX,& CPU::STA,& CPU::XXX,& CPU::XXX,& CPU::STY,& CPU::STA,& CPU::STX,& CPU::XXX,& CPU::DEY,& CPU::XXX,& CPU::TXA,   & CPU::XXX,& CPU::STY,& CPU::STA,& CPU::STX,& CPU::XXX,
            & CPU::BCC,& CPU::STA,& CPU::XXX,& CPU::XXX,& CPU::STY,& CPU::STA,& CPU::STX,& CPU::XXX,& CPU::TYA,& CPU::STA,& CPU::TXS,   & CPU::XXX,& CPU::XXX,& CPU::STA,& CPU::XXX,& CPU::XXX,
            & CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,& CPU::TAY,& CPU::LDA,& CPU::TAX,   & CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,
            & CPU::BCS,& CPU::LDA,& CPU::XXX,& CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,& CPU::CLV,& CPU::LDA,& CPU::TSX,   & CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,
            & CPU::CPY,& CPU::CMP,& CPU::XXX,& CPU::XXX,& CPU::CPY,& CPU::CMP,& CPU::DEC,& CPU::XXX,& CPU::INY,& CPU::CMP,& CPU::DEX,   & CPU::XXX,& CPU::CPY,& CPU::CMP,& CPU::DEC,& CPU::XXX,
            & CPU::BNE,& CPU::CMP,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::CMP,& CPU::DEC,& CPU::XXX,& CPU::CLD,& CPU::CMP,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::CMP,& CPU::DEC,& CPU::XXX,
            & CPU::CPX,& CPU::SBC,& CPU::XXX,& CPU::XXX,& CPU::CPX,& CPU::SBC,& CPU::INC,& CPU::XXX,& CPU::INX,& CPU::SBC,& CPU::NOP,   & CPU::SBC,& CPU::CPX,& CPU::SBC,& CPU::INC,& CPU::XXX,
            & CPU::BEQ,& CPU::SBC,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::SBC,& CPU::INC,& CPU::XXX,& CPU::SED,& CPU::SBC,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::SBC,& CPU::INC,& CPU::XXX,
        };
        static constexpr Byte base_cycle_mtx[kMAX_INSTR_MTX_SIZE] = {
          //0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            7,	6,	0,	8,	3,	3,	5,	5,	3,	2,	2,	2,	4,	4,	6,	6, // 0
            2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7, // 1
            6,	6,	0,	8,	3,	3,	5,	5,	4,	2,	2,	2,	4,	4,	6,	6, // 2
            2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7, // 3
            6,	6,	0,	8,	3,	3,	5,	5,	3,	2,	2,	2,	3,	4,	6,	6, // 4
            2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7, // 5
            6,	6,	0,	8,	3,	3,	5,	5,	4,	2,	2,	2,	5,	4,	6,	6, // 6
            2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7, // 7
            2,	6,	2,	6,	3,	3,	3,	3,	2,	2,	2,	2,	4,	4,	4,	4, // 8
            2,	6,	0,	6,	4,	4,	4,	4,	2,	5,	2,	5,	5,	5,	5,	5, // 9
            2,	6,	2,	6,	3,	3,	3,	3,	2,	2,	2,	2,	4,	4,	4,	4, // A
            2,	5,	0,	5,	4,	4,	4,	4,	2,	4,	2,	4,	4,	4,	4,	4, // B
            2,	6,	2,	8,	3,	3,	5,	5,	2,	2,	2,	2,	4,	4,	6,	6, // C
            2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7, // D
            2,	6,	2,	8,	3,	3,	5,	5,	2,	2,	2,	2,	4,	4,	6,	6, // E
            2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7, // F
        };
        static constexpr char instr_name_mtx[kMAX_INSTR_MTX_SIZE][4] = {
            //0     1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
            "BRK",	"ORA",	"STP",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",	"PHP",	"ORA",	"ASL",	"ANC",	"NOP",	"ORA",	"ASL",	"SLO",
            "BPL",	"ORA",	"STP",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",	"CLC",	"ORA",	"NOP",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",
            "JSR",	"AND",	"STP",	"RLA",	"BIT",	"AND",	"ROL",	"RLA",	"PLP",	"AND",	"ROL",	"ANC",	"BIT",	"AND",	"ROL",	"RLA",
            "BMI",	"AND",	"STP",	"RLA",	"NOP",	"AND",	"ROL",	"RLA",	"SEC",	"AND",	"NOP",	"RLA",	"NOP",	"AND",	"ROL",	"RLA",
            "RTI",	"EOR",	"STP",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",	"PHA",	"EOR",	"LSR",	"ALR",	"JMP",	"EOR",	"LSR",	"SRE",
            "BVC",	"EOR",	"STP",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",	"CLI",	"EOR",	"NOP",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",
            "RTS",	"ADC",	"STP",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",	"PLA",	"ADC",	"ROR",	"ARR",	"JMP",	"ADC",	"ROR",	"RRA",
            "BVS",	"ADC",	"STP",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",	"SEI",	"ADC",	"NOP",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",
            "NOP",	"STA",	"NOP",	"SAX",	"STY",	"STA",	"STX",	"SAX",	"DEY",	"NOP",	"TXA",	"XAA",	"STY",	"STA",	"STX",	"SAX",
            "BCC",	"STA",	"STP",	"AHX",	"STY",	"STA",	"STX",	"SAX",	"TYA",	"STA",	"TXS",	"TAS",	"SHY",	"STA",	"SHX",	"AHX",
            "LDY",	"LDA",	"LDX",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",	"TAY",	"LDA",	"TAX",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",
            "BCS",	"LDA",	"STP",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",	"CLV",	"LDA",	"TSX",	"LAS",	"LDY",	"LDA",	"LDX",	"LAX",
            "CPY",	"CMP",	"NOP",	"DCP",	"CPY",	"CMP",	"DEC",	"DCP",	"INY",	"CMP",	"DEX",	"AXS",	"CPY",	"CMP",	"DEC",	"DCP",
            "BNE",	"CMP",	"STP",	"DCP",	"NOP",	"CMP",	"DEC",	"DCP",	"CLD",	"CMP",	"NOP",	"DCP",	"NOP",	"CMP",	"DEC",	"DCP",
            "CPX",	"SBC",	"NOP",	"ISC",	"CPX",	"SBC",	"INC",	"ISC",	"INX",	"SBC",	"NOP",	"SBC",	"CPX",	"SBC",	"INC",	"ISC",
            "BEQ",	"SBC",	"STP",	"ISC",	"NOP",	"SBC",	"INC",	"ISC",	"SED",	"SBC",	"NOP",	"ISC",	"NOP",	"SBC",	"INC",	"ISC",
        };
    };

    //const CPU::ADDR_MODE addr_mode_mtx[kMAX_INSTR_MTX_SIZE] = {
    //    //  0           1           2          3        4           5           6         7         8           9       A           B           C           D           E       F
    //    & CPU::IMP,& CPU::IZX,& CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::ABS,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABX,& CPU::ABX,
    //    & CPU::ABS,& CPU::IZX,& CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::ABS,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABX,& CPU::ABX,
    //    & CPU::IMP,& CPU::IZX,& CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::ABS,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABX,& CPU::ABX,
    //    & CPU::IMP,& CPU::IZX,& CPU::IMP,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::IND,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABX,& CPU::ABX,
    //    & CPU::IMM,& CPU::IZX,& CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::ABS,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPY,& CPU::ZPY,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABY,& CPU::ABY,
    //    & CPU::IMM,& CPU::IZX,& CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::ABS,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPY,& CPU::ZPY,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABY,& CPU::ABY,
    //    & CPU::IMM,& CPU::IZX,& CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::ABS,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABX,& CPU::ABX,
    //    & CPU::IMM,& CPU::IZX,& CPU::IMM,& CPU::IZX,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::ZP0,& CPU::IMP,& CPU::IMM,& CPU::IMP,& CPU::IMM,& CPU::ABS,& CPU::ABS,& CPU::ABS,& CPU::ABS,
    //    & CPU::REL,& CPU::IZY,& CPU::IMP,& CPU::IZY,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::ZPX,& CPU::IMP,& CPU::ABY,& CPU::IMP,& CPU::ABY,& CPU::ABX,& CPU::ABX,& CPU::ABX,& CPU::ABX,
    //};
    //const CPU::OPERATION operation_mtx[kMAX_INSTR_MTX_SIZE] = {
    //    //  0           1           2          3        4           5           6         7         8           9        A           B           C           D           E       F
    //    & CPU::BRK,& CPU::ORA,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::XXX,& CPU::PHP,& CPU::ORA,& CPU::ASL_A, & CPU::XXX,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::XXX,
    //    & CPU::BPL,& CPU::ORA,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::XXX,& CPU::CLC,& CPU::ORA,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::ORA,& CPU::ASL,& CPU::XXX,
    //    & CPU::JSR,& CPU::AND,& CPU::XXX,& CPU::XXX,& CPU::BIT,& CPU::AND,& CPU::ROL,& CPU::XXX,& CPU::PLP,& CPU::AND,& CPU::ROL_A, & CPU::XXX,& CPU::BIT,& CPU::AND,& CPU::ROL,& CPU::XXX,
    //    & CPU::BMI,& CPU::AND,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::AND,& CPU::ROL,& CPU::XXX,& CPU::SEC,& CPU::AND,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::AND,& CPU::ROL,& CPU::XXX,
    //    & CPU::RTI,& CPU::EOR,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::EOR,& CPU::LSR,& CPU::XXX,& CPU::PHA,& CPU::EOR,& CPU::LSR_A, & CPU::XXX,& CPU::JMP,& CPU::EOR,& CPU::LSR,& CPU::XXX,
    //    & CPU::BVC,& CPU::EOR,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::EOR,& CPU::LSR,& CPU::XXX,& CPU::CLI,& CPU::EOR,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::EOR,& CPU::LSR,& CPU::XXX,
    //    & CPU::RTS,& CPU::ADC,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::ADC,& CPU::ROR,& CPU::XXX,& CPU::PLA,& CPU::ADC,& CPU::ROR_A, & CPU::XXX,& CPU::JMP,& CPU::ADC,& CPU::ROR,& CPU::XXX,
    //    & CPU::BVS,& CPU::ADC,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::ADC,& CPU::ROR,& CPU::XXX,& CPU::SEI,& CPU::ADC,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::ADC,& CPU::ROR,& CPU::XXX,
    //    & CPU::XXX,& CPU::STA,& CPU::XXX,& CPU::XXX,& CPU::STY,& CPU::STA,& CPU::STX,& CPU::XXX,& CPU::DEY,& CPU::XXX,& CPU::TXA,   & CPU::XXX,& CPU::STY,& CPU::STA,& CPU::STX,& CPU::XXX,
    //    & CPU::BCC,& CPU::STA,& CPU::XXX,& CPU::XXX,& CPU::STY,& CPU::STA,& CPU::STX,& CPU::XXX,& CPU::TYA,& CPU::STA,& CPU::TXS,   & CPU::XXX,& CPU::XXX,& CPU::STA,& CPU::XXX,& CPU::XXX,
    //    & CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,& CPU::TAY,& CPU::LDA,& CPU::TAX,   & CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,
    //    & CPU::BCS,& CPU::LDA,& CPU::XXX,& CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,& CPU::CLV,& CPU::LDA,& CPU::TSX,   & CPU::XXX,& CPU::LDY,& CPU::LDA,& CPU::LDX,& CPU::XXX,
    //    & CPU::CPY,& CPU::CMP,& CPU::XXX,& CPU::XXX,& CPU::CPY,& CPU::CMP,& CPU::DEC,& CPU::XXX,& CPU::INY,& CPU::CMP,& CPU::DEX,   & CPU::XXX,& CPU::CPY,& CPU::CMP,& CPU::DEC,& CPU::XXX,
    //    & CPU::BNE,& CPU::CMP,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::CMP,& CPU::DEC,& CPU::XXX,& CPU::CLD,& CPU::CMP,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::CMP,& CPU::DEC,& CPU::XXX,
    //    & CPU::CPX,& CPU::SBC,& CPU::XXX,& CPU::XXX,& CPU::CPX,& CPU::SBC,& CPU::INC,& CPU::XXX,& CPU::INX,& CPU::SBC,& CPU::NOP,   & CPU::XXX,& CPU::CPX,& CPU::SBC,& CPU::INC,& CPU::XXX,
    //    & CPU::BEQ,& CPU::SBC,& CPU::XXX,& CPU::XXX,& CPU::XXX,& CPU::SBC,& CPU::INC,& CPU::XXX,& CPU::SED,& CPU::SBC,& CPU::XXX,   & CPU::XXX,& CPU::XXX,& CPU::SBC,& CPU::INC,& CPU::XXX,
    //};
    //const Byte base_cycle_mtx[kMAX_INSTR_MTX_SIZE] = {
    //  //0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    //    7,	6,	0,	8,	3,	3,	5,	5,	3,	2,	2,	2,	4,	4,	6,	6,
    //    2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7,
    //    6,	6,	0,	8,	3,	3,	5,	5,	4,	2,	2,	2,	4,	4,	6,	6,
    //    2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7,
    //    6,	6,	0,	8,	3,	3,	5,	5,	3,	2,	2,	2,	3,	4,	6,	6,
    //    2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7,
    //    6,	6,	0,	8,	3,	3,	5,	5,	4,	2,	2,	2,	5,	4,	6,	6,
    //    2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7,
    //    2,	6,	2,	6,	3,	3,	3,	3,	2,	2,	2,	2,	4,	4,	4,	4,
    //    2,	6,	0,	6,	4,	4,	4,	4,	2,	5,	2,	5,	5,	5,	5,	5,
    //    2,	6,	2,	6,	3,	3,	3,	3,	2,	2,	2,	2,	4,	4,	4,	4,
    //    2,	5,	0,	5,	4,	4,	4,	4,	2,	4,	2,	4,	4,	4,	4,	4,
    //    2,	6,	2,	8,	3,	3,	5,	5,	2,	2,	2,	2,	4,	4,	6,	6,
    //    2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7,
    //    2,	6,	2,	8,	3,	3,	5,	5,	2,	2,	2,	2,	4,	4,	6,	6,
    //    2,	5,	0,	8,	4,	4,	6,	6,	2,	4,	2,	7,	4,	4,	7,	7,
    //};
    //const char instr_name_mtx[kMAX_INSTR_MTX_SIZE][4] = {
    //    //0     1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
    //    "BRK",	"ORA",	"STP",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",	"PHP",	"ORA",	"ASL",	"ANC",	"NOP",	"ORA",	"ASL",	"SLO",
    //    "BPL",	"ORA",	"STP",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",	"CLC",	"ORA",	"NOP",	"SLO",	"NOP",	"ORA",	"ASL",	"SLO",
    //    "JSR",	"AND",	"STP",	"RLA",	"BIT",	"AND",	"ROL",	"RLA",	"PLP",	"AND",	"ROL",	"ANC",	"BIT",	"AND",	"ROL",	"RLA",
    //    "BMI",	"AND",	"STP",	"RLA",	"NOP",	"AND",	"ROL",	"RLA",	"SEC",	"AND",	"NOP",	"RLA",	"NOP",	"AND",	"ROL",	"RLA",
    //    "RTI",	"EOR",	"STP",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",	"PHA",	"EOR",	"LSR",	"ALR",	"JMP",	"EOR",	"LSR",	"SRE",
    //    "BVC",	"EOR",	"STP",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",	"CLI",	"EOR",	"NOP",	"SRE",	"NOP",	"EOR",	"LSR",	"SRE",
    //    "RTS",	"ADC",	"STP",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",	"PLA",	"ADC",	"ROR",	"ARR",	"JMP",	"ADC",	"ROR",	"RRA",
    //    "BVS",	"ADC",	"STP",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",	"SEI",	"ADC",	"NOP",	"RRA",	"NOP",	"ADC",	"ROR",	"RRA",
    //    "NOP",	"STA",	"NOP",	"SAX",	"STY",	"STA",	"STX",	"SAX",	"DEY",	"NOP",	"TXA",	"XAA",	"STY",	"STA",	"STX",	"SAX",
    //    "BCC",	"STA",	"STP",	"AHX",	"STY",	"STA",	"STX",	"SAX",	"TYA",	"STA",	"TXS",	"TAS",	"SHY",	"STA",	"SHX",	"AHX",
    //    "LDY",	"LDA",	"LDX",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",	"TAY",	"LDA",	"TAX",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",
    //    "BCS",	"LDA",	"STP",	"LAX",	"LDY",	"LDA",	"LDX",	"LAX",	"CLV",	"LDA",	"TSX",	"LAS",	"LDY",	"LDA",	"LDX",	"LAX",
    //    "CPY",	"CMP",	"NOP",	"DCP",	"CPY",	"CMP",	"DEC",	"DCP",	"INY",	"CMP",	"DEX",	"AXS",	"CPY",	"CMP",	"DEC",	"DCP",
    //    "BNE",	"CMP",	"STP",	"DCP",	"NOP",	"CMP",	"DEC",	"DCP",	"CLD",	"CMP",	"NOP",	"DCP",	"NOP",	"CMP",	"DEC",	"DCP",
    //    "CPX",	"SBC",	"NOP",	"ISC",	"CPX",	"SBC",	"INC",	"ISC",	"INX",	"SBC",	"NOP",	"SBC",	"CPX",	"SBC",	"INC",	"ISC",
    //    "BEQ",	"SBC",	"STP",	"ISC",	"NOP",	"SBC",	"INC",	"ISC",	"SED",	"SBC",	"NOP",	"ISC",	"NOP",	"SBC",	"INC",	"ISC",
    //};

};//end nes

#endif