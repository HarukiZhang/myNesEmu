#include "Mapper.h"

namespace nes {
    bool Mapper::read(Word addr, Byte &data){
        if (addr >= 0x8000 && addr < 0xC000){
            cart->read_prg_rom(addr - 0x8000, data);
            return true;
        }
        else if (addr < 0x10000){
            cart->read_chr_rom(addr - 0xC000, data);
            return true;
        }
        return false;
    }

};//end nes;