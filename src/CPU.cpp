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

    void CPU::exe_instr(){
        do {
            clock();
        } while (cycles != 0);
        return;
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

    void CPU::reset(){
        A = X = Y = 0;
        S = 0xfd;//low byte of stack pointer;
        P.val = 0x00 | FLAG::U;//why OR FLAG::U ?

        addr_abs = 0xfffc;//reset handle vector addr;
        fetch(addr_abs, fetch_buf);
        PC = fetch_buf;
        fetch(addr_abs + 1, fetch_buf);
        PC |= fetch_buf << 8;

        addr_abs = addr_rel = fetch_buf = 0;
        //reset handler takes time;
        cycles = 8;
    }

    void CPU::irq(){
        if (P.I == 0){
            //push PC;
            store(kSTACK_BASE + S, (PC >> 8) & 0xff );
            --S;    //push high byte first;
            store(kSTACK_BASE + S, PC & 0xff);
            --S;
            //set STATUS_FLAGS;
            setFlag(FLAG::B, 0);//irq that is not caused by BRK;
            setFlag(FLAG::U, 1);
            setFlag(FLAG::I, 1);//shut down interrupt;
            //push STATUS_FLAGS;
            store(kSTACK_BASE + S, P.val);
            --S;
            //fetch handler addr;
            addr_abs = 0xfffe;
            fetch(addr_abs, fetch_buf);
            PC = fetch_buf;
            fetch(addr_abs + 1, fetch_buf);
            PC |= fetch_buf << 8;
            //interrupt handler takes time;
            cycles = 7;
        }
    }

    void CPU::nmi(){
        //push PC;
        store(kSTACK_BASE + S, (PC >> 8) & 0xff );
        --S;
        store(kSTACK_BASE + S, PC & 0xff);
        --S;
        //set STATUS_FLAGS;
        setFlag(FLAG::B, 0);
        setFlag(FLAG::U, 1);
        setFlag(FLAG::I, 1);//shut down irq;
        //push STATUS_FLAGS;
        store(kSTACK_BASE + S, P.val);
        --S;
        //fetch handler addr;
        addr_abs = 0xfffa;
        fetch(addr_abs, fetch_buf);
        PC = fetch_buf;
        fetch(addr_abs + 1, fetch_buf);
        PC |= fetch_buf << 8;
        //nmi handler takes time;
        cycles = 8;
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
        //there's an original cpu hardware bug:
        //the addition of lo + 1 will discard the carry,
        //which means indirect JMP instr will fail;
        fetch( (hi | lo) , fetch_buf);//fetch low byte first;
        addr_abs = fetch_buf;
        lo += 1;//with page crossing wrap around;
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
        //ACCUM, ZP0, ZPX, ABS, ABX;
        if (cur_opcode == 0x0A){//opcode of ASL-ACCUM;
            setFlag(FLAG::C, A & 0x80);
            A <<= 1;
            setFlag(FLAG::Z, A == 0);
            setFlag(FLAG::N, A & 0x80);
        }
        else {
            fetch(addr_abs, fetch_buf);
            setFlag(FLAG::C, fetch_buf & 0x80);
            fetch_buf <<= 1;
            setFlag(FLAG::Z, fetch_buf == 0);
            setFlag(FLAG::N, fetch_buf & 0x80);
            store(addr_abs, fetch_buf);
        }
        return 0;
    }
    Byte CPU::BCC(){
        //REL;
        if (P.C == 0){//if the carry flag is clear;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;
            PC = addr_abs;
        }
    }
    Byte CPU::BCS(){
        //REL;
        if (P.C){//if the carry flag is set;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;//if branch occurs to diff page, add 2 cycles;
            PC = addr_abs;
        }
        return 0;
    }

    Byte CPU::BEQ(){
        //REL;
        if (P.Z){//if the zero flag is set;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;
            PC = addr_abs;
        }
        return 0;
    }
    Byte CPU::BIT(){
        //ZP0, ABS;
        fetch(addr_abs, fetch_buf);
        setFlag(FLAG::V, fetch_buf & 0x40);//M6;
        setFlag(FLAG::N, fetch_buf & 0x80);//M7;
        setFlag(FLAG::Z, (A & fetch_buf) == 0);//result;
        return 0;
    }
    Byte CPU::BMI(){
        //REL;
        if (P.N){//if the negative flag is set;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;
            PC = addr_abs;
        }
        return 0;
    }
    Byte CPU::BNE(){
        //REL;
        if (P.Z == 0){//if the zero flag is clear;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;
            PC = addr_abs;
        }
        return 0;
    }
    Byte CPU::BPL(){
        //REL;
        if (P.N == 0){//if the negative flag is clear;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;
            PC = addr_abs;
        }
        return 0;
    }

    Byte CPU::BRK(){
        //IMP
        setFlag(FLAG::I, true);//shut down interrupt;
        //push the next instr addr;
        ++PC;
        store(kSTACK_BASE + S, (PC >> 8) & 0xff);
        --S;//push high byte first;
        store(kSTACK_BASE + S, PC & 0xff);
        --S;
        //push status flags;
        setFlag(FLAG::B, true);
        store(kSTACK_BASE + S, P.val);
        --S;
        setFlag(FLAG::B, false);
        //goto irq handler;
        fetch(0xfffe, fetch_buf);
        PC = fetch_buf;
        fetch(0xffff, fetch_buf);
        PC |= fetch_buf << 8;
        return 0;
    }
    Byte CPU::BVC(){
        //REL;
        if (P.V == 0){//if the overflow flag is clear;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;
            PC = addr_abs;
        }
        return 0;        
    }
    Byte CPU::BVS(){
        //REL;
        if (P.V){//if the overflow flag is set;
            addr_abs = PC + addr_rel;
            ++cycles;
            if ( (addr_abs & 0xff00) != (PC & 0xff00) )
                ++cycles;
            PC = addr_abs;
        }
        return 0;
    }
    Byte CPU::CLC(){
        //IMP
        setFlag(FLAG::C, false);
        return 0;
    }
    Byte CPU::CLD(){
        //IMP
        setFlag(FLAG::D, false);
        return 0;
    }

    Byte CPU::CLI(){
        //IMP
        setFlag(FLAG::I, false);
        return 0;
    }
    Byte CPU::CLV(){
        //IMP
        setFlag(FLAG::V, false);
        return 0;
    }
    Byte CPU::CMP(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        bool pos = A >= fetch_buf;
        setFlag(FLAG::C, pos);
        setFlag(FLAG::Z, A == fetch_buf);
        setFlag(FLAG::N, !pos);
        return 0;
    }
    Byte CPU::CPX(){
        //IMM, ZP0, ABS;
        fetch(addr_abs, fetch_buf);
        bool pos = X >= fetch_buf;
        setFlag(FLAG::C, pos);
        setFlag(FLAG::Z, X == fetch_buf);
        setFlag(FLAG::N, !pos);
        return 0;
    }
    Byte CPU::CPY(){
        //IMM, ZP0, ABS;
        fetch(addr_abs, fetch_buf);
        bool pos = Y >= fetch_buf;
        setFlag(FLAG::C, pos);
        setFlag(FLAG::Z, Y == fetch_buf);
        setFlag(FLAG::N, !pos);
        return 0;
    }

    Byte CPU::DEC(){
        //ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        Byte res = fetch_buf - 1;
        setFlag(FLAG::Z, res == 0);
        setFlag(FLAG::N, res & 0x80);
        store(addr_abs, res);//write back;
        return 0;
    }
    Byte CPU::DEX(){
        //IMP
        --X;
        setFlag(FLAG::Z, X == 0);
        setFlag(FLAG::N, X & 0x80);
        return 0;
    }
    Byte CPU::DEY(){
        //IMP
        --Y;
        setFlag(FLAG::Z, Y == 0);
        setFlag(FLAG::N, Y & 0x80);
        return 0;
    }
    Byte CPU::EOR(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A ^= fetch_buf;
        setFlag(FLAG::Z, A == 0);
        setFlag(FLAG::N, A & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::INC(){
        //ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        Byte res = fetch_buf + 1;
        setFlag(FLAG::Z, res == 0);
        setFlag(FLAG::N, res & 0x80);
        store(addr_abs, res);
        return 0;
    }

    Byte CPU::INX(){
        //IMP
        ++X;
        setFlag(FLAG::Z, X == 0);
        setFlag(FLAG::N, X & 0x80);
        return 0;
    }
    Byte CPU::INY(){
        //IMP
        ++Y;
        setFlag(FLAG::Z, Y == 0);
        setFlag(FLAG::Z, Y & 0x80);
        return 0;
    }
    Byte CPU::JMP(){
        //ABS, IND;
        PC = addr_abs;
        return 0;
    }
    Byte CPU::JSR(){
        //ABS;
        --PC;//push PC - 1;
        store(kSTACK_BASE + S, (PC >> 8) & 0xff);
        --S; //push high byte first;
        store(kSTACK_BASE + S, PC & 0xff);
        --S;
        PC = addr_abs;
        return 0;
    }
    Byte CPU::LDA(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A = fetch_buf;
        setFlag(FLAG::Z, A == 0);
        setFlag(FLAG::N, A & 0x80);
        return 1;//add one cycle if page cross;
    }

    Byte CPU::LDX(){
        //IMM, ZP0, ZPY, ABS, ABY;
        fetch(addr_abs, fetch_buf);
        X = fetch_buf;
        setFlag(FLAG::Z, X == 0);
        setFlag(FLAG::N, X & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::LDY(){
        //IMM, ZP0, ZPX, ABS, ABX;
        fetch(addr_abs, fetch_buf);
        Y = fetch_buf;
        setFlag(FLAG::Z, Y == 0);
        setFlag(FLAG::N, Y & 0x80);
        return 1;//add one cycle if page cross;
    }
    Byte CPU::LSR(){
        //ACCUM, ZP0, ZPX, ABS, ABX;
        if (cur_opcode == 0x4A){//opcode of LSR-ACCUM;
            setFlag(FLAG::C, A & 0x1);
            A >>= 1;
            setFlag(FLAG::Z, A == 0);
            setFlag(FLAG::N, A & 0x80);
        }
        else {
            fetch(addr_abs, fetch_buf);
            setFlag(FLAG::C, fetch_buf & 0x1);
            fetch_buf >>= 1;
            setFlag(FLAG::Z, fetch_buf == 0);
            setFlag(FLAG::N, fetch_buf & 0x80);
            store(addr_abs, fetch_buf);
        }
        return 0;
    }
    Byte CPU::NOP(){
        //IMP
        return 0;
    }
    Byte CPU::ORA(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        A |= fetch_buf;
        setFlag(FLAG::Z, A == 0);
        setFlag(FLAG::N, A & 0x80);
        return 0;
    }

    Byte CPU::PHA(){
        //IMP
        store(kSTACK_BASE + S, A);
        --S;
        return 0;
    }
    Byte CPU::PHP(){
        //IMP
        store(kSTACK_BASE + S, P.val);
        --S;
        return 0;
    }
    Byte CPU::PLA(){
        //IMP
        ++S;
        fetch(kSTACK_BASE + S, fetch_buf);
        A = fetch_buf;
        setFlag(FLAG::Z, A == 0);
        setFlag(FLAG::N, A & 0x80);
        return 0;
    }
    Byte CPU::PLP(){
        //IMP
        ++S;
        fetch(kSTACK_BASE + S, fetch_buf);
        P.val = fetch_buf;
        return 0;
    }
    Byte CPU::ROL(){
        //ACCUM, ZP0, ZPX, ABS, ABX;
        Byte b7;
        if (cur_opcode == 0x2A){//opcode of ROL-ACCUM;
            b7 = (A & 0x80) ? 1 : 0;
            setFlag(FLAG::C, b7);
            A <<= 1;
            A |= b7;
            setFlag(FLAG::Z, A == 0);
            setFlag(FLAG::N, A & 0x80);
        }
        else {
            fetch(addr_abs, fetch_buf);
            b7 = (fetch_buf & 0x80) ? 1 : 0;
            setFlag(FLAG::C, b7);
            fetch_buf <<= 1;
            fetch_buf |= b7;
            setFlag(FLAG::Z, fetch_buf == 0);
            setFlag(FLAG::N, fetch_buf & 0x80);
            store(addr_abs, fetch_buf);
        }
        return 0;
    }

    Byte CPU::ROR(){
        //ACCUM, ZP0, ZPX, ABS, ABX;
        Byte b0;
        if (cur_opcode == 0x6A){//opcode of ROR-ACCUM;
            b0 = A & 0x1;
            setFlag(FLAG::C, b0);
            A >>= 1;
            A |= (b0 << 7);
            setFlag(FLAG::Z, A == 0);
            setFlag(FLAG::N, b0);
        }
        else {
            fetch(addr_abs, fetch_buf);
            b0 = fetch_buf & 0x1;
            setFlag(FLAG::C, b0);
            fetch_buf >>= 1;
            fetch_buf |= (b0 << 7);
            setFlag(FLAG::Z, fetch_buf == 0);
            setFlag(FLAG::N, b0);
            store(addr_abs, fetch_buf);
        }
        return 0;
    }
    Byte CPU::RTI(){
        //IMP
        //pull status flags;
        ++S;
        fetch(kSTACK_BASE + S, fetch_buf);
        P.val = fetch_buf;
        setFlag(FLAG::B, false);
        setFlag(FLAG::U, false);
        //pull PC;
        ++S;
        fetch(kSTACK_BASE + S, fetch_buf);
        PC = fetch_buf; //pull low byte first;
        ++S;
        fetch(kSTACK_BASE + S, fetch_buf);
        PC |= (Word)fetch_buf << 8;
        return 0;
    }
    Byte CPU::RTS(){
        //IMP
        ++S;
        fetch(kSTACK_BASE + S, fetch_buf);
        PC = fetch_buf; //pull low byte first;
        ++S;
        fetch(kSTACK_BASE + S, fetch_buf);
        PC |= (Word)fetch_buf << 8;
        ++PC;
        return 0;
    }
    Byte CPU::SBC(){
        //IMM, ZP0, ZPX, ABS, ABX, ABY, IZX, IZY;
        fetch(addr_abs, fetch_buf);
        Word res = A - (!P.C);//carry not = borrow;
        res -= fetch_buf;
        setFlag(FLAG::C, res > A);
        //test zero;
        setFlag(FLAG::Z, (res & 0xff) == 0);
        //test overflow;
        bool sign_acc = A & 0x80;
        bool sign_fch = fetch_buf & 0x80;
        bool sign_res = res & 0x80;
        setFlag(FLAG::V, (sign_acc ^ sign_fch) && (sign_acc ^ sign_res) );
        //test negative;
        setFlag(FLAG::N, sign_res);
        A = res & 0xff;
        return 1;//add one cycle if page cross;
    }
    Byte CPU::SEC(){
        //IMP
        setFlag(FLAG::C, true);
        return 0;
    }
    Byte CPU::SED(){
        //IMP
        setFlag(FLAG::D, true);
        return 0;
    }

    Byte CPU::SEI(){
        //IMP
        setFlag(FLAG::I, true);
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
        setFlag(FLAG::Z, X == 0);
        setFlag(FLAG::N, X & 0x80);
        return 0;
    }

    Byte CPU::TAY(){
        //IMP
        Y = A;
        setFlag(FLAG::Z, Y == 0);
        setFlag(FLAG::N, Y & 0x80);
        return 0;
    }
    Byte CPU::TSX(){
        //IMP
        X = S;
        setFlag(FLAG::Z, X == 0);
        setFlag(FLAG::N, X & 0x80);
        return 0;
    }
    Byte CPU::TXA(){
        //IMP
        A = X;
        setFlag(FLAG::Z, A == 0);
        setFlag(FLAG::N, A & 0x80);
        return 0;
    }
    Byte CPU::TXS(){
        //IMP
        S = X;
        setFlag(FLAG::Z, S == 0);
        setFlag(FLAG::N, S & 0x80);
        return 0;
    }
    Byte CPU::TYA(){
        //IMP
        A = Y;
        setFlag(FLAG::Z, Y == 0);
        setFlag(FLAG::N, Y & 0x80);
        return 0;
    }
    
    Byte CPU::XXX(){
        //IMP
        return 0;
    }

};