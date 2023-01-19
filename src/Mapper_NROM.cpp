#include "Mapper_NROM.h"

namespace nes {

    Mapper_NROM::Mapper_NROM(Cartridge &_cart)
        : Mapper{_cart}
    {
        if(_cart.header.num_prg_rom == 2)
            is_NROM_128 = false;//which means it is NROM-256;
        if (_cart.header.num_prg_ram == 0)
            has_prg_ram = true;//is this correct ?
    }

    bool Mapper_NROM::cpu_read(Word addr, Byte &data){
        //according to MMC, remap the addr to fit the full rom size;
        if (addr < 0x6000) return false;
        else if (addr < 0x8000){
            //TODO: support for PRG-RAM;
            return false;
        }
        else if (addr < 0xC000){
            return cart->read_prg_rom(addr - 0x8000, data);
        }
        else if (addr < 0x10000){
            return cart->read_prg_rom((addr - 0x8000) - (is_NROM_128 * 0x4000), data);
            // 0xC123 - 0x8000 - 0x4000 (one bank); o
            // 0xC123 - 0x8000 - 0 (two banks);     o
            // 0x9123 - 0x8000 - 0x4000 (one bank); x
            // 0x9123 - 0x8000 - 0 (two banks);     o
        }
        return false;
    }

    bool Mapper_NROM::cpu_write(Word addr, Byte data){
        
    }

    bool Mapper_NROM::ppu_read(Word addr, Byte &data){
        
    }
    bool Mapper_NROM::ppu_write(Word addr, Byte data){
        
    }

};//end nes;