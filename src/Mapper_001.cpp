#include "Mapper_001.h"

namespace nes {
	Mapper_001::Mapper_001(Cartridge& _cart)
		: Mapper{_cart}
	{

	}

	Mapper_001::~Mapper_001(){}

	void Mapper_001::reset()
	{
		//To reset the mapper, which clears the shift register and sets the PRG ROM bank mode to 3 
		//(fixing the last bank at $C000 and allowing the 16 KB bank at $8000 to be switched), 
		//one need only do a single write to any ROM address with a 1 in bit 7;
		shift_reg = 0;
		ctrl_reg |= 0x0c;
		prg_rom_base_addr_hi = (Mapper::cart->get_header().num_prg_rom - 1) * kPRG_ROM_SIZE;
		prg_rom_base_addr_lo = 0x0000;
	}

	bool Mapper_001::cpu_read(Word addr, Byte& data)
	{
		//MainBus only gives addresses beyond $6000;
		switch ((addr & 0xf000) >> 12) {
		case 0x6://PRG-RAM
		case 0x7://window : 8 KB; capacity : 32 KB;
			return Mapper::cart->read_prg_ram(prg_ram_base_addr | (addr & 0x1fff), data);
			break;
		case 0x8://PRG-ROM low bank
		case 0x9:
		case 0xA:
		case 0xB:
			return Mapper::cart->read_prg_rom(prg_rom_base_addr_lo | (addr & 0x3fff), data);
			break;
		case 0xC://PRG-ROM high bank
		case 0xD:
		case 0xE:
		case 0xF:
			return Mapper::cart->read_prg_rom(prg_rom_base_addr_hi | (addr & 0x3fff), data);
			break;
		}
		return false;
	}

	bool Mapper_001::cpu_write(Word addr, Byte data)
	{
		//MainBus only gives addresses beyond $6000;
		if (addr < 0x8000) return false;//ie, $6000 ~ $8000;
		
		if (data & 0x80) {
			reset();
			return false;// <-- or return true?
		}
		//then bit 7 of data == 0;

		switch (write_counter) {
		case 0:
		case 1:
		case 2:
		case 3:
			shift_reg = ((data & 0x01) << 4) | ((shift_reg >> 1) & 0x0f);//shift_reg is a 5-bit reg;
			++write_counter;
			return true;
			break;
		case 4:
			shift_reg = ((data & 0x01) << 4) | ((shift_reg >> 1) & 0x0f);
			write_counter = 0;
			break;
		default:
			break;
		}

		switch ((addr & 0xf000) >> 12){
		case 0x8://Control
		case 0x9:
			ctrl_reg = shift_reg & 0x1f;
			set_control_reg();
			break;
		case 0xA://CHR Bank 0
		case 0xB:
			chr_regs[0] = shift_reg & 0x1f;
			break;
		case 0xC://CHR Bank 1
		case 0xD:
			chr_regs[1] = shift_reg & 0x1f;
			break;
		case 0xE://PRG Bank
		case 0xF:
			prg_reg = shift_reg & 0x1f;
			break;
		}
	}

	void Mapper_001::set_control_reg()
	{
		//	4bit0
		//	-----
		//	CPPMM
		//	|||||
		//	|||++- Mirroring(0: one - screen, lower bank; 1: one - screen, upper bank;
		//	|||               2: vertical; 3: horizontal)
		//	|++--- PRG ROM bank mode(0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
		//	|						  2: fix first bank at $8000 and switch 16 KB bank at $C000;
		//	|						  3: fix last bank at $C000 and switch 16 KB bank at $8000)
		//	+----- CHR ROM bank mode(0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
		switch (ctrl_reg & 0x3) {
		case 0: //TODO: one lower bank;
			break;
		case 1: //TODO: one upper;
			break;
		case 2: Mapper::nt_mirror_map = &Mapper::mirror_vertical;			break;
		case 3: Mapper::nt_mirror_map = &Mapper::mirror_horizontal;			break;
		}

		//TODO: PRG ROM bank mode

		//TODO: CHR ROM bank mode
	}
};