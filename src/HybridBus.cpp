#include "HybridBus.h"

namespace nes {
    HybridBus::HybridBus(){}

    void HybridBus::connect(std::shared_ptr<Mapper> &_mapp){
        mapper = _mapp;
    }
    
    bool HybridBus::read(Word addr, Byte &data){
        if (addr < 0x2000){
            return mapper->ppu_read(addr, data);
        }
        else if (addr < 0x3000){
            data = vram[mapper->get_nt_mirror(addr)];
            return true;
        }
        else {// addr < 0x4000;
            //assume that user wont address $3000 - $3EFF;
            data = palette[addr - 0x3F00];
            return true;
        }
    }

    bool HybridBus::write(Word addr, Byte data){
        if (addr < 0x2000){
            //Note: some cartridges may contain RAM that can be written;
            return mapper->ppu_write(addr, data);
        }
        else if (addr < 0x3000){
            vram[mapper->get_nt_mirror(addr)] = data;
            return true;
        }
        else {// addr < 0x4000;
            //assume that user wont address $3000 - $3EFF;
            palette[addr - 0x3F00] = data;
            return true;
        }
    }

};//end nes