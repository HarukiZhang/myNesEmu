#ifndef MAPPER_NROM_H
#define MAPPER_NROM_H

#include "Mapper.h"

namespace nes {

    class Mapper_NROM : public Mapper {
        // PRG ROM size: 16 KiB for NROM-128, 32 KiB for NROM-256
        // PRG ROM bank size: Not bankswitched
        // PRG RAM: 2 or 4 KiB, not bankswitched, only in Family Basic 
        //      (but most emulators provide 8)
        // CHR capacity: 8 KiB ROM         but most emulators support RAM
        // CHR bank size: Not bankswitched, see CNROM
        // Nametable mirroring: Solder pads select vertical or horizontal mirroring
        // Subject to bus conflicts: Yes, but irrelevant
    public:
        Mapper_NROM(Cartridge &_cart);
        ~Mapper_NROM();

        bool cpu_read(Word addr, Byte &data) override;
        bool cpu_write(Word addr, Byte data) override;

        bool ppu_read(Word addr, Byte &data) override;
        bool ppu_write(Word addr, Byte data) override;

        //void set_nt_mirror(); <-- no need to dynamically change;

    private:
        Word addr_mask = 0x3fff;//$7FFF for NROM_256; $3FFF for NROM_128;
        bool has_prg_ram = false;
    };

};//end nes

#endif