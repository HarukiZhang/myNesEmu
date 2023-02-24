#include "Mapper_002.h"
#include "Cartridge.h"

namespace nes {
	Mapper_002::Mapper_002(Cartridge& _cart)
		: Mapper{_cart}
	{
		high_bank_base_addr = (Phad)kPRG_ROM_SIZE * (_cart.get_header().num_prg_rom - 1);
		low_bank_base_addr = 0;

		Mapper::nt_mirror_map = _cart.get_header().mirror_hv ?
			&Mapper::mirror_vertical : &Mapper::mirror_horizontal;
	}

	Mapper_002::~Mapper_002(){}

	void Mapper_002::reset() {
		high_bank_base_addr = (Phad)kPRG_ROM_SIZE * (Mapper::cart->get_header().num_prg_rom - 1);
		low_bank_base_addr = 0;//by default, the lower bank is set to the first bank;
	}

	bool Mapper_002::cpu_read(Word addr, Byte& data) {
		if (addr >= 0xC000) {//0xC000 - 0xFFFF
			Mapper::cart->read_prg_rom(high_bank_base_addr + (addr & 0x3fff), data);
			return true;
		}
		else if (addr >= 0x8000) {//0x8000 - 0xBFFF;
			Mapper::cart->read_prg_rom(low_bank_base_addr + (addr & 0x3fff), data);
			return true;
		}
		else {//0x6000 - 0x8000
			//Mapper_002 do not support PRG-RAM;
			return false;
		}
		//MainBus wont give addresses below $6000;

	}
	
	bool Mapper_002::cpu_write(Word addr, Byte data) {
		//write to PRG-ROM effectively select the bank that cpu reads from;
		if (addr >= 0x8000 && addr <= 0xFFFF) {
			//	7  bit  0
			//	---- ----
			//	xxxx pPPP
			//		 ||||
			//		 ++++ - Select 16 KB PRG ROM bank for CPU $8000 - $BFFF
			//				(UNROM uses bits 2 - 0; UOROM uses bits 3 - 0)
			low_bank_base_addr = (Phad)kPRG_ROM_SIZE * (addr & 0x000f);
		}
		return false;//to inform that the ROM is not updated;
	}

	bool Mapper_002::ppu_read(Word addr, Byte& data) {
		//PPU only gives addr 0x0000 to 0x1FFF;
		//if (Mapper::cart->get_header().num_prg_ram)//there's possibility that num_prg_ram is 0;
		return Mapper::cart->read_chr_rom(addr, data);
	}
	
	bool Mapper_002::ppu_write(Word addr, Byte data) {
		//suppose that char-ROM is not writable;
		return Mapper::cart->write_chr_ram(addr, data);
	}
};