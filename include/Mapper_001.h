#ifndef MAPPER_001_H
#define MAPPER_001_H

#include "Mapper.h"

namespace nes {
	class Mapper_001 : public Mapper {
		//PRG ROM capacity	256K(512K)
		//PRG ROM window	16K + 16K fixed or 32K
		//PRG RAM capacity	32K
		//PRG RAM window	8K
		//CHR capacity		128K
		//CHR window		4K + 4K or 8K
		//NT mirroring		H, V, or 1, switchable
		//Bus conflicts		No
		//IRQ				No
		//Audio				No
		//
		//Banks:
		//	CPU $6000 - $7FFF :  8 KB PRG RAM bank, (optional)
		//	CPU $8000 - $BFFF : 16 KB PRG ROM bank, either switchable or fixed to the first bank
		//	CPU $C000 - $FFFF : 16 KB PRG ROM bank, either fixed to the last bank or switchable
		//	PPU $0000 - $0FFF :  4 KB switchable CHR bank
		//	PPU $1000 - $1FFF :  4 KB switchable CHR bank
		//
		//Variants:
		//			PRG-ROM			PRG-RAM			CHR-Mem
		// SNROM :	128/256 KB		8 KB			8 KB RAM/ROM
		//
	public:
		Mapper_001(Cartridge &_cart);
		~Mapper_001();

		void reset() override;

		bool cpu_read(Word addr, Byte& data) override;
		bool cpu_write(Word addr, Byte data) override;

		bool ppu_read(Word addr, Byte& data) override;
		bool ppu_write(Word addr, Byte data) override;
	
	private:
		void set_control_reg();
		void set_chr_bank(Byte num);
		void set_prg_bank();

	private:
		//Internal Registers;
		//Byte load_reg  = 0;//8 bits; <-- no need;
		Byte shift_reg = 0;  //5 bits;
		Byte ctrl_reg  = 0;  //5 bits;     $8000 - $9FFF
		Byte chr_regs[2]{};  //5 bits * 2; $A000 - $BFFF
						     //			   $C000 - $DFFF
		Byte prg_reg   = 0;  //5 bits;     $E000 - $FFFF

		Phad prg_rom_base_addr_lo = 0;//		  map to CPU $8000 ~ $BFFF : 16 KiB;
		Phad prg_rom_base_addr_hi = 0;//		  map to CPU $C000 ~ $FFFF : 16 KiB;
		Phad prg_ram_base_addr    = 0;//		  map to CPU $6000 ~ $7FFF :  8 KiB;
		Phad chr_mem_base_addr[2]{};  //0 = low : map to PPU $0000 ~ $0FFF :  4 KiB;
									  //1 = high: map to PPU $1000 ~ $1FFF :  4 KiB;

		Byte write_counter = 0;
		Byte num_prg_rom = 0;
		bool prg_ram_disable = false;
	};
};

#endif