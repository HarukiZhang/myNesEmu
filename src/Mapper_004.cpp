#include "Mapper_004.h"

#include <iostream>

namespace nes {
	Mapper_004::Mapper_004(Cartridge& _cart)
		: Mapper{_cart}
	{
		const NESHeader& r_h = Mapper::cart->get_header();
		Mapper::init_nt_mirror();
		is_four_screen_mirror = r_h.four_screen;
		num_prg_rom = r_h.num_prg_rom;
		this->reset();
	}

	Mapper_004::~Mapper_004()
	{
	}
	
	void Mapper_004::reset()
	{
		bank_select = 0;		//$8000-$9FFE, even
		bank_data = 0;			//$8001-$9FFF, odd
		mirroring = 0;			//$A000-$BFFE, even
		irq_latch = 0;			//$C000-$DFFE, even
		irq_reload = false;		    //$C001-$DFFF, odd
		irq_enable = false;		    //$E001-$FFFF, odd

		irq_counter = 0;
		irq_pending = false;

		const NESHeader& r_h = Mapper::cart->get_header();
		//prg_base_addrs_inv[0] = 0 * kWINDOW_8K;
		//prg_base_addr_swap    = 1 * kWINDOW_8K;
		////Note that cart header indicates the num_prg_rom in 16 KiB unit;
		//prg_base_addrs_inv[1] = (r_h.num_prg_rom * 2 - 2) * kWINDOW_8K;//fix to the second-last bank in 8KiB unit;
		//prg_base_addr_last    = (r_h.num_prg_rom * 2 - 1) * kWINDOW_8K;//fix to the last bank in 8KiB unit;
		prg_base_addrs[0] = kWINDOW_8K * 0;
		prg_base_addrs[1] = kWINDOW_8K * 1;
		prg_base_addrs[2] = kWINDOW_8K * (r_h.num_prg_rom * 2 - 2);
		prg_base_addrs[3] = kWINDOW_8K * (r_h.num_prg_rom * 2 - 1);

		Mapper::init_nt_mirror();

		chr_base_addrs[0][0] = 0;
		chr_base_addrs[0][1] = 0;
		chr_base_addrs[0][2] = 0;
		chr_base_addrs[0][3] = 0;
		chr_base_addrs[1][0] = 0;
		chr_base_addrs[1][1] = 0;
		chr_base_addrs[1][2] = 0;
		chr_base_addrs[1][3] = 0;

		prg_ram_chip_enable = false;
		prg_ram_write_protect = false;

	}
	
	bool Mapper_004::cpu_read(Word addr, Byte& data)
	{
		//	CPU $6000 - $7FFF					 : 8 KB PRG RAM bank (optional)
		// 
		//	CPU $8000 - $9FFF (or $C000 - $DFFF) : 8 KB switchable PRG ROM bank
		//	CPU $A000 - $BFFF					 : 8 KB switchable PRG ROM bank
		//	CPU $C000 - $DFFF (or $8000 - $9FFF) : 8 KB PRG ROM bank, fixed to the second-last bank
		//	CPU $E000 - $FFFF					 : 8 KB PRG ROM bank, fixed to the last bank
		
		//MainBus only gives addresses beyond $6000;
		if (addr < 0x8000) {//$6000 ~ $7FFF;
			if (prg_ram_chip_enable) {
				return Mapper::cart->read_prg_ram(addr - 0x6000, data);
			}
			else {
				data = NULL;//which should be open bus;
				return false;
			}
		}
		else {
			Byte seg_idx = (addr & 0x6000) >> 13;
			return Mapper::cart->read_prg_rom(prg_base_addrs[seg_idx] | (addr & 0x1fff), data);
		}

		//switch ((addr & 0xf000) >> 12) {
		//case 0x6://PRG-RAM
		//case 0x7://window : 8 KB; capacity : 8 KB;
		//	if (prg_ram_chip_enable) {
		//		return Mapper::cart->read_prg_ram(addr - 0x6000, data);
		//	}
		//	else {
		//		data = NULL;//which should be open bus;
		//		return false;
		//	}
		//	break;
		//case 0x8:
		//case 0x9:
		//	//inv_idx = ((addr & 0x4000) >> 14) ^ ((bank_select & 0x40) >> 6);
		//	return Mapper::cart->read_prg_rom(prg_base_addrs[0], data);
		//	break;
		//case 0xA:
		//case 0xB:
		//	return Mapper::cart->read_prg_rom(prg_base_addrs[1], data);
		//	break;
		//case 0xC:
		//case 0xD:
		//	//inv_idx = ((addr & 0x4000) >> 14) ^ ((bank_select & 0x40) >> 6);
		//	return Mapper::cart->read_prg_rom(prg_base_addrs[2], data);
		//	break;
		//case 0xE:
		//case 0xF:
		//	return Mapper::cart->read_prg_rom(prg_base_addrs[3], data);
		//	break;
		//default:
		//	std::clog << "Mapper_004::cpu_read : swtich error" << std::endl;
		//	return false;
		//	break;
		//}
		//return false;
	}
	
	bool Mapper_004::cpu_write(Word addr, Byte data)
	{
		//MainBus only gives addresses beyond $6000;
		if (addr < 0x8000) {
			if (!prg_ram_write_protect) {
				return Mapper::cart->write_prg_ram(addr - 0x6000, data);
			}
			else return false;
		}
		//then the addr >= 0x8000;

		switch ((addr & 0x7000) >> 12) {
		case 0x0:
		case 0x1:
			if ((addr & 0x1) == 0) { //bank select;
				bank_select = data;
			}
			else { //bank data;
				bank_data = data;
				set_bank_data();
			}
			break;
		case 0x2:
		case 0x3:
			if ((addr & 0x1) == 0) { //mirroring;
				mirroring = data;
				//This bit has no effect on cartridges with hardwired 4-screen VRAM.
				if (!is_four_screen_mirror) {
					Mapper::nt_mirror_map = (mirroring & 0x01) > 0 ? &Mapper::mirror_horizontal : &Mapper::mirror_vertical;
				}
			}
			else { //prg-ram protect;
				prg_ram_chip_enable = (data & 0x80) > 0;
				prg_ram_write_protect = (data & 0x40) > 0;
			}
			break;
		case 0x4:
		case 0x5:
			if ((addr & 0x1) == 0) { //irq latch
				irq_latch = data;
			}
			else { //irq reload;
				//clears the MMC3 IRQ counter immediately, and then 
				//reloads it at the NEXT rising edge of the PPU address, presumably at PPU cycle 260 of the current scanline.
				irq_counter = 0;
				irq_reload = true;
			}
			break;
		case 0x6:
		case 0x7:
			if ((addr & 0x1) == 0) { //irq disable;
				irq_enable = false;
				irq_pending = false;
			}
			else { //irq enable;
				irq_enable = true;
			}
			break;
		}
		return true;
	}

	void Mapper_004::set_bank_data()
	{
		//  000 : R0 : Select 2 KB CHR bank at PPU $0000 - $07FF (or $1000 - $17FF)
		//  001 : R1 : Select 2 KB CHR bank at PPU $0800 - $0FFF (or $1800 - $1FFF)
		//  010 : R2 : Select 1 KB CHR bank at PPU $1000 - $13FF (or $0000 - $03FF)
		//  011 : R3 : Select 1 KB CHR bank at PPU $1400 - $17FF (or $0400 - $07FF)
		//  100 : R4 : Select 1 KB CHR bank at PPU $1800 - $1BFF (or $0800 - $0BFF)
		//  101 : R5 : Select 1 KB CHR bank at PPU $1C00 - $1FFF (or $0C00 - $0FFF)
		//  110 : R6 : Select 8 KB PRG ROM bank at $8000 - $9FFF (or $C000 - $DFFF)
		//  111 : R7 : Select 8 KB PRG ROM bank at $A000 - $BFFF
		Byte inv_idx = 0;
		switch (bank_select & 0x07) {
		case 0x0:
			//2KB CHR bank 0 low;
			//2KB banks may only select even numbered CHR banks. (The lowest bit is ignored.)
			//chr_base_addrs[0][0] = kWINDOW_2K * (bank_data >> 1);
			//2KB CHR bank 0 high;
			//chr_base_addrs[0][1] = kWINDOW_1K + chr_base_addrs[0][0];

			//if bit 7 of bank_select is 0, put 2KB_base_addr to upper row (1) of matrix;
			//if it is 1, put to lower row (0) of matrix;
			inv_idx = (bank_select & 0x80) >> 7;
			chr_base_addrs[inv_idx][0] = kWINDOW_2K * (bank_data >> 1);
			chr_base_addrs[inv_idx][1] = kWINDOW_1K + chr_base_addrs[inv_idx][0];


			break;
		case 0x1:
			//2KB CHR bank 1 low  :
			//chr_base_addrs[0][2] = kWINDOW_2K * (bank_data >> 1);
			//2KB CHR bank 1 high :
			//chr_base_addrs[0][3] = kWINDOW_1K + chr_base_addrs[0][2];

			inv_idx = (bank_select & 0x80) >> 7;
			chr_base_addrs[inv_idx][2] = kWINDOW_2K * (bank_data >> 1);
			chr_base_addrs[inv_idx][3] = kWINDOW_1K + chr_base_addrs[inv_idx][2];


			break;
		case 0x2:
			//1KB CHR bank 0 :
			//chr_base_addrs[1][0] = kWINDOW_1K * bank_data;

			inv_idx = ((bank_select & 0x80) >> 7) ^ 0x01;
			chr_base_addrs[inv_idx][0] = kWINDOW_1K * bank_data;


			break;
		case 0x3:
			//1KB CHR bank 1 :
			//chr_base_addrs[1][1] = kWINDOW_1K * bank_data;

			inv_idx = ((bank_select & 0x80) >> 7) ^ 0x01;
			chr_base_addrs[inv_idx][1] = kWINDOW_1K * bank_data;


			break;
		case 0x4:
			//1KB CHR bank 2 :
			//chr_base_addrs[1][2] = kWINDOW_1K * bank_data;

			
			inv_idx = ((bank_select & 0x80) >> 7) ^ 0x01;
			chr_base_addrs[inv_idx][2] = kWINDOW_1K * bank_data;
			
			
			break;
		case 0x5:
			//1KB CHR bank 3 :
			//chr_base_addrs[1][3] = kWINDOW_1K * bank_data;


			inv_idx = ((bank_select & 0x80) >> 7) ^ 0x01;
			chr_base_addrs[inv_idx][3] = kWINDOW_1K * bank_data;

			break;
		case 0x6:
			//8KB PRG bank 0 : cap: 512KB; window: 8KB;
			//prg_base_addrs_inv[0] = kWINDOW_8K * (bank_data & 0x3f);

			inv_idx = ((bank_select & 0x40) >> 5);
			prg_base_addrs[inv_idx] = kWINDOW_8K * (bank_data & 0x3f);
			inv_idx ^= 0x2;
			prg_base_addrs[inv_idx] = kWINDOW_8K * (num_prg_rom * 2 - 2);

			//if (bank_select & 0x40) {
			//	prg_base_addrs[0] = kWINDOW_8K * (bank_data & 0x3f);
			//	prg_base_addrs[2] = kWINDOW_8K * (num_prg_rom * 2 - 2);
			//}
			//else {
			//	prg_base_addrs[2] = kWINDOW_8K * (bank_data & 0x3f);
			//	prg_base_addrs[0] = kWINDOW_8K * (num_prg_rom * 2 - 2);
			//}

			break;
		case 0x7:
			//8KB PRG bank 1 : cap: 512KB; window: 8KB;
			//prg_base_addr_swap = kWINDOW_8K * (bank_data & 0x3f);

			prg_base_addrs[1] = kWINDOW_8K * (bank_data & 0x3f);

			break;
		}
	}
	
	bool Mapper_004::ppu_read(Word addr, Byte& data)
	{
		//PPU gives addresses below $3F00;
		if (addr < 0x2000) {
			//Byte inv_idx = (addr & 0x1000) >> 12;
			//inv_idx = inv_idx ^ ((bank_select & 0x80) >> 7);
			//Byte seg_idx = (addr & 0x0c00) >> 10;
			//Phad base_addr = chr_base_addrs[inv_idx][seg_idx];// 1KiB unit;
			Byte idx = (addr & 0x1000) >> 12;
			Byte off = (addr & 0x0c00) >> 10;
			return cart->read_chr_rom(chr_base_addrs[idx][off] | (addr & 0x03ff), data);
		}
		else {//$2000 ~ $3EFF;
			//map $3XXX to $2XXX;
			data = (this->*Mapper::nt_mirror_map)(addr & 0x2fff);
			return true;
		}
	}
	
	bool Mapper_004::ppu_write(Word addr, Byte data)
	{
		//PPU gives addresses below $3F00;
		if (addr < 0x2000) {
			//Byte inv_idx = (addr & 0x1000) >> 12;
			//inv_idx = inv_idx ^ ((bank_select & 0x80) >> 7);
			//Byte seg_idx = (addr & 0x0c00) >> 10;
			//Phad base_addr = chr_base_addrs[inv_idx][seg_idx];// 1KiB unit;
			Byte idx = (addr & 0x1000) >> 12;
			Byte off = (addr & 0x0c00) >> 10;
			return cart->write_chr_ram(chr_base_addrs[idx][off] | (addr & 0x03ff), data);
		}
		else {//$2000 ~ $3EFF;
			//map $3XXX to $2XXX;
			(this->*Mapper::nt_mirror_map)(addr & 0x2fff) = data;
			return true;
		}
	}

	bool Mapper_004::irq_state()
	{
		return irq_pending;
	}

	bool Mapper_004::count_scanline()
	{
		//this function should be called by PPU::clock at every scanline when cycle == 260;
		if (irq_counter == 0 || irq_reload) {
			irq_counter = irq_latch;
			irq_reload = false;
		}
		else --irq_counter;

		if (irq_counter == 0 && irq_enable) {
			irq_pending = true;
			return true;
		}
		return false;
	}

};