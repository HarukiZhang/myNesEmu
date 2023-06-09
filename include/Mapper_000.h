#ifndef MAPPER_000_H
#define MAPPER_000_H

#include "Mapper.h"

namespace nes {

    class Mapper_000 : public Mapper {
        //PRG ROM capacity	16K or 32K
        //PRG ROM window	n / a
        //PRG RAM capacity	2K or 4K in Family Basic only
        //PRG RAM window	n / a
        //CHR capacity	    8K
        //CHR window	    n / a
        //NT mirroring	    Fixed H or V, controlled by solder pads(*V only)
        //Bus conflicts	    Yes
        //IRQ	            No
        //Audio	            No
        // 
        //Overview:
        //  PRG ROM size: 16 KiB for NROM-128, 32 KiB for NROM-256
        //  PRG ROM bank size: Not bankswitched
        //  PRG RAM: 2 or 4 KiB, not bankswitched, only in Family Basic 
        //      (but most emulators provide 8)
        //  CHR capacity: 8 KiB ROM         but most emulators support RAM
        //  CHR bank size: Not bankswitched, see CNROM
        //  Nametable mirroring: Solder pads select vertical or horizontal mirroring
        //  Subject to bus conflicts: Yes, but irrelevant
        //Banks:
        //  All Banks are fixed,
        //  CPU $6000 - $7FFF: Family Basic only : PRG RAM, mirrored as necessary to fill entire 8 KiB window, 
        //      write protectable with an external switch
        //  CPU $8000 - $BFFF : First 16 KB of ROM.
        //  CPU $C000 - $FFFF : Last 16 KB of ROM(NROM - 256) or mirror of $8000 - $BFFF(NROM - 128).
    public:
        Mapper_000(Cartridge &_cart);
        ~Mapper_000();

        void reset() override;

        bool cpu_read(Word addr, Byte &data) override;
        bool cpu_write(Word addr, Byte data) override;

        bool ppu_read(Word addr, Byte &data) override;
        bool ppu_write(Word addr, Byte data) override;

        bool irq_state() override;
        bool count_scanline() override;

    private:
        Word addr_mask = 0x3fff;//$7FFF for NROM_256; $3FFF for NROM_128;
    };

};//end nes

#endif