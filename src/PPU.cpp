#include "PPU.h"

namespace nes {
    
    PPU::PPU(){}
    
    PPU::~PPU(){}

    void PPU::connect(HybridBus *_hb_bus){
        hb_bus = _hb_bus;
    }

    bool PPU::read_regs(Word addr, Byte &data){
        switch (addr & 0x7){
            //only reg#2, 4, 7 are readable;
        case 2 : //$2002 : PPU status reg;
            //only upper 3 bits of ppu_status is effective;
            //lower 5 bits is the "noise" from ppu data buffer;
            data = (ppu_status & 0xe0) | (ppu_data & 0x1f);
            //clear vblank flag;
            ppu_status.vblank_flag = 0;
            //reset address write toggle;
            is_first_write = false;
            break;
        case 4 : //$2004 : OAM data reg;
            data = oam_data;
            break;
        case 7 : //$2007 : PPU memory data;
            //vram reading has one cycle delay;
            //however palette memory is small so no delay;
            data = ppu_data;
            this->read(vram_addr.val, ppu_data);
            if (vram_addr.val >= kPALETTE_BASE)
                data = ppu_data;
            //all read from ppu_data automatically increment vram_addr;
            vram_addr.val += (ppu_ctrl.incr_mode ? 32 : 1);
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
            ppu_ctrl = data;
            temp_addr.nt_select = ppu_ctrl.nt_select;
            break;
        case 1 : //$2001 : PPU mask;
            ppu_mask = data;
            break;
        case 3 : //$2003 : OAM address reg;
            oam_addr = data;
            break;
        case 4 : //$2004 : OAM data reg;
            oam_data = data;
            break;
        case 5 : //$2005 : screen scroll offset reg;
            ppu_scroll = data;
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
            ppu_addr = data;
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
            ppu_data = data;
            this->write(vram_addr.val, data);
            //all write to ppu_data automatically increment vram_addr;
            vram_addr.val += (ppu_ctrl.incr_mode ? 32 : 1);
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
        if (scanline < 240){//0, 1, 2, ... , 239; total=240; visible scanlines;
            update_shifters();

            //fetch operations:
            fetch_bkgr_tile();
            
            //addr operations:
            if (cycle == 256)
                vram_addr.inc_y(ppu_mask.bkgr_enable || ppu_mask.spr_enable);//visibal rendering is complete here;
            else if (cycle == 257)
                vram_addr.transfer_x(temp_addr);//reset nt_select bit for x-axis;
        }
        else if (scanline == 240){
            //post-render scanline : do nothing;
        }
        else if (scanline < 261){//241, 242, ... , 260; total = 20;
            if (scanline == 241 && cycle = 1){
                ppu_status.vblank_flag = 1;
                if (ppu_ctrl.nmi_enable)
                    nmi_out = true;//expect MainBus to find out;
            }
        }
        else {//scanline == 261 or -1; pre-render scanline;
            //fetch operations:
            fetch_bkgr_tile();
            
            //flag and addr operations:
            if (cycle == 1)
                ppu_status = 0;//effectively clear vblank, sprite0hit, and overflow flags;
            else if (cycle == 256)
                vram_addr.inc_y(ppu_mask.bkgr_enable || ppu_mask.spr_enable);
            else if (cycle == 257)
                vram_addr.transfer_x(temp_addr);//reset nt_select bit for x-axis;
            else if (cycle >= 280 && cycle <= 304)
                vram_addr.transfer_y(temp_addr);//why need transfer for so many cycles?
            else if (cycle == 339 && ppu_mask.bkgr_enable && (frame & 1 == 0) )
                ++cycle;//skip one cycle for odd frame (0,0);
        }

        if (ppu_mask.bkgr_enable){
            bkgr_pixel = bkgr_palet = 0;
            //locate the current bit;
            temp_word = 0x8000 >> fine_x;
            //withdraw the piexel info;
            bkgr_pixel = (bkgr_shifter_attr_lo & temp_word) > 0;
            bkgr_pixel |= static_cast<Byte>((bkgr_shifter_attr_hi & temp_word) > 0) << 1;
            //withdraw the palette info;
            bkgr_palet = (bkgr_shifter_patt_lo & temp_word) > 0;
            bkgr_palet |= static_cast<Byte>((bkgr_shifter_patt_hi & temp_word) > 0) << 1;
        }

        //TODO: spr_screen.SetPixel(cycle - 1, scanline, get_color(bkgr_palet, bkgr_pixel));

        ++cycle;
        if (cycle >= 341){
            cycle = 0;
            ++scanline;
            if (scanline >= 262){
                scanline = 0;
                ++frame;//one frame rendering is complete;
                frame_complete = true;
            }
        }
    }

    void PPU::reset(){
        ppu_ctrl = 0;     //$2000
        ppu_mask = 0;     //$2001
        ppu_status = 0;     //$2002
        oam_addr = 0;         //$2003
        oam_data = 0;         //$2004
        ppu_scroll = 0;       //$2005
        ppu_addr = 0;         //$2006
        ppu_data = 0;         //$2007
        
        frame = 0;
        scanline = 0;
        cycle = 0;
        
        vram_addr.val = 0;//current vram address;
        temp_addr.val = 0;//temp vram addr, or as the address of the top left onscreen tile;
        fine_x = 0;//fine x scroll; low 3 bits address 8 pixels;
        is_first_write = false;//toggle: 1 = after 1st write; 0 = after 2nd write / init;
        
        bkgr_next_id = 0;
        bkgr_next_attr = 0;
        bkgr_next_lsb = 0;
        bkgr_next_msb = 0;

        bkgr_shifter_patt_lo = 0;
        bkgr_shifter_patt_hi = 0;
        bkgr_shifter_attr_lo = 0;
        bkgr_shifter_attr_hi = 0;

        //latch the pixel infos to put onto the screen;
        bkgr_pixel = 0;
        bkgr_palet = 0;

        //tempoary variable for internal use;
        temp_word = 0;
        temp_byte = 0;
    }

    void PPU::fetch_bkgr_tile(){

        if ( (cycle >= 1 && cycle <= 256) || (cycle >= 321 && cycle <= 338) ){

            switch (cycle & 0x7){
            case 0 ://for 8th cycles; but not for the absolute 0 cycle. which is idle;
                vram_addr.inc_x(ppu_mask.bkgr_enable || ppu_mask.spr_enable);
                break;
            case 1 ://fetch name table byte;
                load_bkgr_shifters();
                read(vram_addr.get_nt_addr(), bkgr_next_id);
                break;
            case 2 ://just do nothing;
                break;
            case 3 ://fetch attribute table byte;
                read(vram_addr.get_at_addr(), bkgr_next_attr);
                //choose square from 4;
                if (vram_addr.coarse_y >= 4) bkgr_next_attr >>= 4;
                if (vram_addr.coarse_x >= 4) bkgr_next_attr >>= 2;
                bkgr_next_attr &= 0x3;
                break;
            case 4 ://just do nothing;
                break;
            case 5 ://fetch background tile low byte;
                //one pattern tile = 16B; there are 256 tiles within one pattern table;
                //there are two pattern table space, total 8KB space; need 13-bit address;
                //the MSB is used for table select; using PPU_CTRL.background_select;
                //early fetched bkgr_next_id described the order of tile;
                //bkgr_next_id * 16 is the position within a pattern table;
                //vram_addr.fine_y decides the order of byte within a tile;
                temp_word = ((Word)ppu_ctrl.bkgr_select << 12) | (bkgr_next_id << 4) | (vram_addr.fine_y);
                read(temp_word, bkgr_next_lsb);
                break;
            case 6 ://just do nothing;
                break;
            default://for 7th cycles : fetch background tile high byte;
                read(temp_word + 8, bkgr_next_msb);// <--  attetion: maybe not work;
                break;
            }

        }
        else if (cycle == 339){
            read(vram_addr.get_nt_addr(), bkgr_next_id);
        }

    }

    inline void PPU::load_bkgr_shifters(){
        bkgr_shifter_patt_lo = (bkgr_shifter_patt_lo & 0xff00) | bkgr_next_lsb;
        bkgr_shifter_patt_hi = (bkgr_shifter_patt_hi & 0xff00) | bkgr_next_msb;
        bkgr_shifter_attr_lo = (bkgr_shifter_attr_lo & 0xff00) | ((bkgr_next_attr & 0b01) ? 0xff : 0x0);
        bkgr_shifter_attr_hi = (bkgr_shifter_attr_hi & 0xff00) | ((bkgr_next_attr & 0b10) ? 0xff : 0x0);
    }

    inline void PPU::update_shifters(){
        if (ppu_mask.bkgr_enable){
            bkgr_shifter_patt_lo <<= 1;
            bkgr_shifter_patt_hi <<= 1;
            bkgr_shifter_attr_lo <<= 1;
            bkgr_shifter_attr_hi <<= 1;
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
        temp_word = n_scanl;
        temp_word -= y_coord;
        //check the ppu_ctrl reg to determine sprite height;
        temp_byte = ppu_ctrl.sprite_h ? kSPR_HEIGHT_8 : kSPR_HEIGHT_16;
        return (temp_word >= temp_byte) ? false : true;
        //>= : signed minus or unsigned greater than;
        //otherwise : within the range of sprite height;
    }
};//end nes;