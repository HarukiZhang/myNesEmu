#include "CPU.h"

#include "PPU.h"
#include "MainBus.h"


namespace nes {

    CPU::CPU(){
        instr_mtx[	0x00	] = {	"BRK"	, 	&CPU::BRK	, 	&CPU::IMP	, 	7	};
        instr_mtx[	0x01	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x05	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x06	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x08	] = {	"PHP"	, 	&CPU::PHP	, 	&CPU::IMP	, 	3	};
        instr_mtx[	0x09	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x0A	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x0D	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x0E	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x10	] = {	"BPL"	, 	&CPU::BPL	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x11	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x15	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x16	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x18	] = {	"CLC"	, 	&CPU::CLC	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x19	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0x1D	] = {	"ORA"	, 	&CPU::ORA	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0x1E	] = {	"ASL"	, 	&CPU::ASL	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0x20	] = {	"JSR"	, 	&CPU::JSR	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x21	] = {	"AND"	, 	&CPU::AND	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x24	] = {	"BIT"	, 	&CPU::BIT	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x25	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x26	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x28	] = {	"PLP"	, 	&CPU::PLP	, 	&CPU::IMP	, 	4	};
        instr_mtx[	0x29	] = {	"AND"	, 	&CPU::AND	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x2A	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x2C	] = {	"BIT"	, 	&CPU::BIT	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x2D	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x2E	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x30	] = {	"BMI"	, 	&CPU::BMI	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x31	] = {	"AND"	, 	&CPU::AND	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x35	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x36	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x38	] = {	"SEC"	, 	&CPU::SEC	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x39	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0x3D	] = {	"AND"	, 	&CPU::AND	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0x3E	] = {	"ROL"	, 	&CPU::ROL	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0x40	] = {	"RTI"	, 	&CPU::RTI	, 	&CPU::IMP	, 	6	};
        instr_mtx[	0x41	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x45	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x46	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x48	] = {	"PHA"	, 	&CPU::PHA	, 	&CPU::IMP	, 	3	};
        instr_mtx[	0x49	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x4A	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x4C	] = {	"JMP"	, 	&CPU::JMP	, 	&CPU::ABS	, 	3	};
        instr_mtx[	0x4D	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x4E	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x50	] = {	"BVC"	, 	&CPU::BVC	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x51	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x55	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x56	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x58	] = {	"CLI"	, 	&CPU::CLI	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x59	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ABY	, 	4	};
        instr_mtx[	0x5D	] = {	"EOR"	, 	&CPU::EOR	, 	&CPU::ABX	, 	4	};
        instr_mtx[	0x5E	] = {	"LSR"	, 	&CPU::LSR	, 	&CPU::ABX	, 	7	};
        instr_mtx[	0x60	] = {	"RTS"	, 	&CPU::RTS	, 	&CPU::IMP	, 	6	};
        instr_mtx[	0x61	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::IZX	, 	6	};
        instr_mtx[	0x65	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ZP0	, 	3	};
        instr_mtx[	0x66	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::ZP0	, 	5	};
        instr_mtx[	0x68	] = {	"PLA"	, 	&CPU::PLA	, 	&CPU::IMP	, 	4	};
        instr_mtx[	0x69	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::IMM	, 	2	};
        instr_mtx[	0x6A	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x6C	] = {	"JMP"	, 	&CPU::JMP	, 	&CPU::IND	, 	5	};
        instr_mtx[	0x6D	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ABS	, 	4	};
        instr_mtx[	0x6E	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::ABS	, 	6	};
        instr_mtx[	0x70	] = {	"BVS"	, 	&CPU::BVS	, 	&CPU::REL	, 	2	};
        instr_mtx[	0x71	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::IZY	, 	5	};
        instr_mtx[	0x75	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ZPX	, 	4	};
        instr_mtx[	0x76	] = {	"ROR"	, 	&CPU::ROR	, 	&CPU::ZPX	, 	6	};
        instr_mtx[	0x78	] = {	"SEI"	, 	&CPU::SEI	, 	&CPU::IMP	, 	2	};
        instr_mtx[	0x79	] = {	"ADC"	, 	&CPU::ADC	, 	&CPU::ABY	, 	4	};
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

    void CPU::clock(){
        if (cycles == 0){
            fetch(PC++, cur_opcode);

            INSTR &cur_instr = instr_mtx[cur_opcode];

            cycles = cur_instr.cycles;
            Byte addi_cc1 = (this->*cur_instr.addrMode)();//why use this->*
            Byte addi_cc2 = (this->*cur_instr.operate)();

            //additional cycles;
            cycles += (addi_cc1 & addi_cc2);
            //what will determine whether there's an additional cycles?
        }
        cycles--;
        return;
    }

    inline bool CPU::fetch(Word addr, Byte &data){
        return mainBus->read(addr, data);
    }
    inline bool CPU::store(Word addr, Byte data){
        return mainBus->write(addr, data);
    }

    inline void CPU::setFlag(FLAG flag, bool test){
        if (test) P.val |= flag;
        else P.val &= (~flag);
    }

    //addressing modes:

    Byte CPU::IMP(){
        //implied mode usally operate upon accumulator;
        fetch_buf = A;
        return 0;
    }

    Byte CPU::IMM(){
        addr_abs = PC++;
        return 0;
    }

    Byte CPU::ZP0(){
        fetch(PC++, fetch_buf);
        addr_abs = fetch_buf;
        return 0;
    }

    Byte CPU::ZPX(){
        fetch(PC++, fetch_buf);
        addr_abs = (fetch_buf + X) & 0xff;//discard carry;
        return 0;
    }

    Byte CPU::ZPY(){
        fetch(PC++, fetch_buf);
        addr_abs = (fetch_buf + Y) & 0xff;//discard carry;
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
        Word tmp = fetch_buf;
        tmp <<= 8;
        addr_abs |= tmp;
        return 0;
    }
    
    Byte CPU::ABX(){
        fetch(PC++, fetch_buf);
        addr_abs = fetch_buf;
        fetch(PC++, fetch_buf);
        Word tmp = fetch_buf;
        tmp <<= 8;
        addr_abs |= tmp;
        addr_abs += X;
        if ( (addr_abs & 0xff00) != tmp )
            return 1;
        else return 0;
    }

    Byte CPU::ABY(){
        fetch(PC++, fetch_buf);
        addr_abs = fetch_buf;
        fetch(PC++, fetch_buf);
        Word tmp = fetch_buf;
        tmp <<= 8;
        addr_abs |= tmp;
        addr_abs += Y;
        if ( (addr_abs & 0xff00) != tmp )
            return 1;
        else return 0;
    }
    
    Byte CPU::IND(){
        fetch(PC++, fetch_buf);
        Byte lo = fetch_buf;
        fetch(PC++, fetch_buf);
        Word hi = fetch_buf;
        hi <<= 8;
        //there's an original cpu bug:
        //the addition of lo + 1 will discard the carry,
        //which means indirect JMP instr will fail;
        fetch( (hi | lo) , fetch_buf);//fetch low byte first;
        addr_abs = fetch_buf;
        lo += 1;//witch page crossing wrap around;
        fetch( (hi | lo) , fetch_buf);//fetch high byte;
        addr_abs |= fetch_buf << 8;//auto promotion to Word;
        return 0;
    }
    
    Byte CPU::IZX(){
        fetch(PC++, fetch_buf);
        Byte tmp = fetch_buf + X;//with zero page wrap around;
        fetch(tmp + 1, fetch_buf);//fetch high byte first;
        addr_abs = fetch_buf;
        addr_abs <<= 8;
        fetch(tmp, fetch_buf);
        addr_abs |= fetch_buf;
        return 0;
    }
    
    Byte CPU::IZY(){
        fetch(PC++, fetch_buf);
        addr_abs = fetch_buf;//use addr_abs as tmp;
        fetch(addr_abs + 1, fetch_buf);//fetch high byte first;
        Word base = fetch_buf;
        base <<= 8;
        fetch(addr_abs, fetch_buf);
        base |= fetch_buf;
        addr_abs = base + Y;
        if ( (addr_abs & 0xff00) != (base & 0xff00) )
            return 1;
        else return 0;
    }

    //opcodes:

    Byte CPU::ADC(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        Word res = A;
        res += fetch_buf;
        res += P.C;
        //test carry;
        setFlag(FLAG::C, res & 0x100);
        //test zero;
        setFlag(FLAG::Z, (res & 0xff) == 0);
        //test overflow;
        bool sign_acc = A & 0x80;
        bool sign_fch = fetch_buf & 0x80;
        bool sign_res = res & 0x80;
        setFlag(FLAG::V, (sign_acc == sign_fch) && (sign_acc ^ sign_res) );
        //test negative;
        setFlag(FLAG::N, sign_res);
        A = res & 0xff;
        return 1;//add one cycle if page cross;
    }
    Byte CPU::AND(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A &= fetch_buf;
        //set flags:
        setFlag(FLAG::Z, A == 0);
        setFlag(FLAG::N, A & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::ASL(){
        //ABS, ZP0, ACCUM, ZPX, ABX;
    }
    Byte CPU::BCC(){}
    Byte CPU::BCS(){
        //REL;
        if (P.C){
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;//if branch occurs to diff page, add 2 cycles;
            PC = addr_abs;
        }
        return 0;
    }

    Byte CPU::BEQ(){}
    Byte CPU::BIT(){}
    Byte CPU::BMI(){}
    Byte CPU::BNE(){}
    Byte CPU::BPL(){}

    Byte CPU::BRK(){}//IMP
    Byte CPU::BVC(){}
    Byte CPU::BVS(){}
    Byte CPU::CLC(){
        //IMP
        setFlag(FLAG::C, false);
        return 0;
    }
    Byte CPU::CLD(){}//IMP

    Byte CPU::CLI(){}//IMP
    Byte CPU::CLV(){}//IMP
    Byte CPU::CMP(){}
    Byte CPU::CPX(){}
    Byte CPU::CPY(){}

    Byte CPU::DEC(){}
    Byte CPU::DEX(){}//IMP
    Byte CPU::DEY(){}//IMP
    Byte CPU::EOR(){
        //add one cycle if page cross;
    }
    Byte CPU::INC(){}

    Byte CPU::INX(){}//IMP
    Byte CPU::INY(){}//IMP
    Byte CPU::JMP(){}
    Byte CPU::JSR(){}
    Byte CPU::LDA(){
        if (this->fetch(addr_abs + addr_rel, fetch_buf))
            A = fetch_buf;
        //add one cycle if page cross;
    }

    Byte CPU::LDX(){
        //add one cycle if page cross;
    }
    Byte CPU::LDY(){
        //add one cycle if page cross;
    }
    Byte CPU::LSR(){}//ABS、ZP0、ACCUM、ZPX、ABX；
    Byte CPU::NOP(){}//IMP
    Byte CPU::ORA(){}

    Byte CPU::PHA(){}//IMP
    Byte CPU::PHP(){}//IMP
    Byte CPU::PLA(){}//IMP
    Byte CPU::PLP(){}//IMP
    Byte CPU::ROL(){}//ABS、ZP0、ACCUM、ZPX、ABX；

    Byte CPU::ROR(){}//ABS、ZP0、ACCUM、ZPX、ABX；
    Byte CPU::RTI(){}//IMP
    Byte CPU::RTS(){}//IMP
    Byte CPU::SBC(){
        //add one cycle if page cross;
    }
    Byte CPU::SEC(){}//IMP
    Byte CPU::SED(){}//IMP

    Byte CPU::SEI(){}//IMP
    Byte CPU::STA(){}
    Byte CPU::STX(){}
    Byte CPU::STY(){}
    Byte CPU::TAX(){}//IMP

    Byte CPU::TAY(){}//IMP
    Byte CPU::TSX(){}//IMP
    Byte CPU::TXA(){}//IMP
    Byte CPU::TXS(){}//IMP
    Byte CPU::TYA(){}//IMP
    
    Byte CPU::XXX(){}//IMP

};