#include <iostream>

#include "CPU.h"

namespace nes {

    CPU::CPU(){
        instr_mtx[	0x00	] = {	"BRK"	, 	&CPU::BRK	, 	&CPU::IMP	, 	7	};
        instr_mtx[	0x01	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x05	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x06	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x08	] = {	"PHP"	, 	&CPU::PHP	, 	&CPU::IMP	, 	3	};
        instr_mtx[	0x09	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x0A	] = {	"ASL"	, 	&CPU::ASL_A	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x0D	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x0E	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x10	] = {	"BPL"	, 	&CPU::BPL	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x11	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x15	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x16	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x18	] = {	"CLC"	, 	&CPU::CLC	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x19	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0x1A	] = {	"NOP"	, 	&CPU::NOP	, 	&CPU::IMP	, 	2	}; //unofficial
        instr_mtx[	0x1D	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0x1E	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0x20	] = {	"JSR"	, 	&CPU::JSR	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x21	] = {	"AND"	, 	&CPU::AND	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x24	] = {	"BIT"	, 	&CPU::BIT	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x25	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x26	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x28	] = {	"PLP"	, 	&CPU::PLP	, 	&CPU::IMP	, 	4	};
        instr_mtx[	0x29	] = {	"AND"	, 	&CPU::AND	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x2A	] = {	"ROL"	, 	&CPU::ROL_A	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x2C	] = {	"BIT"	, 	&CPU::BIT	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x2D	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x2E	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x30	] = {	"BMI"	, 	&CPU::BMI	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x31	] = {	"AND"	, 	&CPU::AND	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x35	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x36	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x38	] = {	"SEC"	, 	&CPU::SEC	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x39	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0x3A	] = {	"NOP"	, 	&CPU::NOP	, 	&CPU::IMP	, 	2	}; //unofficial
        instr_mtx[	0x3D	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0x3E	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0x40	] = {	"RTI"	, 	&CPU::RTI	, 	&CPU::IMP	, 	6	};
        instr_mtx[	0x41	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x45	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x46	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x48	] = {	"PHA"	, 	&CPU::PHA	, 	&CPU::IMP	, 	3	};
        instr_mtx[	0x49	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x4A	] = {	"LSR"	, 	&CPU::LSR_A	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x4C	] = {	"JMP"	, 	&CPU::JMP	, 	&CPU::ABS	, 	3	};
        instr_mtx[	0x4D	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x4E	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x50	] = {	"BVC"	, 	&CPU::BVC	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x51	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x55	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x56	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x58	] = {	"CLI"	, 	&CPU::CLI	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x59	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0x5A	] = {	"NOP"	, 	&CPU::NOP	, 	&CPU::IMP	, 	2	}; //unofficial
        instr_mtx[	0x5D	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0x5E	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0x60	] = {	"RTS"	, 	&CPU::RTS	, 	&CPU::IMP	, 	6	};
        instr_mtx[	0x61	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x65	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x66	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x68	] = {	"PLA"	, 	&CPU::PLA	, 	&CPU::IMP	, 	4	};
        instr_mtx[	0x69	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x6A	] = {	"ROR"	, 	&CPU::ROR_A	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x6C	] = {	"JMP"	, 	&CPU::JMP	, 	&CPU::IND	, 	5	};
        instr_mtx[	0x6D	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x6E	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x70	] = {	"BVS"	, 	&CPU::BVS	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x71	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x75	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x76	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x78	] = {	"SEI"	, 	&CPU::SEI	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x79	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0x7A	] = {	"NOP"	, 	&CPU::NOP	, 	&CPU::IMP	, 	2	}; //unofficial
        instr_mtx[	0x7D	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0x7E	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0x81	] = {	"STA"	, 	&CPU::STA	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x84	] = {	"STY"	, 	&CPU::STY	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x85	] = {	"STA"	, 	&CPU::STA	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x86	] = {	"STX"	, 	&CPU::STX	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x88	] = {	"DEY"	, 	&CPU::DEY	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x8A	] = {	"TXA"	, 	&CPU::TXA	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x8C	] = {	"STY"	, 	&CPU::STY	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x8D	] = {	"STA"	, 	&CPU::STA	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x8E	] = {	"STX"	, 	&CPU::STX	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x90	] = {	"BCC"	, 	&CPU::BCC	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x91	] = {	"STA"	, 	&CPU::STA	, 	&CPU::IZY	, 	6	};
        instr_mtx[	0x94	] = {	"STY"	, 	&CPU::STY	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x95	] = {	"STA"	, 	&CPU::STA	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x96	] = {	"STX"	, 	&CPU::STX	, 	&CPU::ZPY	, 	4	};
        instr_mtx[	0x98	] = {	"TYA"	, 	&CPU::TYA	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x99	] = {	"STA"	, 	&CPU::STA	, 	&CPU::ABY	, 	5	};
        instr_mtx[	0x9A	] = {	"TXS"	, 	&CPU::TXS	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x9D	] = {	"STA"	, 	&CPU::STA	, 	&CPU::ABX	, 	5	};
        instr_mtx[	0xA0	] = {	"LDY"	, 	&CPU::LDY	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0xA1	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0xA2	] = {	"LDX"	, 	&CPU::LDX	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0xA4	] = {	"LDY"	, 	&CPU::LDY	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0xA5	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0xA6	] = {	"LDX"	, 	&CPU::LDX	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0xA8	] = {	"TAY"	, 	&CPU::TAY	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xA9	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0xAA	] = {	"TAX"	, 	&CPU::TAX	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xAC	] = {	"LDY"	, 	&CPU::LDY	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0xAD	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0xAE	] = {	"LDX"	, 	&CPU::LDX	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0xB0	] = {	"BCS"	, 	&CPU::BCS	, 	&CPU::REL	, 	2	};
        instr_mtx[	0xB1	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0xB4	] = {	"LDY"	, 	&CPU::LDY	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0xB5	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0xB6	] = {	"LDX"	, 	&CPU::LDX	, 	&CPU::ZPY	, 	4	};
        instr_mtx[	0xB8	] = {	"CLV"	, 	&CPU::CLV	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xB9	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0xBA	] = {	"TSX"	, 	&CPU::TSX	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xBC	] = {	"LDY"	, 	&CPU::LDY	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0xBD	] = {	"LDA"	, 	&CPU::LDA	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0xBE	] = {	"LDX"	, 	&CPU::LDX	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0xC0	] = {	"CPY"	, 	&CPU::CPY	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0xC1	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0xC4	] = {	"CPY"	, 	&CPU::CPY	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0xC5	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0xC6	] = {	"DEC"	, 	&CPU::DEC	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0xC8	] = {	"INY"	, 	&CPU::INY	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xC9	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0xCA	] = {	"DEX"	, 	&CPU::DEX	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xCC	] = {	"CPY"	, 	&CPU::CPY	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0xCD	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0xCE	] = {	"DEC"	, 	&CPU::DEC	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0xD0	] = {	"BNE"	, 	&CPU::BNE	, 	&CPU::REL	, 	2	};
        instr_mtx[	0xD1	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0xD5	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0xD6	] = {	"DEC"	, 	&CPU::DEC	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0xD8	] = {	"CLD"	, 	&CPU::CLD	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xD9	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0xDA	] = {	"NOP"	, 	&CPU::NOP	, 	&CPU::IMP	, 	2	}; //unofficial
        instr_mtx[	0xDD	] = {	"CMP"	, 	&CPU::CMP	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0xDE	] = {	"DEC"	, 	&CPU::DEC	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0xE0	] = {	"CPX"	, 	&CPU::CPX	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0xE1	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0xE4	] = {	"CPX"	, 	&CPU::CPX	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0xE5	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0xE6	] = {	"INC"	, 	&CPU::INC	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0xE8	] = {	"INX"	, 	&CPU::INX	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xE9	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0xEA	] = {	"NOP"	, 	&CPU::NOP	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xEC	] = {	"CPX"	, 	&CPU::CPX	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0xED	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0xEE	] = {	"INC"	, 	&CPU::INC	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0xF0	] = {	"BEQ"	, 	&CPU::BEQ	, 	&CPU::REL	, 	2	};
        instr_mtx[	0xF1	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0xF5	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0xF6	] = {	"INC"	, 	&CPU::INC	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0xF8	] = {	"SED"	, 	&CPU::SED	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0xF9	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0xFA	] = {	"NOP"	, 	&CPU::NOP	, 	&CPU::IMP	, 	2	}; //unofficial
        instr_mtx[	0xFD	] = {	"SBC"	, 	&CPU::SBC	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0xFE	] = {	"INC"	, 	&CPU::INC	, 	&CPU::ABX	, 	7	};
        
        for (int i = 0; i < 0x100; ++i){
            if (instr_mtx[i].name.empty()){
                instr_mtx[i].name = "XXX";
                instr_mtx[i].operate = &CPU::XXX;
                instr_mtx[i].addrMode = &CPU::IMP;
                instr_mtx[i].cycles = 2;
            }
        }
    }

    CPU::~CPU(){}

    void CPU::connect(MainBus *_bus){
        mainBus = _bus;
    }

    void CPU::exe_instr(){
        do {
            clock();
        } while (cycles != 0);
        return;
    }

    void CPU::clock(){
        if (cycles == 0){
            fetch(PC++, cur_opcode);
            set_flag(FLAG::U, true);//before executing any instr;

            INSTR &cur_instr = instr_mtx[cur_opcode];

            cycles = cur_instr.cycles;
            temp_byte = (this->*cur_instr.addrMode)();
            temp_byte &= (this->*cur_instr.operate)();

            //additional cycles;
            cycles += temp_byte;
            set_flag(FLAG::U, true);//after executing an instr;
        }
        cycles--;
        return;
    }

    void CPU::reset(){
        A = X = Y = 0;
        S = 0xFD;//low byte of stack pointer;
        P.val = 0 | FLAG::U;// <-- always set unused flag bit to 1;

        //fetch reset handler address;
        fetch(kRESET_VECTOR, fetch_buf);
        PC = fetch_buf;
        fetch(kRESET_VECTOR + 1, fetch_buf);
        PC |= (Word)fetch_buf << 8;

        addr_abs = addr_rel = fetch_buf = 0;
        //reset handler takes time;
        cycles = 8;
    }

    void CPU::irq(){
        if (P.I == 0){
            //push PC;
            push((PC >> 8) & 0xff);//push high byte first;
            push(PC & 0xff);

            //set STATUS_FLAGS;
            set_flag(FLAG::B, 0);//irq that is not caused by BRK;
            set_flag(FLAG::U, 1);
            set_flag(FLAG::I, 1);//shut down interrupt;

            //push STATUS_FLAGS;
            push(P.val);

            //fetch handler addr;
            fetch(kIRQ_VECTOR, fetch_buf);
            PC = fetch_buf;
            fetch(kIRQ_VECTOR + 1, fetch_buf);
            PC |= (Word)fetch_buf << 8;
            //interrupt handler takes time;
            cycles = 7;
        }
    }

    void CPU::nmi(){
        //push PC;
        push((PC >> 8) & 0xff);//push high byte first;
        push(PC & 0xff);

        //set STATUS_FLAGS;
        //FLAG::B represents a signal in the CPU controlling whether or not it was processing an interrupt when the flags were pushed.
        set_flag(FLAG::B, 0);
        set_flag(FLAG::U, 1);
        set_flag(FLAG::I, 1);//shut down irq;
        
        //push STATUS_FLAGS;
        push(P.val);

        //fetch nmi handler addr;
        fetch(kNMI_VECTOR, fetch_buf);
        PC = fetch_buf;
        fetch(kNMI_VECTOR + 1, fetch_buf);
        PC |= (Word)fetch_buf << 8;
        //nmi handler takes time;
        cycles = 8;
    }

    bool CPU::complete(){
        return cycles == 0;
    }

    void CPU::dma_cycles(Byte cc){
        cycles += cc;
    }

    std::map<Word, std::string> CPU::disassemble(Word addr_start, Word addr_stop){
        uint32_t addr = addr_start, STOP = addr_stop;
        std::map<Word, std::string> map_asm;
        Word line_addr = 0;
        Byte opcode = 0;
        Byte value = 0;
        Word temp = 0;

        auto hex = [](Word n, Byte r) {
            std::string s(r, '0');
            for (int i = r - 1; i >= 0; --i, n >>= 4)
                s[i] = "0123456789ABCDEF"[n & 0xF];
            return s;
        };

        while (addr <= STOP) {
            line_addr = addr;
            std::string sInstr = "$" + hex(line_addr, 4) + ": ";
            fetch(addr++, opcode);
            const INSTR& rInstr = instr_mtx[opcode];
            sInstr += rInstr.name + " ";

            if (rInstr.addrMode == &CPU::IMP)
                sInstr += " {IMP}";
            else if (rInstr.addrMode == &CPU::IMM) {
                fetch(addr++, value);
                sInstr += "#$" + hex(value, 2) + " {IMM}";
            }
            else if (rInstr.addrMode == &CPU::ZP0) {
                fetch(addr++, value);
                sInstr += "$" + hex(value, 2) + " {ZP0}";
            }
            else if (rInstr.addrMode == &CPU::ZPX) {
                fetch(addr++, value);
                sInstr += "$" + hex(value, 2) + ",X {ZPX}";
            }
            else if (rInstr.addrMode == &CPU::ZPY) {
                fetch(addr++, value);
                sInstr += "$" + hex(value, 2) + ",Y {ZPY}";
            }
            else if (rInstr.addrMode == &CPU::REL) {
                fetch(addr++, value);
                temp = value;
                if (value & 0x80) temp |= 0xff00;
                sInstr += "$" + hex(value, 2) + " [$" + hex(addr + temp, 4) + "]" + " {REL}";
            }
            else if (rInstr.addrMode == &CPU::ABS) {
                fetch(addr++, value);
                temp = value & 0xff;
                fetch(addr++, value);
                temp |= (Word)value << 8;
                sInstr += "$" + hex(temp, 4) + " {ABS}";
            }
            else if (rInstr.addrMode == &CPU::ABX) {
                fetch(addr++, value);
                temp = value & 0xff;
                fetch(addr++, value);
                temp |= (Word)value << 8;
                sInstr += "$" + hex(temp, 4) + ",X {ABX}";
            }
            else if (rInstr.addrMode == &CPU::ABY) {
                fetch(addr++, value);
                temp = value & 0xff;
                fetch(addr++, value);
                temp |= (Word)value << 8;
                sInstr += "$" + hex(temp, 4) + ",Y {ABY}";
            }
            else if (rInstr.addrMode == &CPU::IND) {
                fetch(addr++, value);
                temp = value & 0xff;
                fetch(addr++, value);
                temp |= (Word)value << 8;
                sInstr += "($" + hex(temp, 4) + ") {IND}";
            }
            else if (rInstr.addrMode == &CPU::IZX) {
                fetch(addr++, value);
                sInstr += "($" + hex(value, 2) + ",X) {IZX}";
            }
            else if (rInstr.addrMode == &CPU::IZY) {
                fetch(addr++, value);
                sInstr += "($" + hex(value, 2) + "),Y {IZY}";
            }

            map_asm[line_addr] = sInstr;
        }
        std::clog << "Disassemble : complete" << std::endl;
        return map_asm;
    }

    inline bool CPU::fetch(Word addr, Byte &data){
        return mainBus->read(addr, data);
    }
    inline bool CPU::store(Word addr, Byte data){
        return mainBus->write(addr, data);
    }

    inline void CPU::set_flag(FLAG flag, bool test){
        if (test) P.val |= flag;
        else P.val &= (~flag);
    }

    //==================================================================
    //addressing modes:
    //==================================================================

    Byte CPU::IMP(){
        //this addressing mode includes both Implicit mode 
        //and Accumulator mode;
        return 0;
    }

    Byte CPU::IMM(){
        addr_abs = PC++;
        return 0;
    }

    Byte CPU::ZP0(){
        fetch(PC++, fetch_buf);
        addr_abs = fetch_buf;
        addr_abs &= 0x00ff;
        return 0;
    }

    Byte CPU::ZPX(){
        fetch(PC++, fetch_buf);
        fetch_buf += X;//wrap around if crossed;
        addr_abs = fetch_buf & 0x00ff;
        return 0;
    }

    Byte CPU::ZPY(){
        fetch(PC++, fetch_buf);
        fetch_buf += Y;//wrap around if crossed;
        addr_abs = fetch_buf & 0x00ff;
        return 0;
    }

    Byte CPU::REL(){
        fetch(PC++, fetch_buf);
        addr_rel = fetch_buf;
        if (addr_rel & 0x80)//check the sign bit;
            addr_rel |= 0xff00;//manually convert to negative Word;
        return 0;
    }
    
    Byte CPU::ABS(){
        fetch(PC++, fetch_buf);
        addr_abs = fetch_buf;
        fetch(PC++, fetch_buf);
        addr_abs |= (Word)fetch_buf << 8;
        return 0;
    }
    
    Byte CPU::ABX(){
        //cycle 2: read low byte / add reg X;
        fetch(PC++, fetch_buf);  //fetch low byte;
        addr_abs = fetch_buf & 0x00ff;
        addr_abs += X;
        
        if ((addr_abs & 0xff00)) temp_byte = 1;//if page crossed;
        else temp_byte = 0;
        
        //cycle 3: read high byte / add low result;
        fetch(PC++, fetch_buf);  //fetch high byte;
        addr_abs |= (Word)fetch_buf << 8;

        return temp_byte;
    }

    Byte CPU::ABY(){
        //cycle 2: read low byte / add reg Y;
        fetch(PC++, fetch_buf);  //fetch low byte;
        addr_abs = fetch_buf & 0x00ff;
        addr_abs += Y;
        
        if ( (addr_abs & 0xff00) ) temp_byte = 1;//if page crossed;
        else temp_byte = 0;

        //cycle 3: read high byte / add low result;
        fetch(PC++, fetch_buf);  //fetch high byte;
        addr_abs |= (Word)fetch_buf << 8;

        return temp_byte;
    }
    
    Byte CPU::IND(){
        fetch(PC++, temp_byte);
        fetch(PC++, fetch_buf);
        temp_word = (Word)fetch_buf << 8;
        //there's an original cpu hardware bug:
        //the addition of lo + 1 will discard the carry,
        //which means indirect JMP instr will fail;
        fetch( (temp_word | temp_byte) , fetch_buf);//fetch low byte first;
        addr_abs = fetch_buf;
        temp_byte += 1;//if page crossed, it wraps around;
        fetch( (temp_word | temp_byte) , fetch_buf);
        addr_abs |= (Word)fetch_buf << 8;//auto promotion to Word;
        return 0;
    }
    
    Byte CPU::IZX(){
        //normally used in conjunction with a table of address held on zero page;
        //FORMULA: val = PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)	

        //cycle 1: read
        fetch(PC++, addr_zp0);

        //cycle 2: dummy read (always)
        addr_zp0 += X;//wrap around if crossed;

        //cycle 3: read
        fetch(addr_zp0, fetch_buf);//fetch low byte;
        addr_abs = fetch_buf & 0x00ff;

        //cycle 4: read
        addr_zp0 += 1;//still wrap around if crossed;
        fetch(addr_zp0, fetch_buf);//fetch high byte within zero page;
        addr_abs |= (Word)fetch_buf << 8;
        
        return 0;
    }
    
    Byte CPU::IZY(){
        //in instr contains the zero page location of the least signif byte of 16 bit addr;
        //FORMULA: val = PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)

        //cycle 1: read
        fetch(PC++, addr_zp0);
        
        //cycle 2: read
        fetch(addr_zp0, fetch_buf); //fetch low byte;
        addr_abs = fetch_buf & 0xff;
        
        //cycle 3: dummy read, depends on:
        //  for LOAD  instr: if page crossed
        //  for STORE instr: always
        addr_abs += Y;
        if (addr_abs & 0xff00) temp_byte = 1;//page cross check;
        else temp_byte = 0;

        //cycle 3: read
        addr_zp0 += 1;//wrap around if crossed;
        fetch(addr_zp0, fetch_buf);
        addr_abs += (Word)fetch_buf << 8;//use operator+= : above "addr_abs += Y" may have carry out onto bit 8;
        
        return temp_byte;
    }

    //==================================================================
    //opcodes:
    //==================================================================
    Byte CPU::ADC() {
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        adc();
        return 1;//add one cycle if page cross;
    }

    //Byte CPU::ADC(){
    //    //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
    //    fetch(addr_abs, fetch_buf);
    //    temp_word = A;
    //    temp_word += fetch_buf;
    //    temp_word += P.C;
    //    //test carry;
    //    set_flag(FLAG::C, temp_word & 0x100);
    //    //test zero;
    //    set_flag(FLAG::Z, (temp_word & 0xff) == 0);
    //    //test overflow;
    //    bool sign_acc = A & 0x80;
    //    bool sign_fch = fetch_buf & 0x80;
    //    bool sign_res = temp_word & 0x80;
    //    set_flag(FLAG::V, (sign_acc == sign_fch) && (sign_acc ^ sign_res) );
    //    //test negative;
    //    set_flag(FLAG::N, sign_res);
    //    A = temp_word & 0xff;
    //    return 1;//add one cycle if page cross;
    //}
    
    Byte CPU::AND(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A = A & fetch_buf;
        //set flags:
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::ASL(){
        //ACCUM, ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        set_flag(FLAG::C, fetch_buf & 0x80);
        fetch_buf <<= 1;
        set_flag(FLAG::Z, fetch_buf == 0);
        set_flag(FLAG::N, fetch_buf & 0x80);
        store(addr_abs, fetch_buf);
        return 0;
    }
    Byte CPU::ASL_A() {
        //only used by ACCUM mode;
        set_flag(FLAG::C, A & 0x80);
        A <<= 1;
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 0;
    }

    inline void CPU::branch(bool test) {
        if (test) {
            addr_abs = PC + addr_rel;
            ++cycles;      //if branch suceeds;
            if ((addr_abs & 0xff00) != (PC & 0xff00))
                ++cycles;  //if to a new page;
            PC = addr_abs;
        }
    }

    Byte CPU::BCC(){
        //REL;
        //if the carry flag is clear;
        branch(P.C == 0);
        return 0;
    }
    Byte CPU::BCS(){
        //REL;
        //if the carry flag is set;
        branch(P.C);
        return 0;
    }

    Byte CPU::BEQ(){
        //REL;
        //if the zero flag is set;
        branch(P.Z);
        return 0;
    }
    Byte CPU::BIT(){
        //ZP0, ABS;
        fetch(addr_abs, fetch_buf);
        set_flag(FLAG::V, fetch_buf & 0x40);//M6;
        set_flag(FLAG::N, fetch_buf & 0x80);//M7;
        set_flag(FLAG::Z, (A & fetch_buf) == 0);//result == 0;
        return 0;
    }
    Byte CPU::BMI(){
        //REL;
        //if the negative flag is set;
        branch(P.N);
        return 0;
    }
    Byte CPU::BNE(){
        //REL;
        //if the zero flag is clear;
        branch(P.Z == 0);
        return 0;
    }
    Byte CPU::BPL(){
        //REL;
        //if the negative flag is clear;
        branch(P.N == 0);
        return 0;
    }

    Byte CPU::BRK(){
        //IMP       
        //push the next instr addr;
        ++PC;
        push((PC >> 8) & 0xff);//push high byte first;
        push(PC & 0xff);

        //push status flags;
        //  BRK should push status with bits 4 and 5 set
        set_flag(FLAG::U, true);
        push((P.val | FLAG::B));//FLAG::B is a transient state;

        //shut down interrupt;
        set_flag(FLAG::I, true);//Note the order when FLAG::I should be set, ie, after pushing status flags;
        
        //goto irq handler;
        fetch(kIRQ_VECTOR, fetch_buf);
        PC = fetch_buf;
        fetch(kIRQ_VECTOR + 1, fetch_buf);
        PC |= (Word)fetch_buf << 8;
        return 0;
    }
    Byte CPU::BVC(){
        //REL;
        //if the overflow flag is clear;
        branch(P.V == 0);
        return 0;        
    }
    Byte CPU::BVS(){
        //REL;
        //if the overflow flag is set;
        branch(P.V);
        return 0;
    }
    Byte CPU::CLC(){
        //IMP
        set_flag(FLAG::C, false);
        return 0;
    }
    Byte CPU::CLD(){
        //IMP
        set_flag(FLAG::D, false);
        return 0;
    }

    Byte CPU::CLI(){
        //IMP
        set_flag(FLAG::I, false);
        return 0;
    }
    Byte CPU::CLV(){
        //IMP
        set_flag(FLAG::V, false);
        return 0;
    }

    //helper funtion for CMP(), CPX(), CPY();
    inline void CPU::cmp(Byte reg) {
        temp_word = (Word)reg - (Word)fetch_buf;
        set_flag(FLAG::C, reg >= fetch_buf);
        set_flag(FLAG::Z, (temp_word & 0x00ff) == 0x0000);
        set_flag(FLAG::N, temp_word & 0x0080);
    }

    Byte CPU::CMP(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        cmp(A);
        return 1; // <-- why?
    }
    Byte CPU::CPX(){
        //IMM, ZP0, ABS;
        fetch(addr_abs, fetch_buf);
        cmp(X);
        return 0;
    }
    Byte CPU::CPY(){
        //IMM, ZP0, ABS;
        fetch(addr_abs, fetch_buf);
        cmp(Y);
        return 0;
    }

    Byte CPU::DEC(){
        //ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        temp_byte = fetch_buf - 1;
        set_flag(FLAG::Z, temp_byte == 0);
        set_flag(FLAG::N, temp_byte & 0x80);
        store(addr_abs, temp_byte);//write back;
        return 0;
    }
    Byte CPU::DEX(){
        //IMP
        --X;
        set_flag(FLAG::Z, X == 0);
        set_flag(FLAG::N, X & 0x80);
        return 0;
    }
    Byte CPU::DEY(){
        //IMP
        --Y;
        set_flag(FLAG::Z, Y == 0);
        set_flag(FLAG::N, Y & 0x80);
        return 0;
    }
    Byte CPU::EOR(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A = A ^ fetch_buf;
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::INC(){
        //ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        temp_byte = fetch_buf + 1;          // <--  wrap around ???
        set_flag(FLAG::Z, temp_byte == 0);
        set_flag(FLAG::N, temp_byte & 0x80);
        store(addr_abs, temp_byte);
        return 0;
    }

    Byte CPU::INX(){
        //IMP
        ++X;
        set_flag(FLAG::Z, X == 0);
        set_flag(FLAG::N, X & 0x80);
        return 0;
    }
    Byte CPU::INY(){
        //IMP
        ++Y;
        set_flag(FLAG::Z, Y == 0);
        set_flag(FLAG::N, Y & 0x80);
        return 0;
    }
    Byte CPU::JMP(){
        //ABS, IND;
        PC = addr_abs;
        return 0;
    }

    //helper function for stack operation;
    inline void CPU::push(Byte val) {
        store(kSTACK_BASE + S, val);
        --S;
    }
    inline void CPU::pull(Byte &val) {
        ++S;
        fetch(kSTACK_BASE + S, val);
    }

    Byte CPU::JSR(){
        //ABS;
        --PC;//push PC - 1;
        push((PC >> 8) & 0xff);//push high byte first;
        push(PC & 0xff);
        PC = addr_abs;
        return 0;
    }
    Byte CPU::LDA(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A = fetch_buf;
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 1;//add one cycle if page cross;
    }

    Byte CPU::LDX(){
        //IMM, ZP0, ZPY, ABS, ABY;
        fetch(addr_abs, fetch_buf);
        X = fetch_buf;
        set_flag(FLAG::Z, X == 0);
        set_flag(FLAG::N, X & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::LDY(){
        //IMM, ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        Y = fetch_buf;
        set_flag(FLAG::Z, Y == 0);
        set_flag(FLAG::N, Y & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::LSR(){
        //ACCUM, ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        set_flag(FLAG::C, fetch_buf & 0x1);
        fetch_buf >>= 1;
        set_flag(FLAG::Z, fetch_buf == 0);
        set_flag(FLAG::N, fetch_buf & 0x80);
        store(addr_abs, fetch_buf);
        return 0;
    }
    Byte CPU::LSR_A() {
        set_flag(FLAG::C, A & 0x1);
        A >>= 1;
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 0;
    }
    Byte CPU::NOP(){
        //IMP
        //unofficial NOP with different addressing mode;
        switch (cur_opcode) {
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            return 1;
            break;
        }
        return 0;
    }
    Byte CPU::ORA(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A = A | fetch_buf;
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 1;//  <-- why?
    }

    Byte CPU::PHA(){
        //IMP
        push(A);
        return 0;
    }
    Byte CPU::PHP(){
        //IMP
        //"FLAG::B represents a signal in the CPU controlling whether or not it was processing an interrupt when the flags were pushed."
        push((P.val | FLAG::B | FLAG::U));
        return 0;
    }
    Byte CPU::PLA(){
        //IMP
        pull(A);
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 0;
    }
    Byte CPU::PLP(){
        //IMP
        pull(P.val);
        //"The CPU pushes a value with B clear during an interrupt, 
        //  pushes a value with B set in response to PHP or BRK, 
        //  and disregards bits 5 and 4 when reading flags from the stack in the PLP or RTI instruction."
        set_flag(FLAG::B, false);
        set_flag(FLAG::U, false);
        return 0;
    }
    Byte CPU::ROL(){
        //ACCUM, ZP0, ZPX, ABS, ABX;
        //"Bit 0 is filled with the current value of the carry flag 
        //whilst the old bit 7 becomes the new carry flag value."
        fetch(addr_abs, fetch_buf);
        temp_byte = fetch_buf;
        fetch_buf <<= 1;
        fetch_buf |= P.C;
        set_flag(FLAG::C, temp_byte & 0x80);
        set_flag(FLAG::Z, fetch_buf == 0);
        set_flag(FLAG::N, fetch_buf & 0x80);
        store(addr_abs, fetch_buf);
        return 0;
    }
    Byte CPU::ROL_A() {
        //This func is only used by ACCUM mode;
        temp_byte = A;
        A <<= 1;
        A |= P.C;
        set_flag(FLAG::C, temp_byte & 0x80);
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 0;
    }

    Byte CPU::ROR(){
        //ACCUM, ZP0, ZPX, ABS, ABX;
        //Bit 7 is filled with the current value of the carry flag 
        //whilst the old bit 0 becomes the new carry flag value.
        fetch(addr_abs, fetch_buf);
        temp_byte = fetch_buf & 0x1;
        fetch_buf >>= 1;
        fetch_buf |= (P.C << 7);
        set_flag(FLAG::N, P.C);
        set_flag(FLAG::C, temp_byte);
        set_flag(FLAG::Z, fetch_buf == 0);
        store(addr_abs, fetch_buf);
        return 0;
    }
    Byte CPU::ROR_A() {
        //This func is only used by ACCUM mode;
        temp_byte = A & 0x1;
        A >>= 1;
        A |= (P.C << 7);
        set_flag(FLAG::N, P.C);//carry becomes bit 7, which decide nagetive flag;
        set_flag(FLAG::C, temp_byte);//update carry flag;
        set_flag(FLAG::Z, A == 0);
        return 0;
    }
    Byte CPU::RTI(){
        //IMP
        //pull status flags;
        pull(P.val);
        //"The CPU pushes a value with B clear during an interrupt, 
        //  pushes a value with B set in response to PHP or BRK, 
        //  and disregards bits 5 and 4 when reading flags from the stack in the PLP or RTI instruction."
        set_flag(FLAG::B, false);
        set_flag(FLAG::U, false);
        //pull PC;
        pull(temp_byte);//pull low byte first;
        PC = temp_byte;
        pull(temp_byte);
        PC |= (Word)temp_byte << 8;
        //RTI() return to addr + 0;
        return 0;
    }
    Byte CPU::RTS(){
        //IMP
        pull(temp_byte);//pull low byte first;
        PC = temp_byte;
        pull(temp_byte);
        PC |= (Word)temp_byte << 8;
        ++PC;//RTS() return to addr + 1;
        return 0;
    }

    //helper function for ADC() and SBC();
    inline void CPU::adc() {
        temp_word = (Word)A + (Word)fetch_buf + (Word)P.C;
        //test carry;
        set_flag(FLAG::C, temp_word & 0x100);
        //test zero;
        set_flag(FLAG::Z, (temp_word & 0xff) == 0);

        //test overflow;
        //bool sign_acc = A & 0x80;
        //bool sign_fch = fetch_buf & 0x80;
        //bool sign_res = temp_word & 0x80;
        //set_flag(FLAG::V, (sign_acc == sign_fch) && (sign_acc ^ sign_res));

        // (sign_acc NXOR sign_mem) AND (sign_acc XOR sign_result)
        set_flag(FLAG::V, (~((Word)A ^ (Word)fetch_buf) & ((Word)A ^ temp_word)) & 0x0080 );

        //test negative;
        set_flag(FLAG::N, temp_word & 0x80);
        A = temp_word & 0xff;
    }

    Byte CPU::SBC(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        fetch_buf ^= 0xff;//flip each bit;
        adc();
        return 1;//add one cycle if page cross;

        //temp_word = A - (!P.C);//carry not = borrow;
        //temp_word -= fetch_buf;
        //set_flag(FLAG::C, temp_word > A);
        ////test zero;
        //set_flag(FLAG::Z, (temp_word & 0xff) == 0);
        ////test overflow;
        //bool sign_acc = A & 0x80;
        //bool sign_fch = fetch_buf & 0x80;
        //bool sign_res = temp_word & 0x80;
        //set_flag(FLAG::V, (sign_acc ^ sign_fch) && (sign_acc ^ sign_res) );
        ////test negative;
        //set_flag(FLAG::N, sign_res);
        //A = temp_word & 0xff;
    }

    Byte CPU::SEC(){
        //IMP
        set_flag(FLAG::C, true);
        return 0;
    }
    Byte CPU::SED(){
        //IMP
        set_flag(FLAG::D, true);
        return 0;
    }

    Byte CPU::SEI(){
        //IMP
        set_flag(FLAG::I, true);
        return 0;
    }
    Byte CPU::STA(){
        //ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        store(addr_abs, A);
        return 0;
    }
    Byte CPU::STX(){
        //ZP0, ZPY, ABS;
        store(addr_abs, X);
        return 0;
    }
    Byte CPU::STY(){
        //ZP0, ZPX, ABS;
        store(addr_abs, Y);
        return 0;
    }
    Byte CPU::TAX(){
        //IMP
        X = A;
        set_flag(FLAG::Z, X == 0);
        set_flag(FLAG::N, X & 0x80);
        return 0;
    }

    Byte CPU::TAY(){
        //IMP
        Y = A;
        set_flag(FLAG::Z, Y == 0);
        set_flag(FLAG::N, Y & 0x80);
        return 0;
    }
    Byte CPU::TSX(){
        //IMP
        X = S;
        set_flag(FLAG::Z, X == 0);
        set_flag(FLAG::N, X & 0x80);
        return 0;
    }
    Byte CPU::TXA(){
        //IMP
        A = X;
        set_flag(FLAG::Z, A == 0);
        set_flag(FLAG::N, A & 0x80);
        return 0;
    }
    Byte CPU::TXS(){
        //IMP
        S = X;
        //TXS() do not affect any flags;
        return 0;
    }
    Byte CPU::TYA(){
        //IMP
        A = Y;
        set_flag(FLAG::Z, Y == 0);
        set_flag(FLAG::N, Y & 0x80);
        return 0;
    }
    
    Byte CPU::XXX(){
        //IMP
        return 0;
    }

};