#include "Mapper_001.h"

#include <iostream>

namespace nes {
	Mapper_001::Mapper_001(Cartridge& _cart)
		: Mapper{_cart}
	{
		const NESHeader& r_h = Mapper::cart->get_header();
		num_prg_rom = r_h.num_prg_rom;
		//initialize nt mirror according to cart->header;
		Mapper::nt_mirror_map = _cart.get_header().mirror_hv ?
			&Mapper::mirror_vertical : &Mapper::mirror_horizontal;
		//set to a known state;
		this->reset();
	}

	Mapper_001::~Mapper_001(){}

	void Mapper_001::reset()
	{
		//to set the mapper to a known state;
		shift_reg = 0;
		ctrl_reg = 0x1c;// <-- why this?

		prg_rom_base_addr_hi = (num_prg_rom - 1) * kWINDOW_16K;//fix to the last bank;
		prg_rom_base_addr_lo = 0x0000;
		
		chr_mem_base_addr[0] = 0;
		chr_mem_base_addr[1] = 0;// high addr <-- is this correct ?
		
		prg_ram_base_addr = 0;

		write_counter = 0;
		prg_ram_disable = false;

	}

	bool Mapper_001::cpu_read(Word addr, Byte& data)
	{
		//MainBus only gives addresses beyond $6000;
		switch ((addr & 0xf000) >> 12) {
		case 0x6://PRG-RAM
		case 0x7://window : 8 KB; capacity : 32 KB;
			if (!prg_ram_disable) {
				return Mapper::cart->read_prg_ram(prg_ram_base_addr | (addr & 0x1fff), data);
			}
			else {
				data = NULL;//should be open bus;
				return false;
			}
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
		default:
			std::clog << "Mapper_001::cpu_read : swtich error" << std::endl;
			return false;
			break;
		}
	}

	bool Mapper_001::cpu_write(Word addr, Byte data)
	{
		//MainBus only gives addresses beyond $6000;
		if (addr < 0x8000) {//ie, $6000 ~ $8000;
			if (!prg_ram_disable) {
				return Mapper::cart->write_prg_ram(prg_ram_base_addr | (addr - 0x6000), data);
			}
			else {
				return false;
			}
		}
		//then addr >= 0x8000;
		
		//reset writes;
		if (data & 0x80) {
			//To reset the mapper, which clears the shift register and sets the PRG ROM bank mode to 3 
			//(fixing the last bank at $C000 and allowing the 16 KB bank at $8000 to be switched), 
			//one need only do a single write to any ROM address with a 1 in bit 7;
			shift_reg = 0;
			ctrl_reg = ctrl_reg | 0x0c;//to set the PRG-ROM bank mode to 3;
			return false;// <-- or return true?
		}
		//then bit 7 of data == 0;

		//receive five writes;
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
			std::clog << "Mapper_001::cpu_write : write_counter swtich error" << std::endl;
			return false;
			break;
		}

		//manipulate config or mapping;
		//Only on the fifth write does the address matter;
		switch ((addr & 0x7000) >> 12){//the mapper only sees bits 13 and 14;
		case 0x0://Control
		case 0x1:
			ctrl_reg = shift_reg & 0x1f;
			set_control_reg();
			break;
		case 0x2://CHR Bank 0
		case 0x3:
			chr_regs[0] = shift_reg & 0x1f;
			set_chr_bank(0);
			break;
		case 0x4://CHR Bank 1
		case 0x5:
			chr_regs[1] = shift_reg & 0x1f;
			set_chr_bank(1);
			break;
		case 0x6://PRG Bank
		case 0x7:
			prg_reg = shift_reg & 0x1f;
			set_prg_bank();
			break;
		default:
			std::clog << "Mapper_001::cpu_write : addr swtich error" << std::endl;
			return false;
			break;
		}

		//After the fifth write, the shift register is cleared automatically, 
		//	so writing again with bit 7 set to clear the shift register is not needed.
		shift_reg = 0;
		return true;
	}

	inline void Mapper_001::set_control_reg()
	{
		//	4bit0
		//	-----
		//	CPPMM
		//	|||||
		//	|||++- Mirroring(0: one - screen, lower bank; 1: one - screen, upper bank;
		//	|||              2: vertical; 3: horizontal)
		//	|++--- PRG ROM bank mode(0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
		//	|						    2: fix first bank at $8000 and switch 16 KB bank at $C000;
		//	|							3: fix last bank at $C000 and switch 16 KB bank at $8000)
		//	+----- CHR ROM bank mode(0: switch 8 KB at a time; 1: switch two separate 4 KB banks)
		
		switch (ctrl_reg & 0x3) {
		case 0: Mapper::nt_mirror_map = &Mapper::mirror_a_only;     break;
		case 1: Mapper::nt_mirror_map = &Mapper::mirror_b_only;     break;
		case 2: Mapper::nt_mirror_map = &Mapper::mirror_vertical;   break;
		case 3: Mapper::nt_mirror_map = &Mapper::mirror_horizontal; break;
		}

		//PRG ROM bank mapping should perform when prg_reg is updated;

		//so does CHR ROM bank mapping;
	}

	inline void Mapper_001::set_chr_bank(Byte num)
	{
		//CHR bank 0 : Select 4 KB or 8 KB CHR bank at PPU $0000 (low bit ignored in 8 KB mode)
		//			   [SNROM var] bit 4 : PRG RAM disable (0: enable, 1: open bus) (ignored in 8 KB mode)
		//CHR bank 1 : Select 4 KB CHR bank at PPU $1000 (ignored in 8 KB mode)
		num &= 0x1;
		//CHR ROM bank mode
		if ((ctrl_reg & 0x10) == 0x10) {//1: switch two separate 4 KB banks;
			//general setting;
			chr_mem_base_addr[num] = (chr_regs[num] & 0x1f) * kWINDOW_4K;
			//SNROM setting;
			prg_ram_disable = (chr_regs[num] & 0x10) > 0;//"this is merely for write protection and not strictly required for compatible emulation."
		}
		else {//0: switch 8 KB at a time;
			if (num == 1) return;
			//general setting;
			chr_mem_base_addr[0] = (chr_regs[0] & 0x1e) * kWINDOW_4K;//because the selector uses upper 4 bits with lsb == 0;
			chr_mem_base_addr[1] = chr_mem_base_addr[0] + kWINDOW_4K;
			//SNROM setting;
			prg_ram_disable = (chr_regs[0] & 0x10) > 0;
		}
	}

	inline void Mapper_001::set_prg_bank()
	{
		//4bit0
		//-----
		//RPPPP
		//|||||
		//|++++- Select 16 KB PRG ROM bank (low bit ignored in 32 KB mode)
		//+----- MMC1B and later: PRG RAM chip enable (0: enabled; 1: disabled; ignored on MMC1A)
		//	     MMC1A: Bit 3 bypasses fixed bank logic in 16K mode (0: affected; 1: bypassed)
		// 
		//The high bit does not select a PRG ROM bank. 
		//MMC1 with 512K was supported by re-using a line from the CHR banking controls.

		switch ((ctrl_reg & 0x0c) >> 2) {//PRG ROM bank mode
		case 0x0:
		case 0x1:
			//0, 1: switch 32 KB at $8000, ignoring low bit of bank number;
			prg_rom_base_addr_lo = (prg_reg & 0x0e) * kWINDOW_16K;
			prg_rom_base_addr_hi = prg_rom_base_addr_lo + kWINDOW_16K;
			break;
		case 0x2:
			//2 : fix first bank at $8000 and switch 16 KB bank at $C000;
			prg_rom_base_addr_lo = 0x00000000;
			prg_rom_base_addr_hi = (prg_reg & 0x0f) * kWINDOW_16K;
			break;
		case 0x3:
			//3 : fix last bank at $C000 and switch 16 KB bank at $8000;
			prg_rom_base_addr_hi = (num_prg_rom - 1) * kWINDOW_16K;
			prg_rom_base_addr_lo = (prg_reg & 0x0f) * kWINDOW_16K;
			break;
		}
	}

	bool Mapper_001::ppu_read(Word addr, Byte& data)
	{
		//PPU only gives addresses below $2000;
		return Mapper::cart->read_chr_rom(chr_mem_base_addr[(addr & 0x1000) >> 12] | (addr & 0x0fff), data);
	}

	bool Mapper_001::ppu_write(Word addr, Byte data)
	{
		//PPU only gives addresses below $2000;
		return Mapper::cart->write_chr_ram(chr_mem_base_addr[(addr & 0x1000) >> 12] | (addr & 0x0fff), data);
	}
};