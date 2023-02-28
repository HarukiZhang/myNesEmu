#ifndef MAPPER_004_H
#define MAPPER_004_H

#include "Mapper.h"

namespace nes {

	class PPU;//forw decl;

	class Mapper_004 : public Mapper {

		//debug:
		friend class PPU;

		//PRG ROM capacity	512K
		//PRG ROM window	8K + 8K + 16K fixed
		//PRG RAM capacity	8K
		//PRG RAM window	8K
		//CHR capacity		256K
		//CHR window		2Kx2 + 1Kx4
		//NT mirroring		H or V, switchable, or 4 fixed
		//Bus conflicts		No
		//IRQ				Yes
		//Audio				No
		//
		//Banks:
		//	CPU $6000 - $7FFF: 8 KB PRG RAM bank(optional)
		//	CPU $8000 - $9FFF(or $C000 - $DFFF) : 8 KB switchable PRG ROM bank
		//	CPU $A000 - $BFFF : 8 KB switchable PRG ROM bank
		//	CPU $C000 - $DFFF(or $8000 - $9FFF) : 8 KB PRG ROM bank, fixed to the second - last bank
		//	CPU $E000 - $FFFF : 8 KB PRG ROM bank, fixed to the last bank
		//	PPU $0000 - $07FF(or $1000 - $17FF) : 2 KB switchable CHR bank
		//	PPU $0800 - $0FFF(or $1800 - $1FFF) : 2 KB switchable CHR bank
		//	PPU $1000 - $13FF(or $0000 - $03FF) : 1 KB switchable CHR bank
		//	PPU $1400 - $17FF(or $0400 - $07FF) : 1 KB switchable CHR bank
		//	PPU $1800 - $1BFF(or $0800 - $0BFF) : 1 KB switchable CHR bank
		//	PPU $1C00 - $1FFF(or $0C00 - $0FFF) : 1 KB switchable CHR bank
	public:
		Mapper_004(Cartridge &_cart);
		~Mapper_004();

		void reset() override;

		bool cpu_read(Word addr, Byte& data) override;
		bool cpu_write(Word addr, Byte data) override;

		bool ppu_read(Word addr, Byte& data) override;
		bool ppu_write(Word addr, Byte data) override;

		bool irq_state() override;
		bool count_scanline() override;

	private:
		void set_bank_data();

	private:
		//memory mapping register:
		Byte bank_select = 0;		//$8000-$9FFE, even
		Byte bank_data = 0;			//$8001-$9FFF, odd
		Byte mirroring = 0;			//$A000-$BFFE, even; 0: vertical; 1: horizontal;
		//Byte prg_ram_protect = 0;	//$A001-$BFFF, odd   <-- no need;
		
		//scanline counting registers:
		Byte irq_latch = 0;			//$C000-$DFFE, even; latch the value to be reloaded into irq counter;
		bool irq_reload = false;	//$C001-$DFFF, odd;  if true, it indicates an irq counter reload is pending;
		//Byte irq_disable = 0;		//$E000-$FFFE, even  <-- no need;
		bool irq_enable = false;	//$E001-$FFFF, odd;  if true, it allows a pending irq to trigger (ie, when counter to 0);

		Word irq_counter = 0;
		bool irq_pending = false;//if true, it indicates that now an irq has been triggered;

		//PRG-ROM bank base addresses:
		//Phad prg_base_addrs_inv[2]{}; //0 :  R6
		//							  //1 : (-2)
		//Phad prg_base_addr_swap = 0;  //     R7
		//Phad prg_base_addr_last = 0;  //    (-1)

		//PRG-ROM bank base address array:
		// |0      |1      |2      |3      |
		// |$8000~ |$A000~ |$C000~ |$E000~ |
		Phad prg_base_addrs[4];
		Byte num_prg_rom = 0;

		//CHR-Mem bank base address matrix:
		//   |0        |1        |2        |3        |
		// |0|2KB_0_lo |2KB_0_hi |2KB_1_lo |2KB_1_hi |
		// |1|1KB_0    |1KB_1    |1KB_2    |1KB_3    |
		Phad chr_base_addrs[2][4]{};  //unit : 1KiB;

		bool is_four_screen_mirror = false;
		bool prg_ram_chip_enable = false;
		bool prg_ram_write_protect = false;
	};
};

#endif