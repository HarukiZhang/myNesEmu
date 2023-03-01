#ifndef MAPPER_003_H
#define MAPPER_003_H

#include "Mapper.h"

namespace nes {

	class Mapper_003 : public Mapper {
		//PRG ROM capacity	16K or 32K
		//PRG ROM window	n / a
		//PRG RAM capacity	None
		//CHR capacity		32K(2M oversize version)
		//CHR window		8K
		//NT mirroring		Fixed H or V
		//Bus conflicts		Yes(CNROM, some others)
		//IRQ				No
		//Audio				One case
		//
		//Overview:
		//	PRG ROM size		: 16 KiB or 32 KiB
		//	PRG ROM bank size	: Not bankswitched
		//	PRG RAM				: None
		//	CHR capacity		: Up to 2048 KiB ROM
		//	CHR bank size		: 8 KiB
		//	NT mirroring		: Fixed vertical or horizontal mirroring
		//	Bus conflicts		: Yes(CNROM), but not all compatible boards have bus conflicts.
		//
		//Banks:
		//	PPU $0000 - $1FFF : 8 KB switchable CHR ROM bank

	public:
		Mapper_003(Cartridge& _cart);
		~Mapper_003();

		void reset() override;

		bool cpu_read(Word addr, Byte& data) override;
		bool cpu_write(Word addr, Byte data) override;

		bool ppu_read(Word addr, Byte& data) override;
		bool ppu_write(Word addr, Byte data) override;

		bool irq_state() override;
		bool count_scanline() override;

	private:
		Word prg_addr_mask = 0x3fff;//$7FFF for 32K; $3FFF for 16K;
		Phad chr_base_addr = 0;
		//Byte chr_select_mask = 0x03;//for CNROM; vary for larger CHR-ROM size;
	};
};


#endif