#include "Mapper_003.h"

namespace nes {
	Mapper_003::Mapper_003(Cartridge& _cart)
		: Mapper {_cart}
	{
		const NESHeader& r_h = cart->get_header();
		if (r_h.num_prg_rom == 1) {
			prg_addr_mask = 0x3fff;
		}
		else if (r_h.num_prg_rom == 2) {
			prg_addr_mask = 0x7fff;
		}
		Mapper::init_nt_mirror();
	}

	Mapper_003::~Mapper_003()
	{
	}
	
	void Mapper_003::reset()
	{
	}

	bool Mapper_003::cpu_read(Word addr, Byte& data)
	{
		//Mapper_003 do not support PRG-RAM;
		if (addr >= 0x8000) {
			return Mapper::cart->read_prg_rom(addr & prg_addr_mask, data);
		}
		return false;
	}

	bool Mapper_003::cpu_write(Word addr, Byte data)
	{
		if (addr >= 0x8000) {
			//CNROM only implements the lowest 2 bits, capping it at 32 KiB CHR.
			//Other boards may implement 4 or more bits for larger CHR.
			chr_base_addr = kWINDOW_8K * (data & 0x03);//now only support CHR-Mem <= 32KiB;
			return true;
		}
		return false;
	}

	bool Mapper_003::ppu_read(Word addr, Byte& data)
	{
		//PPU gives addresses below $3F00;
		if (addr < 0x2000) {
			return cart->read_chr_rom(chr_base_addr | addr, data);
		}
		else {//$2000 ~ $3EFF;
			//map $3XXX to $2XXX;
			data = (this->*Mapper::nt_mirror_map)(addr & 0x2fff);
			return true;
		}
	}

	bool Mapper_003::ppu_write(Word addr, Byte data)
	{
		//PPU gives addresses below $3F00;
		if (addr < 0x2000) {
			return cart->write_chr_ram(chr_base_addr | addr, data);
		}
		else {//$2000 ~ $3EFF;
			//map $3XXX to $2XXX;
			(this->*Mapper::nt_mirror_map)(addr & 0x2fff) = data;
			return true;
		}
	}

	bool Mapper_003::irq_state()
	{
		return false;
	}

	bool Mapper_003::count_scanline()
	{
		return false;
	}
};