#include "PPU.h"

#define S_MODE

namespace nes {
    
    PPU::PPU(){
        pal_screen[0x00] = olc::Pixel(84, 84, 84);
        pal_screen[0x01] = olc::Pixel(0, 30, 116);
        pal_screen[0x02] = olc::Pixel(8, 16, 144);
        pal_screen[0x03] = olc::Pixel(48, 0, 136);
        pal_screen[0x04] = olc::Pixel(68, 0, 100);
        pal_screen[0x05] = olc::Pixel(92, 0, 48);
        pal_screen[0x06] = olc::Pixel(84, 4, 0);
        pal_screen[0x07] = olc::Pixel(60, 24, 0);
        pal_screen[0x08] = olc::Pixel(32, 42, 0);
        pal_screen[0x09] = olc::Pixel(8, 58, 0);
        pal_screen[0x0A] = olc::Pixel(0, 64, 0);
        pal_screen[0x0B] = olc::Pixel(0, 60, 0);
        pal_screen[0x0C] = olc::Pixel(0, 50, 60);
        pal_screen[0x0D] = olc::Pixel(0, 0, 0);
        pal_screen[0x0E] = olc::Pixel(0, 0, 0);
        pal_screen[0x0F] = olc::Pixel(0, 0, 0);

        pal_screen[0x10] = olc::Pixel(152, 150, 152);
        pal_screen[0x11] = olc::Pixel(8, 76, 196);
        pal_screen[0x12] = olc::Pixel(48, 50, 236);
        pal_screen[0x13] = olc::Pixel(92, 30, 228);
        pal_screen[0x14] = olc::Pixel(136, 20, 176);
        pal_screen[0x15] = olc::Pixel(160, 20, 100);
        pal_screen[0x16] = olc::Pixel(152, 34, 32);
        pal_screen[0x17] = olc::Pixel(120, 60, 0);
        pal_screen[0x18] = olc::Pixel(84, 90, 0);
        pal_screen[0x19] = olc::Pixel(40, 114, 0);
        pal_screen[0x1A] = olc::Pixel(8, 124, 0);
        pal_screen[0x1B] = olc::Pixel(0, 118, 40);
        pal_screen[0x1C] = olc::Pixel(0, 102, 120);
        pal_screen[0x1D] = olc::Pixel(0, 0, 0);
        pal_screen[0x1E] = olc::Pixel(0, 0, 0);
        pal_screen[0x1F] = olc::Pixel(0, 0, 0);

        pal_screen[0x20] = olc::Pixel(236, 238, 236);
        pal_screen[0x21] = olc::Pixel(76, 154, 236);
        pal_screen[0x22] = olc::Pixel(120, 124, 236);
        pal_screen[0x23] = olc::Pixel(176, 98, 236);
        pal_screen[0x24] = olc::Pixel(228, 84, 236);
        pal_screen[0x25] = olc::Pixel(236, 88, 180);
        pal_screen[0x26] = olc::Pixel(236, 106, 100);
        pal_screen[0x27] = olc::Pixel(212, 136, 32);
        pal_screen[0x28] = olc::Pixel(160, 170, 0);
        pal_screen[0x29] = olc::Pixel(116, 196, 0);
        pal_screen[0x2A] = olc::Pixel(76, 208, 32);
        pal_screen[0x2B] = olc::Pixel(56, 204, 108);
        pal_screen[0x2C] = olc::Pixel(56, 180, 204);
        pal_screen[0x2D] = olc::Pixel(60, 60, 60);
        pal_screen[0x2E] = olc::Pixel(0, 0, 0);
        pal_screen[0x2F] = olc::Pixel(0, 0, 0);

        pal_screen[0x30] = olc::Pixel(236, 238, 236);
        pal_screen[0x31] = olc::Pixel(168, 204, 236);
        pal_screen[0x32] = olc::Pixel(188, 188, 236);
        pal_screen[0x33] = olc::Pixel(212, 178, 236);
        pal_screen[0x34] = olc::Pixel(236, 174, 236);
        pal_screen[0x35] = olc::Pixel(236, 174, 212);
        pal_screen[0x36] = olc::Pixel(236, 180, 176);
        pal_screen[0x37] = olc::Pixel(228, 196, 144);
        pal_screen[0x38] = olc::Pixel(204, 210, 120);
        pal_screen[0x39] = olc::Pixel(180, 222, 120);
        pal_screen[0x3A] = olc::Pixel(168, 226, 144);
        pal_screen[0x3B] = olc::Pixel(152, 226, 180);
        pal_screen[0x3C] = olc::Pixel(160, 214, 228);
        pal_screen[0x3D] = olc::Pixel(160, 162, 160);
        pal_screen[0x3E] = olc::Pixel(0, 0, 0);
        pal_screen[0x3F] = olc::Pixel(0, 0, 0);
    }
    
    PPU::~PPU(){}

    void PPU::clock() {
        //     |Idle|BG Fetch |SPR Fetch  |F2 Fetch   |U Fetch    |
        //     | 0  |1 ... 256|257 ... 320|321 ... 336|337 ... 340|
        // |   |_____________
        // |  0| pre-render
        // |   |_____________
        // |  1|
        // |...| visibal
        // |240|_____________
        // |241| post-render
        // |   |_____________
        // |242|
        // |...| vblank
        // |261|_____________
        //


        // "If either of bits 3 or 4 is enabled, at any time outside of the vblank interval the PPU will be making continual use 
        // to the PPU address and data bus to fetch tiles to render, as well as internally fetching sprite data from the OAM."
        //
        // "Disabling rendering (clear both bits 3 and 4) during a visible part of the frame can be problematic. 
        // It can cause a corruption of the sprite state, which will display incorrect sprite data on the next frame.
        // It is, however, perfectly fine to mask sprites but leave the background on (set bit 3, clear bit 4) at any time in the frame."
        // 
        // "Sprite 0 hit does not trigger in any area where the background or sprites are hidden."

        if (scanline < 241) {//scanlines that need to fetch;
#ifdef S_MODE
            Byte opcode = 0;
            if (scanline == 0) {
                opcode = opcode_mtx[0][cycle];
                (this->*micop_mtx[opcode])();
            }
            else {
                opcode = opcode_mtx[1][cycle];
                (this->*micop_mtx[opcode])();
            }
#else
            if (cycle > 0) {//non-idle cycles;

                //no matter whether render enabled;
                if (scanline == 0 && cycle == 1) {
                    ppu_status = 0;//effectively clear vblank_flag, sprite_hit, and sprite_overflow;
                    nmi_out = false;
                    for (Byte i = 0; i < 8; ++i) {
                        sprt_shifters_lo[i] = 0;
                        sprt_shifters_hi[i] = 0;
                        sprt_x_counters[i] = 0xff;
                        sprt_attr_latches[i] = 0;
                    }
                }

                if (check_render_enabled()) {
                    if (scanline == 0) {//pre-render scanline;
                    //"Seconday OAM clear and Sprite evaluation do not occur on the pre-render scanline. Sprite fetches still do."
                        if (cycle <= 256) {
                            fetch_bkgr_tile();
                            update_bkgr_shifters();
                        }
                        else if (cycle <= 320) {
                            fetch_sprt_tile();
                            if (cycle >= 280 && cycle <= 304) {
                                vram_addr.reset_vert(temp_addr);
                            }
                        }
                        else if (cycle <= 336) {
                            fetch_bkgr_tile();
                            update_bkgr_shifters();
                        }


                    }
                    else {//scanline 1 ~ 240 : visible scanlines
                        if (cycle <= 256) {
                            if (cycle <= 64) {
                                clear_sec_oam();
                            }
                            else {
                                if (cycle == 65) sp0_pres_nl = false;
                                eval_sprite();
                            }
                            fetch_bkgr_tile();
                            render_pixel();
                            update_bkgr_shifters();
                            update_sprt_shifters();
                        }
                        else if (cycle <= 320) {
                            fetch_sprt_tile();
                        }
                        else if (cycle <= 336) {
                            fetch_bkgr_tile();
                            update_bkgr_shifters();
                        }

                    }

                    //addr operations:
                    if (cycle == 256) {//visible rendering is complete here;
                        vram_addr.inc_vert();
                        eval_state = eval_idx = eval_off = 0;
                        sprt_num = soam_idx;
                        soam_idx = 0;
                        sprt_fetch_idx = 0;
                        sp0_present = false;
                        sp0_present = sp0_pres_nl;
                        for (Byte i = 0; i < 8; ++i) {
                            sprt_shifters_lo[i] = 0;
                            sprt_shifters_hi[i] = 0;
                            sprt_x_counters[i] = 0xff;
                            sprt_attr_latches[i] = 0;
                        }
                    }
                    else if (cycle == 257) {
                        load_bkgr_shifters();
                        vram_addr.reset_hori(temp_addr);//reset nt_select bit for x-axis;
                        if (scroll_updated_while_rendering) {
                            scroll_updated_while_rendering = false;
                            fine_x = temp_fine_x;
                        }
                    }
                    else if (cycle == 337) {
                        load_bkgr_shifters();
                        read(vram_addr.get_nt_addr(), bkgr_next_id);
                    }
                    else if (cycle == 339) {
                        read(vram_addr.get_nt_addr(), bkgr_next_id);
                        if (scanline == 0)
                            ++cycle;
                    }
                }

            }
            //cycle == 0 : idle;
#endif
        }
        else if (scanline == 241 && cycle == 1) {
            ppu_status.vblank_flag = 1;
            if (ppu_ctrl.nmi_enable) nmi_out = true;//expect MainBus to find out;
        }

        ++cycle;
        if (cycle > 340) {
            cycle = 0;
            ++scanline;
            if (scanline > 261) {
                scanline = 0;
                ++frame;
                frame_complete = true;//one frame rendering is complete;
            }
        }


    }

    inline void PPU::clear_sec_oam() {
        //"Secondary OAM (32-byte buffer for current sprites on scanline) is initialized to $FF"
        sec_oam[cycle & 0x1f] = 0xFF;
    }

    void PPU::eval_sprite() {
        //evaluating sprites period has 3 * 64 = 192 cycles;
        if (eval_idx == 0x40) return;
        if ((cycle & 0x1) == 0) return;//even cycle no op;
#ifdef S_MODE

        eval_data = oam.ent[eval_idx & 0x3f][eval_off & 0x3];

        switch (eval_state) {
        case 0:

            if (soam_idx < 8) {

                if (check_in_range(eval_data)) {
                    //find that sprite#0 has been in range;
                    if (eval_idx == 0) sp0_pres_nl = true;//inform the next scanline;
        case 1:
                    eval_state = 1;
                    sec_oam.ent[soam_idx][eval_off & 0x3] = eval_data;
                    ++eval_off;
                    if (eval_off >= 4) {
                        ++soam_idx;
                        ++eval_idx;
                        eval_off = 0;
                        eval_state = 0;
                    }
                    break;
                }
                else {
                    ++eval_idx;
                    eval_off = 0;
                    eval_state = 0;
                }
                break;
            }
            else {
        case 2:
                eval_state = 2;
                if (check_in_range(eval_data)) {
                    ppu_status.sprite_overflow = 1;
                }
                else {
                    ++eval_idx;
                    ++eval_off;
                }
            }
            break;
        default:
            std::clog << "eval_sprite: swtich error" << std::endl;
            break;
        }
#else

        if (soam_idx < 8) {
            sec_oam.ent[soam_idx][0] = oam.ent[eval_idx & 0x3f][0];
            if (check_in_range(oam.ent[eval_idx & 0x3f][0])) {

                if (eval_idx == 0) sp0_pres_nl = true;//inform the next scanline;
                
                for (eval_off = 1; eval_off < 4; ++eval_off) {
                    sec_oam.ent[soam_idx][eval_off & 0x3] = oam.ent[eval_idx & 0x3f][eval_off & 0x3];
                }
                ++soam_idx;
            }
            ++eval_idx;
            eval_off = 0;
        }
        else {
            //"The first such check correctly checks the y coordinate of the next OAM entry, but after that the logic breaks 
            //  and starts scanning OAM "diagonally", evaluating the tile number/attributes/X-coordinates of subsequent 
            //  OAM entries as Y-coordinates (due to incorrectly incrementing m when moving to the next sprite). 
            //  This results in inconsistent sprite overflow behavior showing both false positives and false negatives."
            if (check_in_range(oam.ent[eval_idx & 0x3f][eval_off & 0x3])) {
                ppu_status.sprite_overflow = 1;
            }
            else {
                ++eval_idx;
                ++eval_off;
            }
        }
#endif
    }

    //void PPU::fetch_tile() {
    //    if (cycle < 339) {
    //        switch (cycle & 0x7) {
    //        case 0://8th cycle
    //            if (!sprite_fetch_enable) {
    //                vram_addr.inc_hori();
    //            }
    //            break;
    //        case 1://fetch name table byte;
    //            if (!sprite_fetch_enable) {
    //                load_bkgr_shifters();
    //                read(vram_addr.get_nt_addr(), bkgr_next_id);
    //            }
    //            break;
    //        case 2:
    //            break;
    //        case 3://fetch attribute table byte;
    //            if (!sprite_fetch_enable) {
    //                read(vram_addr.get_at_addr(), bkgr_next_attr);
    //                //choose square from 4;
    //                if ((vram_addr.coarse_y & 0x3) > 1) bkgr_next_attr >>= 4;
    //                if ((vram_addr.coarse_x & 0x3) > 1) bkgr_next_attr >>= 2;
    //                bkgr_next_attr &= 0x3;
    //            }
    //            break;
    //        case 4:
    //            break;
    //        case 5://fetch low byte;
    //            if (sprite_fetch_enable) {
    //                //TODO
    //            }
    //            else {
    //                bkgr_addr = get_bkgr_patt_addr();
    //                read(bkgr_addr, bkgr_next_lsb);
    //            }
    //            break;
    //        case 6:
    //            break;
    //        case 7://fetch high byte;
    //            if (sprite_fetch_enable) {
    //                //TODO
    //            }
    //            else {
    //                bkgr_addr |= 0x0008;//set plane bit to access high byte;
    //                read(bkgr_addr, bkgr_next_msb);
    //            }
    //            break;
    //        default:
    //            break;
    //        }
    //    }
    //    else if (cycle == 339)
    //        read(vram_addr.get_nt_addr(), bkgr_next_id);
    //
    //}

    void PPU::fetch_bkgr_tile() {
        switch (cycle & 0x7) {
        case 0://for 8th cycles; but not for the absolute 0 cycle. which is idle;
            vram_addr.inc_hori();
            break;
        case 1://fetch name table byte;
            load_bkgr_shifters();
            read(vram_addr.get_nt_addr(), bkgr_next_id);
            break;
        case 2://just do nothing;
            break;
        case 3://fetch attribute table byte;
            read(vram_addr.get_at_addr(), bkgr_next_attr);
            //choose square from 4;
            if ((vram_addr.coarse_y & 0x3) > 1) bkgr_next_attr >>= 4;
            if ((vram_addr.coarse_x & 0x3) > 1) bkgr_next_attr >>= 2;
            bkgr_next_attr &= 0x3;
            break;
        case 4://just do nothing;
            break;
        case 5://fetch background tile low byte;
            bkgr_addr = get_bkgr_patt_addr();
            read(bkgr_addr, bkgr_next_lsb);
            break;
        case 6://just do nothing;
            break;
        default://for 7th cycles : fetch background tile high byte;
            bkgr_addr |= 0x0008;//set plane bit to access high byte;
            read(bkgr_addr, bkgr_next_msb);
            break;
        }
    }

    void PPU::fetch_nt_tile(){
        read(vram_addr.get_nt_addr(), bkgr_next_id);
    }
    void PPU::fetch_at_tile(){
        read(vram_addr.get_at_addr(), bkgr_next_attr);
        //choose square from 4;
        if ((vram_addr.coarse_y & 0x3) > 1) bkgr_next_attr >>= 4;
        if ((vram_addr.coarse_x & 0x3) > 1) bkgr_next_attr >>= 2;
        bkgr_next_attr &= 0x3;
    }
    void PPU::fetch_bg_tile(){
        if (!fetch_toggle) {
            bkgr_addr = get_bkgr_patt_addr();
            read(bkgr_addr, bkgr_next_lsb);
        }
        else {
            bkgr_addr |= 0x0008;//set plane bit to access high byte;
            read(bkgr_addr, bkgr_next_msb);
        }
        fetch_toggle = !fetch_toggle;
    }
    void PPU::fetch_sp_tile(){
        auto flip_hori = [](Byte b) {
            b = (b & 0xf0) >> 4 | (b & 0x0f) << 4;
            b = (b & 0xcc) >> 2 | (b & 0x33) << 2;
            b = (b & 0xaa) >> 1 | (b & 0x55) << 1;
            return b;
        };

        if (sprt_fetch_idx < sprt_num) {
            if (!fetch_toggle) {
                sprt_addr = get_sprt_addr();
                read(sprt_addr, sprt_shifters_lo[sprt_fetch_idx]);
                if (sec_oam.ent[sprt_fetch_idx].flip_h) {
                    sprt_shifters_lo[sprt_fetch_idx] = flip_hori(sprt_shifters_lo[sprt_fetch_idx]);
                }
                //copy x_coord to x_counter;
                sprt_x_counters[sprt_fetch_idx] = sec_oam.ent[sprt_fetch_idx].x_coord;
            }
            else {
                sprt_addr |= 0x0008;
                read(sprt_addr, sprt_shifters_hi[sprt_fetch_idx]);
                if (sec_oam.ent[sprt_fetch_idx].flip_h) {
                    sprt_shifters_hi[sprt_fetch_idx] = flip_hori(sprt_shifters_hi[sprt_fetch_idx]);
                }
                //copy attribute byte to sprt_attr_latches;
                sprt_attr_latches[sprt_fetch_idx] = sec_oam.ent[sprt_fetch_idx][2];
                ++sprt_fetch_idx;
            }
            fetch_toggle = !fetch_toggle;
        }
    }

    void PPU::fetch_sprt_tile() {
        auto flip_hori = [](Byte b) {
            b = (b & 0xf0) >> 4 | (b & 0x0f) << 4;
            b = (b & 0xcc) >> 2 | (b & 0x33) << 2;
            b = (b & 0xaa) >> 1 | (b & 0x55) << 1;
            return b;
        };

        switch (cycle & 0x7) {
        case 1:
            read(vram_addr.get_nt_addr(), bkgr_next_id);
            break;
        case 3:
            read(vram_addr.get_at_addr(), bkgr_next_attr);
            break;
        case 5:
            if (sprt_fetch_idx < sprt_num) {
                sprt_addr = get_sprt_addr();
                read(sprt_addr, sprt_shifters_lo[sprt_fetch_idx]);
                if (sec_oam.ent[sprt_fetch_idx].flip_h) {
                    sprt_shifters_lo[sprt_fetch_idx] = flip_hori(sprt_shifters_lo[sprt_fetch_idx]);
                }
                //copy x_coord to x_counter;
                sprt_x_counters[sprt_fetch_idx] = sec_oam.ent[sprt_fetch_idx].x_coord;
            }
            break;
        case 7:
            if (sprt_fetch_idx < sprt_num) {
                sprt_addr |= 0x0008;
                read(sprt_addr, sprt_shifters_hi[sprt_fetch_idx]);
                if (sec_oam.ent[sprt_fetch_idx].flip_h) {
                    sprt_shifters_hi[sprt_fetch_idx] = flip_hori(sprt_shifters_hi[sprt_fetch_idx]);
                }
                //copy attribute byte to sprt_attr_latches;
                sprt_attr_latches[sprt_fetch_idx] = sec_oam.ent[sprt_fetch_idx][2];
                ++sprt_fetch_idx;
            }
            break;
        }
        return;
    }

    inline Word PPU::get_sprt_addr() {
        OBJ_ATTR& r_oa = sec_oam.ent[sprt_fetch_idx];
        if (!ppu_ctrl.sprite_h) {//size 8*8
            if (!r_oa.flip_v) {//not flipped vertically;
                return ((Word)ppu_ctrl.spr_select << 12) 
                    | ((Word)r_oa.index << 4) | ((Word)(scanline - r_oa.y_coord) & 0x0007);
            }
            else {//flipped vertically;
                return ((Word)ppu_ctrl.spr_select << 12) 
                    | ((Word)r_oa.index << 4) | ((Word)(7 - (scanline - r_oa.y_coord)) & 0x0007);
            }
        }
        else {//size 8*16
            //"For 8x16 sprites, the PPU ignores the pattern table selection and selects a pattern table from bit 0 of this number."
            if (!sec_oam.ent[sprt_fetch_idx].flip_v) {
                if (scanline - r_oa.y_coord < 8) {//the pattern byte is at the top half;
                    return (((Word)r_oa.index & 0x1) << 12) 
                        | (((Word)r_oa.index & 0xfe) << 4) | ((Word)(scanline - r_oa.y_coord) & 0x0007);
                }
                else {//at bottom half;
                    return (((Word)r_oa.index & 0x1) << 12) 
                        | ((((Word)r_oa.index & 0xfe) + 1) << 4) | ((Word)(scanline - r_oa.y_coord) & 0x0007);
                }
            }
            else {
                if (scanline - r_oa.y_coord < 8) {//the pattern byte is at the upper half;
                    return (((Word)r_oa.index & 0x1) << 12) 
                        | ((((Word)r_oa.index & 0xfe) + 1) << 4) | ((Word)(7 - (scanline - r_oa.y_coord)) & 0x0007);
                }
                else {//at bottom half;
                    return (((Word)r_oa.index & 0x1) << 12) 
                        | (((Word)r_oa.index & 0xfe) << 4) | ((Word)(7 - (scanline - r_oa.y_coord)) & 0x0007);
                }
            }
        }
    }


    inline Word PPU::get_bkgr_patt_addr() {
        //one pattern tile = 16B; there are 256 tiles within one pattern table;
        //there are two pattern table space, total 8KB space; need 13-bit address;
        //the MSB is used for table select; using PPU_CTRL.background_select;
        //early fetched bkgr_next_id described the order of tile;
        //bkgr_next_id * 16 is the position within a pattern table;
        //vram_addr.fine_y decides the order of byte within a tile;
        //
        // FEDC BA98 7654 3210
        // -------------------
        // 000H RRRR CCCC PTTT
        //   || |||| |||| |+++-T : Fine Y offset, the row number within a tile
        //   || |||| |||| +----P : Bit plane(0: "lower"; 1: "upper")
        //   || |||| ++++------C : Tile column
        //   || ++++-----------R : Tile row
        //   |+----------------H : Half of pattern table(0: "left"; 1: "right")
        //   +-----------------0 : Pattern table is at $0000 - $1FFF
        return (((Word)ppu_ctrl.bkgr_select << 12) | ((Word)bkgr_next_id << 4) | vram_addr.fine_y) & 0xfff7;//ensure the plane bit is 0;
    }

    inline void PPU::render_pixel() {

        bkgr_pixel = bkgr_attrb = 0;
        if (ppu_mask.bkgr_enable) {
            //locate the current bit;
            extract_mask = 0x8000 >> fine_x;
            //withdraw the pixel info;
            bkgr_pixel = (bkgr_shifter_patt_lo & extract_mask) > 0;
            bkgr_pixel |= static_cast<Byte>((bkgr_shifter_patt_hi & extract_mask) > 0) << 1;
            //withdraw the palette info;
            bkgr_attrb = (bkgr_shifter_attr_lo & extract_mask) > 0;
            bkgr_attrb |= static_cast<Byte>((bkgr_shifter_attr_hi & extract_mask) > 0) << 1;
        }

        sprt_pixel = sprt_attrb = 0;
        first_being_rendered = false;
        if (ppu_mask.spr_enable) {
            for (Byte i = 0; i < sprt_num; ++i) {
                if (sprt_x_counters[i] == 0) {
                    sprt_pixel = (sprt_shifters_lo[i] & 0x80) > 0;
                    sprt_pixel |= static_cast<Byte>((sprt_shifters_hi[i] & 0x80) > 0) << 1;
                    sprt_attrb = (sprt_attr_latches[i] & 0x3) + 0x04;//point to sprite palettes;
                    sprt_priority = sprt_attr_latches[i] & 0x20;//0: in front of background; 1: behind background
                    if (sprt_pixel != 0) {//if the sprite is not transparent;

                        //if sp0 is in the sec oam, then it must be put at the first place in sec oam;
                        
                        if (i == 0) {//if the first sprite in sec oam is being rendered;
                            
                            first_being_rendered = true;

                            if (cycle < 256 && (bkgr_pixel > 0) && sp0_present && ppu_mask.bkgr_enable) {
                                if (cycle <= 8) {
                                    if (ppu_mask.bkgr_col_enable && ppu_mask.spr_col_enable)
                                        ppu_status.sprite_hit = 1;
                                }
                                else {//cycle : 9 ~ 255 : x = 8 ~ 254 (x starts from 0)
                                    ppu_status.sprite_hit = 1;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }

        //if (cycle < 256) {//not detect at x == 255;
        //    if (bkgr_pixel && sprt_pixel) {//if both are opaque;

        //        if (sp0_present && first_being_rendered) {//if sp0 is being rendered;

        //            if (ppu_mask.bkgr_enable && ppu_mask.spr_enable) {//only when both BG and SP are visible;
        //                
        //                if (ppu_mask.bkgr_col_enable && ppu_mask.spr_col_enable)
        //                    ppu_status.sprite_hit = 1;
        //                else if (cycle >= 9)
        //                    ppu_status.sprite_hit = 1;
        //            }
        //        }
        //    }
        //}

        if (cycle <= 8) {//visible left-most 8 pixels;
            if (!ppu_mask.bkgr_col_enable) {
                bkgr_pixel = 0;
            }
            if (!ppu_mask.spr_col_enable) {
                sprt_pixel = 0;
            }
        }

        //Priority multiplexer decision table
        //====================================================
        //BG pixel	Sprite pixel	Priority	    Output
        // 0	        0	            X	        BG ($3F00)
        // 0	        1-3	            X	        Sprite
        // 1-3	        0	            X	        BG
        // 1-3	        1-3	            1	        BG
        // 1-3	        1-3	            0	        Sprite

        if (bkgr_pixel == 0) {
            if (sprt_pixel == 0) {
                output_pixel = output_attrb = 0;
            }
            else {
                output_pixel = sprt_pixel;
                output_attrb = sprt_attrb;
            }
        }
        else {//if BG is visible
            if (sprt_pixel == 0) {
                output_pixel = bkgr_pixel;
                output_attrb = bkgr_attrb;
            }
            else {//and if SP is also visible
                if (sprt_priority) {
                    output_pixel = bkgr_pixel;
                    output_attrb = bkgr_attrb;
                }
                else {
                    output_pixel = sprt_pixel;
                    output_attrb = sprt_attrb;
                }
            }
        }

        //only need to set pixel within visible scanlines * background rendering cycles;
        //Because visible field is (1,1) to (256, 240), so need to map it a little bit;
        spr_screen.SetPixel(cycle - 1, scanline - 1, get_color(output_attrb, output_pixel));


        //debug:
        bkgr_screen.SetPixel(cycle - 1, scanline - 1, get_color(bkgr_attrb, bkgr_pixel));

        return;
    }

    void PPU::connect(std::shared_ptr<Mapper>& mapp){
        mapper = mapp;
    }

    bool PPU::read_regs(Word addr, Byte &data){
        switch (addr & 0x7){
            //only reg#2, 4, 7 are readable;
        case 2 : //$2002 : PPU status reg;
            //only upper 3 bits of ppu_status is effective;
            //lower 5 bits is the "noise" from ppu data buffer; actually it's PPU open bus;
            data = (ppu_status & 0xe0) | (ppu_data & 0x1f); //"Read PPUSTATUS: Return old status of NMI_occurred in bit 7,
            ppu_status.vblank_flag = 0;                     //  then set NMI_occurred to false."
            nmi_out = false;//immediately pull back nmi_out;
            //reset address write toggle;
            is_first_write = false;
            break;
        case 4 : //$2004 : OAM data reg;
            data = oam[oam_addr];
            break;
        case 7 : //$2007 : PPU memory data;
            //vram reading has one cycle delay;
            //however palette memory is small so no delay;
            data = ppu_data;                     //output buffer;
            this->read(vram_addr.val, ppu_data); //update buffer;
            if (vram_addr.val >= kPALETTE_BASE)  //palette do not delay;
                data = ppu_data;
            //all read from ppu_data automatically increment vram_addr;
            vram_addr.val += addr_increment;
            break;
        default:
            //reading a "write-only" reg returns the latch's
            //current value, as do the unused bits of PPUSTATUS;
            data = NULL;//temporarily;
            return false;
            break;
        }
        return true;
    }

    bool PPU::write_regs(Word addr, Byte data){
        switch (addr & 0x7){
            //only for writable regs;
        case 0 : //$2000 : PPU control reg;
            ppu_ctrl = data;//"Write to PPUCTRL: Set NMI_output to bit 7." --nesdev;
            addr_increment = ppu_ctrl.incr_mode ? 32 : 1;
            temp_addr.nt_sel_x = ppu_ctrl.nt_select & 0x1;
            temp_addr.nt_sel_y = (ppu_ctrl.nt_select & 0x2) > 0;
            //sprite size : 0: 8x8 pixels; 1: 8x16 pixels
            sprite_size = ppu_ctrl.sprite_h ? 0x10 : 0x8;
            //"If the PPU is currently in vertical blank, and the PPUSTATUS ($2002) vblank flag is still set (1), 
            // changing the NMI flag in bit 7 of $2000 from 0 to 1 will immediately generate an NMI."
            if (!ppu_ctrl.nmi_enable)
                nmi_out = false;
            else if (ppu_status.vblank_flag)//implicitly AND (ppu_ctrl.nmi_enable == 1);
                nmi_out = true;
            break;
        case 1 : //$2001 : PPU mask;
            ppu_mask = data;
            greyscale_mask = ppu_mask.greyscale ? 0x30 : 0xff;
            break;
        case 3 : //$2003 : OAM address reg;
            oam_addr = data;
            break;
        case 4 : //$2004 : OAM data reg;
            oam[oam_addr] = data;
            break;
        case 5 : //$2005 : screen scroll offset reg;
            ppu_scroll = data;
            if (is_first_write == 0){//toggle: 1 = after 1st write; 0 = after 2nd write / init;
                if (scanline <= 240 && cycle <= 256) {
                    scroll_updated_while_rendering = true;
                    temp_fine_x = data & 0x7;
                }
                else {
                    fine_x = data & 0x7;//low 3 bits;
                }
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
            vram_addr.val += addr_increment;
            break;
        default:
            return false;
            break;
        }
        return true;
    }

    inline bool PPU::read(Word addr, Byte &data){
        if (addr >= 0x3F00) {//$3F00 - $3FFF;  map to $3F00 - $3F1F; Palette;
            addr &= ((addr & 0x0003) == 0) ? 0x000f : 0x001f;//if bits 0 and 1 are all 0, clear bit 4;
            data = palette[addr];
            //This is implemented as a bitwise AND with $30 on any value read from PPU $3F00-$3FFF, both on the display and through PPUDATA.
            data &= greyscale_mask;
            return true;
        }
        else if (addr >= 0x2000) {//$2000 - $3EFF : Name Table / Attribute Table;
            //map $3XXX to $2XXX;
            data = vram[mapper->get_nt_mirror(addr & 0x2fff)];
            return true;
        }
        else {//$0000 - $1FFF : Pattern Table;
            return mapper->ppu_read(addr, data);
        }
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
    }

    inline bool PPU::write(Word addr, Byte data){
        if (addr < 0x2000) {
            //Note: some cartridges may contain RAM that can be written;
            return mapper->ppu_write(addr, data);
        }
        else if (addr < 0x3F00) {//$2000 - $3EFF;
            vram[mapper->get_nt_mirror(addr & 0x2fff)] = data;
            return true;
        }
        else {//$3F00 - $3FFF;
            addr &= ((addr & 0x0003) == 0) ? 0x000f : 0x001f;//if bits 0 and 1 are all 0, clear bit 4;
            palette[addr] = data;
            return true;
        }
    }

    inline void PPU::load_bkgr_shifters(){
        bkgr_shifter_patt_lo = (bkgr_shifter_patt_lo & 0xff00) | bkgr_next_lsb;
        bkgr_shifter_patt_hi = (bkgr_shifter_patt_hi & 0xff00) | bkgr_next_msb;
        bkgr_shifter_attr_lo = (bkgr_shifter_attr_lo & 0xff00) | ((bkgr_next_attr & 0b01) ? 0xff : 0x0);
        bkgr_shifter_attr_hi = (bkgr_shifter_attr_hi & 0xff00) | ((bkgr_next_attr & 0b10) ? 0xff : 0x0);
    }

    inline void PPU::update_bkgr_shifters(){
        //should only perform while ppu_mask.bkgr_enable == true;
        bkgr_shifter_patt_lo <<= 1;
        bkgr_shifter_patt_hi <<= 1;
        bkgr_shifter_attr_lo <<= 1;
        bkgr_shifter_attr_hi <<= 1;
    }

    inline void PPU::update_sprt_shifters() {
        if (ppu_mask.spr_enable) {
            for (Byte i = 0; i < sprt_num; ++i) {
                if (sprt_x_counters[i] > 0) {
                    --sprt_x_counters[i];
                }
                else {
                    sprt_shifters_lo[i] <<= 1;
                    sprt_shifters_hi[i] <<= 1;
                }
            }
        }
    }

    olc::Sprite& PPU::get_screen() {
        return spr_screen;
    }

    olc::Sprite& PPU::get_bkgr() {
        return bkgr_screen;
    }

    olc::Sprite& PPU::get_name_table(Word sel) {

        for (Word nt_y = 0; nt_y < 30; ++nt_y) {
            for (Word nt_x = 0; nt_x < 32; ++nt_x) {
                Word nt_idx = vram[(sel << 10) | (nt_y * 32 + nt_x)];
                Byte attrb = vram[(sel << 10) + (0x3C0 | (8 * (nt_y / 4) + (nt_x / 4)))];
                if ((nt_y & 0x3) > 1) attrb >>= 4;
                if ((nt_x & 0x3) > 1) attrb >>= 2;
                attrb &= 0x3;
                for (Byte row = 0; row < 8; ++row) {
                    Byte low_group = 0, high_group = 0;
                    Word pt_idx = ((Word)ppu_ctrl.bkgr_select << 12) | (nt_idx << 4) | row;
                    mapper->ppu_read(pt_idx & 0xfff7, low_group);//read pattern table for specific tile;
                    mapper->ppu_read(pt_idx | 0x0008, high_group);
                    for (Byte col = 0; col < 8; ++col) {
                        Byte pixel = ((high_group & 0x1) << 1) | (low_group & 0x1);
                        low_group >>= 1;
                        high_group >>= 1;
                        nt_screen.SetPixel(nt_x * 8 + (7 - col), nt_y * 8 + row, get_color(attrb, pixel));
                    }
                }
            }
        }
  
        return nt_screen;
    }

    olc::Pixel& PPU::get_color(Byte palet, Byte pixel) {
        read(kPALETTE_BASE | (palet << 2) | pixel, palet_idx);
        return pal_screen[palet_idx & 0x3f];
    }

    olc::Sprite& PPU::get_pattern_table(Byte tb_sel, Byte palette) {
        for (Word tile_y = 0; tile_y < 16; ++tile_y) {
            for (Word tile_x = 0; tile_x < 16; ++tile_x) {
                for (Word row = 0; row < 8; ++row) {
                    Byte msb_group = 0, lsb_group = 0;
                    Word temp_addr = ( ((Word)tb_sel << 12) | (tile_y << 8) | (tile_x << 4) | row );
                    read(temp_addr & 0xfff7, lsb_group);
                    read(temp_addr | 0x0008, msb_group);
                    for (Word col = 0; col < 8; ++col) {
                        Byte pixel = (msb_group & 0x1) + (lsb_group & 0x1);
                        msb_group >>= 1; lsb_group >>= 1;
                        spr_pattern_table[tb_sel].SetPixel(
                            (tile_x << 3) + (7 - col),
                            (tile_y << 3) + row,
                            get_color(palette, pixel)
                        );
                    }
                }
            }
        }
        return spr_pattern_table[tb_sel];
    }

    std::string PPU::get_obj_attr_ent(Word index)
    {
        auto hex = [](uint32_t n) {
            char s[3]{ 0,0,0 };
            for (int i = 1; i >= 0; i--, n >>= 4)
                s[i] = "0123456789ABCDEF"[n & 0xF];
            return s;
        };
        auto bin = [](uint32_t n) {
            char s[5]{};
            for (int i = 3; i >= 0; --i, n >>= 1)
                s[i] = "01"[n & 0x1];
            return s;
        };

        std::string str{ "(" };
        str += hex(oam.ent[index].x_coord);
        str += ",";
        str += hex(oam.ent[index].y_coord);
        str += ")  ";
        str += hex(oam.ent[index].index);
        str += "  ";
        str += bin((oam.ent[index][2] & 0xf0) >> 4);
        str += " ";
        str += bin(oam.ent[index][2] & 0x0f);
        return str;
    }

    inline bool PPU::check_in_range(Byte y_coord){
        //sprite_size = 8 or 16;
        return (y_coord <= scanline) && (y_coord + sprite_size > scanline);
    }

    inline bool PPU::check_render_enabled() {
        return (ppu_mask.bkgr_enable || ppu_mask.spr_enable);
    }

    void PPU::reset() {
        //Writes to the following registers are ignored if earlier than ~29658 CPU clocks after reset : 
        //  PPUCTRL, PPUMASK, PPUSCROLL, PPUADDR.
        //  This also means that the PPUSCROLL / PPUADDR latch will not toggle.
        //The other registers work immediately : 
        //  PPUSTATUS, OAMADDR, OAMDATA($2004), PPUDATA, and OAMDMA($4014).
        //
        //There is an internal reset signal that clears 
        //  PPUCTRL, PPUMASK, PPUSCROLL, PPUADDR, the PPUSCROLL / PPUADDR latch, and the PPUDATA read buffer. 
        //
        //This reset signal is set on reset and cleared at the end of VBlank, 
        //  by the same signal that clears the VBlank, sprite 0, and overflow flags.
        //Attempting to write to a register while it is being cleared has no effect, which explains why writes are "ignored" after reset.
        //
        ppu_ctrl = 0;      //$2000
        ppu_mask = 0;      //$2001
        ppu_status = 0xa0; //$2002 The VBL flag (bit 7) is random at power, and unchanged by reset. It is next set around 27384, then around 57165.
        //oam_addr = 0;    //$2003 unchanged by reset; changed during rendering and cleared at the end of normal rendering;
        //oam_data = 0;    //$2004 <- deleted;
        ppu_scroll = 0;    //$2005
        //ppu_addr = 0;    //$2006
        ppu_data = 0;      //$2007

        frame = 0;
        scanline = 0;
        cycle = 0;

        //"Clearing PPUSCROLL and PPUADDR corresponds to clearing the VRAM address latch(T) and the fine X scroll.
        //Note that the VRAM address itself(V) is not cleared."
        //vram_addr.val = 0;
        temp_addr.val = 0;
        fine_x = 0;
        is_first_write = false;
        addr_increment = 1;               // <-- reset to general 1;
        temp_fine_x = 0;
        scroll_updated_while_rendering = false;

        //internal variable for tile fetch;
        bkgr_addr = 0;
        bkgr_next_id = 0;
        bkgr_next_attr = 0;
        bkgr_next_lsb = 0;
        bkgr_next_msb = 0;

        //background shifters;
        bkgr_shifter_patt_lo = 0;
        bkgr_shifter_patt_hi = 0;
        bkgr_shifter_attr_lo = 0;
        bkgr_shifter_attr_hi = 0;

        //background render buffers;
        extract_mask = 0;
        bkgr_pixel = 0;//low 2bits of pixel color index, choosing color byte within a palette;
        bkgr_attrb = 0;//high 2btis, choosing from 4 palettes;

        //internal variable for evaluating sprite;
        soam_idx = 0;
        eval_idx = 0;
        eval_off = 0;
        eval_state = 0;
        eval_data = 0;

        //internal variables for sprite fetch;
        sprt_num = 0;
        sprt_addr = 0;
        sprt_fetch_idx = 0;
        for (Byte i = 0; i < 8; ++i) {
            sprt_shifters_lo[i] = 0;
            sprt_shifters_hi[i] = 0;
            sprt_attr_latches[i] = 0;
            sprt_x_counters[i] = 0;
        }

        //sprite render buffers;
        sprt_pixel = 0;
        sprt_attrb = 0;
        sprt_priority = 0;

        //internal state register;
        sprite_size = 8;             //generally it's a 8*8 sprite;
        greyscale_mask = 0xff;       //generally do not mask;
        nmi_out = false;//active low signal outputted from PPU, can be seen by bus and CPU;
        frame_complete = false;
        //toggles for sprite0hit;
        sp0_pres_nl = false;//check for the next line while evaluating sprites;
        sp0_present = false;//indicate sp0 is present in current line;
        first_being_rendered = false;

        //internal variables for rendering;
        palet_idx = 0;
        output_pixel = 0;
        output_attrb = 0;


    }

    bool PPU::is_frame_complete()
    {
        bool ret = frame_complete;
        frame_complete = false;
        return ret;
    }

};//end nes;