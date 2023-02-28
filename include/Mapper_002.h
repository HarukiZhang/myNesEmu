#ifndef MAPPER_002_H
#define MAPPER_002_H

#include "Mapper.h"

namespace nes {
	class Mapper_002 : public Mapper {
		//PRG ROM capacity	256K/4096K
		//PRG ROM window	16K + 16K fixed
		//PRG RAM capacity	None
		//CHR capacity		8K
		//CHR window		n / a
		//NT mirroring		Fixed H or V, controlled by solder pads
		//Bus conflicts		Yes / No
		//IRQ				No
		//Audio				No
		//
		//Banks:
		//	CPU $8000 - $BFFF	: 16 KB switchable PRG ROM bank
		//	CPU $C000 - $FFFF	: 16 KB PRG ROM bank, fixed to the last bank
		//
		//Registers:
		//	Bank select($8000 - $FFFF)
	public:
		Mapper_002(Cartridge &_cart);
		~Mapper_002();

		void reset() override;

		bool cpu_read(Word addr, Byte& data) override;
		bool cpu_write(Word addr, Byte data) override;

		bool ppu_read(Word addr, Byte& data) override;
		bool ppu_write(Word addr, Byte data) override;

		bool irq_state() override;
		bool count_scanline() override;

	private:
		Phad high_bank_base_addr = 0;
		Phad low_bank_base_addr = 0;
	};
};

#endif