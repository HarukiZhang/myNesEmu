#include "HybridBus.h"

namespace nes {
    void HybridBus::connect(std::shared_ptr<Mapper> &_mapp){
        mapper = _mapp;
        switch (mapper->get_nt_mirror()){
            case NT_Mirror::Horizontal :
                nt_mirror = HybridBus::mirror_horizontal;
                break;
            case NT_Mirror::Vertical :
                nt_mirror = HybridBus::mirror_vertical;
                break;
            case NT_Mirror::Single_Screen :
                nt_mirror = HybridBus::mirror_single_screen;
                break;
            case NT_Mirror::Four_screen :
                nt_mirror = HybridBus::mirror_four_screen;
                break;
            default: break;
        }
    }
    
    bool HybridBus::read(Word addr, Byte &data){
        if (addr < 0x2000){
            return mapper->ppu_read(addr, data);
        }
        else if (addr < 0x3000){
            data = (this->*nt_mirror)(addr);
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
            (this->*nt_mirror)(addr) = data;
            return true;
        }
        else {// addr < 0x4000;
            //assume that user wont address $3000 - $3EFF;
            palette[addr - 0x3F00] = data;
            return true;
        }
    }

    //mirror functions =========================================================
    Byte &HybridBus::mirror_horizontal(Word addr){
        // 0010 0'000 0000 0000 -> $0000
        // 0010 0'011 1111 1111 -> $03FF
        // 0010 0'100 0000 0000 -> $0000
        // 0010 0'111 1111 1111 -> $03FF
        // 0010 1'000 0000 0000 -> $0400
        // 0010 1'011 1111 1111 -> $07FF
        // 0010 1'100 0000 0000 -> $0400
        // 0010 1'111 1111 1111 -> $07FF
        Word mask = (addr & 0x800) >> 1;//mask=0x400 if bit 11 is set, otherwise 0x0;
        return vram[(addr & 0x3ff) | mask];
    }

    Byte &HybridBus::mirror_vertical(Word addr){
        // 0010 0000 0000 0000 -> $0000
        // 0010 0011 1111 1111 -> 0000 0011 1111 1111
        // 0010 0100 0000 0000 -> 0000 0100 0000 0000
        // 0010 0111 1111 1111 -> 0000 0111 1111 1111
        // 0010 1000 0000 0000 -> $0000
        // 0010 1011 1111 1111 -> 0000 0011 1111 1111
        // 0010 1100 0000 0000 -> 0000 0100 0000 0000
        // 0010 1111 1111 1111 -> 0000 0111 1111 1111
        return vram[addr & 0x7ff];
    }

    Byte &HybridBus::mirror_single_screen(Word addr){
        return vram[addr & 0x3ff];
    }

    Byte &HybridBus::mirror_four_screen(Word addr){
        if (addr & 0x800){
            //high addr need to call mapper;
            //TODO: how to address cart-nested ram?
            return vram[addr & 0x7ff];//  <--- this is wrong now;
        }
        else
            return vram[addr & 0x7ff];
    }

};//end nes