#include "HybridBus.h"

namespace nes {
    HybridBus::HybridBus(){}

    void HybridBus::connect(std::shared_ptr<Mapper> &_mapp){
        mapper = _mapp;
    }
    
    bool HybridBus::read(Word addr, Byte &data){
        if (addr < 0x2000){//$0000 - $1FFF : Pattern Table;
            return mapper->ppu_read(addr, data);
        }
        else if (addr < 0x3F00){//$2000 - $3EFF : Name Table / Attribute Table;
            //map $3XXX to $2XXX;
            data = vram[mapper->get_nt_mirror(addr & 0x2fff)];
            return true;
        }
        else {//$3F00 - $3FFF;  map to $3F00 - $3F1F; Palette;
            addr &= ((addr & 0x0003) == 0) ? 0x000f : 0x001f;//if bits 0 and 1 are all 0, clear bit 4;
            data = palette[addr];
            return true;
        }
    }

    bool HybridBus::write(Word addr, Byte data){
        if (addr < 0x2000){
            //Note: some cartridges may contain RAM that can be written;
            return mapper->ppu_write(addr, data);
        }
        else if (addr < 0x3F00){//$2000 - $3EFF;
            vram[mapper->get_nt_mirror(addr & 0x2fff)] = data;
            return true;
        }
        else {//$3F00 - $3FFF;
            addr &= ((addr & 0x0003) == 0) ? 0x000f : 0x001f;//if bits 0 and 1 are all 0, clear bit 4;
            palette[addr] = data;
            return true;
        }
    }

};//end nes

//Palette mapping:
// 0000 0000
// 0000 0001
// 0000 0010
// 0000 0011
// 0000 0100
// 0000 0101
// 0000 0110
// 0000 0111
// 0000 1000
// 0000 1001
// 0000 1010
// 0000 1011
// 0000 1100
// 0000 1101
// 0000 1110
// 0000 1111
// 0001 0000 --> 0000 0000
// 0001 0001
// 0001 0010
// 0001 0011
// 0001 0100 --> 0000 0100
// 0001 0101
// 0001 0110
// 0001 0111
// 0001 1000 --> 0000 1000
// 0001 1001
// 0001 1010
// 0001 1011
// 0001 1100 --> 0000 1100
// 0001 1101
// 0001 1110
// 0001 1111