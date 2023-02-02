#include "PPU.h"

namespace nes {
    
    void PPU::connect(HybridBus *_hb_bus){
        hb_bus = _hb_bus;
    }

    bool PPU::read_regs(Word addr, Byte &data){
        switch (addr & 0x7){
            //only reg#2, 4, 7 are readable;
        case 2 : //$2002 : PPU status reg;
            data = ppu_regs.ppu_status;
            is_first_write = false;//reset;
            break;
        case 4 : //$2004 : OAM data reg;
        case 7 : //$2007 : PPU memory data;
            data = ppu_regs[addr];
            break;
        default:
            //reading a "write-only" reg returns the latch's
            //current value, as do the unused bits of PPUSTATUS;
            return false;
            break;
        }
        return true;
    }

    bool PPU::write_regs(Word addr, Byte data){
        switch (addr & 0x7){
            //only for writable regs;
        case 0 : //$2000 : PPU control reg;
            ppu_regs.ppu_ctrl = data;
            temp_addr.nt_select = ppu_regs.ppu_ctrl.nt_select;
            break;
        case 1 : //$2001 : PPU mask;
        case 3 : //$2003 : OAM address reg;
        case 4 : //$2004 : OAM data reg;
            ppu_regs[addr] = data;
            break;
        case 5 : //$2005 : screen scroll offset reg;
            ppu_regs.ppu_scroll = data;
            if (is_first_write == 0){//toggle: 1 = after 1st write; 0 = after 2nd write / init;
                fine_x = data & 0x7;//low 3 bits;
                temp_addr.coarse_x = data >> 3;//high 5 bits;
            }
            else {
                temp_addr.fine_y = data & 0x7;//low 3 bits;
                temp_addr.coarse_y = data >> 3;//high 5 bits;
            }
            is_first_write = !is_first_write;
            break;
        case 6 : //$2006 : PPU address reg;
            ppu_regs.ppu_addr = data;
            if (is_first_write == 0){
                temp_addr.high_byte = data & 0x3f;//low 6 bits;
                temp_addr.msb = 0;//Bit 14 (15th bit) of temp_addr gets set to zero;
            }
            else {
                temp_addr.low_byte = data;//all 8 bits;
                vram_addr = temp_addr;//After temp_addr is updated, contents copied into vram_addr;
            }
            is_first_write = !is_first_write;
            break;
        case 7 : //$2007 : PPU data reg;
            ppu_regs.ppu_data = data;
            break;
        default:
            return false;
            break;
        }
        return true;
    }

    void PPU::clock(){

    }

    // OAM *PPU::get_oam(){
    //     return &oam;
    // }
    
    // void PPU::oam_dma(PPU &_ppu, const Byte *_ram_ptr){
    //     std::memcpy(reinterpret_cast<Byte*>(&_ppu.oam.obj_attr[0]), _ram_ptr, 0x100);
    //     return;
    // }


    bool PPU::obj_compare(Byte n_scanl, Byte y_coord){
        ppu_obj_comp = n_scanl;
        ppu_obj_comp -= y_coord;
        //check the ppu_ctrl reg to determine sprite height;
        if (ppu_regs.ppu_ctrl.H == 1)
            obj_eval_ul = kSPR_HEIGHT_8;
        else obj_eval_ul = kSPR_HEIGHT_16;
        if (ppu_obj_comp >= obj_eval_ul)
            return false;//signed minus or unsigned greater than;
        else return true;//within the range of sprite height;
    }
};//end nes;