#include "HybridBus.h"

namespace nes {
    void HybridBus::connect(Mapper *_mapp){
        mapper = _mapp;
        nt_mirror = mapper->get_nt_mirror();
    }
    
    bool HybridBus::read(Word addr, Byte &data){
        if (addr < 0x2000){
            return mapper->ppu_read(addr, data);
        }
        else if (addr < 0x3000){
            switch (nt_mirror){
            case NT_Mirror::Horizontal :
                {
                    // 0010 0000 0000 0000 -> $0000
                    // 0010 0011 1111 1111 -> $03FF
                    // 0010 0100 0000 0000 -> $0000
                    // 0010 0111 1111 1111 -> $03FF
                    // 0010 1000 0000 0000 -> $0400
                    // 0010 1011 1111 1111 -> $07FF
                    // 0010 1100 0000 0000 -> $0400
                    // 0010 1111 1111 1111 -> $07FF
                    bool coef = addr & 0x800;
                    data = vram.vals[(coef * 0x400) | (addr & 0x3ff)];
                    return true;
                }
                break;
            case NT_Mirror::Vertical :
                {
                    // 0010 0000 0000 0000 -> $0000
                    // 0010 0011 1111 1111 -> 0000 0011 1111 1111
                    // 0010 0100 0000 0000 -> 0000 0100 0000 0000
                    // 0010 0111 1111 1111 -> 0000 0111 1111 1111
                    // 0010 1000 0000 0000 -> $0000
                    // 0010 1011 1111 1111 -> 0000 0011 1111 1111
                    // 0010 1100 0000 0000 -> 0000 0100 0000 0000
                    // 0010 1111 1111 1111 -> 0000 0111 1111 1111
                    data = vram.vals[addr & 0x7ff];
                    return true;
                }
                break;
            case NT_Mirror::Single_Screen :
                {
                    data = vram.vals[addr & 0x3ff];
                    return true;
                }
                break;
            case NT_Mirror::Four_screen :
                {
                    if (addr & 0x800){
                        //high addr need to call mapper;
                        
                        //TODO: how to address cart-nested ram?
                        
                        return false;
                    }
                    else {
                        data = vram.vals[addr & 0x7ff];
                        return true;
                    }
                }
                break;
            default :
                //other NameTable Mirroring is not suppurted;
                return false;
                break;
            }
        }
        else {// addr < 0x4000;
            //assume that user wont address $3000 - $3EFF;
            data = palette.vals[addr - 0x3F00];
            return true;
        }
    }

    bool HybridBus::write(Word addr, Byte data){
        if (addr < 0x2000){
            return mapper->ppu_write(addr, data);
        }
        else if (addr < 0x3000){
            switch (nt_mirror){
            case NT_Mirror::Horizontal :
                {
                    bool coef = addr & 0x800;
                    vram.vals[(coef * 0x400) | (addr & 0x3ff)] = data;
                    return true;
                }
                break;
            case NT_Mirror::Vertical :
                {
                    vram.vals[addr & 0x7ff] = data;
                    return true;
                }
                break;
            case NT_Mirror::Single_Screen :
                {
                    vram.vals[addr & 0x3ff] = data;
                    return true;
                }
                break;
            case NT_Mirror::Four_screen :
                {
                    if (addr & 0x800){
                        //high addr need to call mapper;
                        
                        //TODO: how to address cart-nested ram?
                        
                        return false;
                    }
                    else {
                        vram.vals[addr & 0x7ff] = data;
                        return true;
                    }
                }
                break;
            default :
                //other NameTable Mirroring is not suppurted;
                return false;
                break;
            }
        }
        else {// addr < 0x4000;
            //assume that user wont address $3000 - $3EFF;
            palette.vals[addr - 0x3F00] = data;
            return true;
        }
    }
};//end nes