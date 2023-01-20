#include "Mapper_NROM.h"
#include "Cartridge.h"

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
        //MainBus wont give addresses below $6000;
        if (addr < 0x8000){
            if (has_prg_ram)//               <-- is this if-judge necessary?
                return Mapper::cart->read_cart_ram(addr - 0x6000, data);
            else return false;
        }
        else if (addr < 0xC000){
            return Mapper::cart->read_prg_rom(addr - 0x8000, data);
        }
        else if (addr < 0x10000){
            return Mapper::cart->read_prg_rom((addr - 0x8000) - (is_NROM_128 * 0x4000), data);
            // 0xC123 - 0x8000 - 0x4000 (one bank); o
            // 0xC123 - 0x8000 - 0 (two banks);     o
            // 0x9123 - 0x8000 - 0x4000 (one bank); x
            // 0x9123 - 0x8000 - 0 (two banks);     o
        }
        return false;
    }

    bool Mapper_NROM::cpu_write(Word addr, Byte data){
        if (addr < 0x8000){
            if (has_prg_ram)
                return Mapper::cart->write_cart_ram(addr - 0x6000, data);
            else return false;
        }
        //it seems not writable to prg-rom and chr-rom;
        return false;
    }

    bool Mapper_NROM::ppu_read(Word addr, Byte &data){
        //HybridBus only gives addr below 0x2000;
        return Mapper::cart->read_chr_rom(addr, data);
    }
    bool Mapper_NROM::ppu_write(Word addr, Byte data){
        //chr-rom is not writable;
        return false;
    }

};//end nes;