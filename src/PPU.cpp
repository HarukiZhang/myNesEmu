#include "PPU.h"

namespace nes {
    
    void PPU::connect(HybridBus *_hb_bus){
        hb_bus = _hb_bus;
    }

    bool PPU::read_regs(Word addr, Byte &data){
        switch (addr & 0x7){
            //only reg#2, 4, 7 are readable;
        case 2 : //$2002 : PPU status reg;
            //only upper 3 bits of ppu_status is effective;
            //lower 5 bits is the "noise" from ppu data buffer;
            data = (ppu_regs.ppu_status & 0xe0) | (ppu_regs.ppu_data & 0x1f);
            //clear vblank flag;
            ppu_regs.ppu_status.vblank_flag = 0;
            //reset address write toggle;
            is_first_write = false;
            break;
        case 4 : //$2004 : OAM data reg;
            data = ppu_regs.oam_data;
            break;
        case 7 : //$2007 : PPU memory data;
            //vram reading has one cycle delay;
            //however palette memory is small so no delay;
            data = ppu_regs.ppu_data;
            this->read(vram_addr.val, ppu_regs.ppu_data);
            if (vram_addr.val >= kPALETTE_BASE)
                data = ppu_regs.ppu_data;
            //all read from ppu_data automatically increment vram_addr;
            vram_addr.val += (ppu_regs.ppu_ctrl.incr_mode ? 32 : 1);
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
                vram_addr.val = temp_addr.val;//After temp_addr is updated, contents copied into vram_addr;
            }
            is_first_write = !is_first_write;
            break;
        case 7 : //$2007 : PPU data reg;
            ppu_regs.ppu_data = data;
            this->write(vram_addr.val, data);
            //all write to ppu_data automatically increment vram_addr;
            vram_addr.val += (ppu_regs.ppu_ctrl.incr_mode ? 32 : 1);
            break;
        default:
            return false;
            break;
        }
        return true;
    }

    inline bool PPU::read(Word addr, Byte &data){
        return hb_bus->read(addr, data);
    }

    inline bool PPU::write(Word addr, Byte data){
        return hb_bus->write(addr, data);
    }

    void PPU::clock(){
        if (scanline < 240){//0, 1, 2, ... , 239; total=240;
            //TODO;
        }
        else if (scanline == 240){
            //do nothing;
        }
        else if (scanline < 261){//241, 242, ... , 260; total = 20;
            if (scanline == 241 && cycle = 1){
                ppu_regs.ppu_status.vblank_flag = 1;
                if (ppu_regs.ppu_ctrl.nmi_enable)
                    nmi_out = true;//expect MainBus to find out;
            }
        }
        else {//scanline == 261 or -1;
            if (cycle == 1) ppu_regs.ppu_status.vblank_flag = 0;

            //TODO: dummy fetch for cycle 1-256;

            //TODO: if (cycle >= 321 && cycle <= 338) fetch_bkgr_tile(Word cycle);

            if (cycle == 256)
                vram_addr.incr_scroll_y(ppu_regs.ppu_mask.bkgr_enable || ppu_regs.ppu_mask.spr_enable);
            else if (cycle == 257)
                vram_addr.transfer_addr_x(temp_addr);//reset nt_select bit for x-axis;
            else if (cycle >= 280 && cycle < 305)
                vram_addr.transfer_addr_y(temp_addr);//why need transfer for so many cycles?
        }

        ++cycle;
        if (cycle >= 341){
            cycle = 0;
            ++scanline;
            if (scanline >= 262) scanline = 0;
        }
    }

    void PPU::fetch_bkgr_tile(){
        switch (cycle & 0x7){
        case 0 ://for 8th cycles; but not for the absolute 0 cycle. which is idle;
            vram_addr.incr_scroll_x(ppu_regs.ppu_mask.bkgr_enable || ppu_regs.ppu_mask.spr_enable);
            break;
        case 1 ://fetch name table byte;
            break;
        case 2 ://just do nothing;
            break;
        case 3 ://fetch attribute table byte;
            break;
        case 4 ://just do nothing;
            break;
        case 5 ://fetch background tile low byte;
            break;
        case 6 ://just do nothing;
            break;
        default://for 7th cycles : fetch background tile high byte;
            break;
        }
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