#ifndef PPU_H
#define PPU_H

#include "olcPixelGameEngine.h"

#include "Mapper.h"
#include "Log.h"

#define T_SP0H

namespace nes {

    //forward declaration;
    class MainBus;

    class PPU {
        friend class MainBus;//for mainbus to access oam memory within ppu;
    public:
        PPU();
        ~PPU();
        void connect(std::shared_ptr<Mapper> &mapp);
        bool read_regs(Word addr, Byte &data);
        bool write_regs(Word addr, Byte data);
        bool read(Word addr, Byte &data);//read ppu mapping space;
        bool write(Word addr, Byte data);//write ppu mapping space;
        void clock();
        void reset();

        olc::Sprite& get_screen();
        olc::Pixel& get_color(Byte palet, Byte pixel);

        //debug:
        bool is_frame_complete();
        olc::Sprite& get_pattern_table(Byte tb_sel, Byte palette);
        std::string get_obj_attr_ent(Word index);
        //olc::Sprite& get_bkgr();
        olc::Sprite& get_name_table(Word sel);

    private:
        void fetch_bkgr_tile();
        void fetch_sprt_tile();
        void render_pixel();
        void clear_sec_oam();
        void eval_sprite();
        void load_bkgr_shifters();
        void update_bkgr_shifters();
        void update_sprt_shifters();
        
        bool check_in_range(Byte y_coord);
        Word get_bkgr_patt_addr();
        Word get_sprt_addr();
        bool check_render_enabled();

        void fetch_nt_tile();
        void fetch_at_tile();
        void fetch_bg_tile();
        void fetch_sp_tile();
    
        //micro operations:
        using MICOP = void(PPU::*)(void);

        void IDLE_DOT() {}
        //clear flags in ppu_status + shut down nmi;
        void CLR_FLAG() {
            IN_FE_NT();
            ppu_status = 0;//effectively clear vblank_flag, sprite_hit, and sprite_overflow;
            sp0_hit_flag = false;
            nmi_out = false;
            //clear sprite rendering buffers;
            for (Byte i = 0; i < 8; ++i) {
                sprt_shifters_lo[i] = 0;
                sprt_shifters_hi[i] = 0;
                sprt_x_counters[i] = 0xff;
                sprt_attr_latches[i] = 0;
            }
#ifdef T_SP0H
            LOG() << "[PPU] vblank finished" << std::endl;
#endif
        }
        //rendering within scanline 1 to 240;
        void RENDER_N() {
            if (!check_render_enabled()) return;
            render_pixel();
            update_bkgr_shifters();
            update_sprt_shifters();
        }
        //invisible nametable fetches;
        void IN_FE_NT() {
            if (!check_render_enabled()) return;
            load_bkgr_shifters();
            fetch_nt_tile();
            if (cycle == 337) return;
            update_bkgr_shifters();
            update_bkgr_shifters();
        }
        //invisible attrtable fetches;
        void IN_FE_AT() {
            if (!check_render_enabled()) return;
            fetch_at_tile();
            update_bkgr_shifters();
            update_bkgr_shifters();
        }
        //invisible background tile fetches from pattTable;
        void IN_FE_BG() {
            if (!check_render_enabled()) return;
            fetch_bg_tile();
            update_bkgr_shifters();
            update_bkgr_shifters();
        }
        //visible nametable fetches with clearing sec oam;
        void V_F_N_CS() {
            if (!check_render_enabled()) return;
            load_bkgr_shifters();
            fetch_nt_tile();
            RENDER_N();
            clear_sec_oam();
        }
        //visible attrtable fetches with clearing sec oam;
        void V_F_A_CS() {
            if (!check_render_enabled()) return;
            fetch_at_tile();
            RENDER_N();
            clear_sec_oam();
        }
        //visible background tile fetches with clearing sec oam;
        void V_F_B_CS() {
            if (!check_render_enabled()) return;
            fetch_bg_tile();
            RENDER_N();
            clear_sec_oam();
        }
        //visible nametable fetches with sprite evaluation;
        void V_F_N_SE() {
            if (!check_render_enabled()) return;
            load_bkgr_shifters();
            fetch_nt_tile();
            RENDER_N();
            eval_sprite();
            //visible nametable fetches with clearing sec oam;
        }
        //visible attrtable fetches with sprite evaluation;
        void V_F_A_SE() {
            if (!check_render_enabled()) return;
            fetch_at_tile();
            RENDER_N();
            eval_sprite();
        }
        //visible background tile fetches with sprite evaluation;
        void V_F_B_SE() {
            if (!check_render_enabled()) return;
            fetch_bg_tile();
            RENDER_N();
            eval_sprite();
        }
        //increment x scroll;
        void INC_HORI() {
            if (!check_render_enabled()) return;
            RENDER_N();
            vram_addr.inc_hori();
        }
        //increment x scroll for pre-render line;
        void INC_HR_P() {
            if (!check_render_enabled()) return;
            vram_addr.inc_hori();
        }
        //increment y scroll;
        void INC_VERT() {
            //cycle 256, the last visible cycle;
            if (!check_render_enabled()) return;
            //no need to eval sprite in this dot;
            RENDER_N();
            vram_addr.inc_hori();
            vram_addr.inc_vert();
            //need to clear for every scanline;
            eval_state = eval_idx = eval_off = 0;
            sprt_num = soam_idx;
            soam_idx = 0;
            sprt_fetch_idx = 0;
            sp0_present = sp0_pres_nl;
            sp0_pres_nl = false;
            for (Byte i = 0; i < 8; ++i) {
                sprt_shifters_lo[i] = 0;
                sprt_shifters_hi[i] = 0;
                sprt_x_counters[i] = 0xff;
                sprt_attr_latches[i] = 0;
            }
        }
        //increment y scroll for pre-render line;
        void INC_VT_P() {
            if (!check_render_enabled()) return;
            vram_addr.inc_hori();
            vram_addr.inc_vert();
            //need to clear for every scanline;
            eval_state = eval_idx = eval_off = 0;
            sprt_num = soam_idx;
            soam_idx = 0;
            sprt_fetch_idx = 0;
            sp0_present = sp0_pres_nl;
            sp0_pres_nl = false;
            for (Byte i = 0; i < 8; ++i) {
                sprt_shifters_lo[i] = 0;
                sprt_shifters_hi[i] = 0;
                sprt_x_counters[i] = 0xff;
                sprt_attr_latches[i] = 0;
            }
        }
        //sprite tile fetches at cycle 257 to 320;
        void SP_FETCH() {
            if (!check_render_enabled()) return;
            fetch_sp_tile();
        }
        //reset x scroll;
        void RST_HORI() {
            //at cycle 257;
            if (!check_render_enabled()) return;
            //no need to fetch sprite tile;
            update_sprt_shifters();
            vram_addr.reset_hori(temp_addr);
            if (scroll_updated_while_rendering) {
                scroll_updated_while_rendering = false;
                fine_x = temp_fine_x;
            }
        }
        //reset y scroll;
        void RST_VERT() {
            //only need to reset at cycle 280 and 304;
            if (!check_render_enabled()) return;
            vram_addr.reset_vert(temp_addr);
        }
        //the 339 dot at pre-render line;
        void ODD_SKIP() {
            if (check_render_enabled()) {
                fetch_nt_tile();
            }
            ++cycle;//odd frame skip;
        }
        //the visible zone version of 339 dot;
        void FET_NT_N() {
            if (!check_render_enabled()) return;
            fetch_nt_tile();
        }

    private:
        //PPU I/O Registers:
        PPU_CTRL ppu_ctrl;     //$2000
        PPU_MASK ppu_mask;     //$2001
        PPU_STATUS ppu_status; //$2002
        Byte oam_addr = 0;     //$2003
        //Byte oam_data = 0;   //$2004 <- no need;
        Byte ppu_scroll = 0;   //$2005
        Byte ppu_addr = 0;     //$2006
        Byte ppu_data = 0;     //$2007

        //Memory related to PPU;
        //VRAM vram; <-- put to Mapper;
        Palette palette;
        OAM oam;
        SEC_OAM sec_oam;

        //olc PGE buffer;
        olc::Pixel  pal_screen[0x40];//system palette
        olc::Sprite spr_screen{ 256, 240 };//main composite screen;
        olc::Sprite spr_pattern_table[2] = { {128, 128}, {128, 128} };

        //olc::Sprite bkgr_screen{ 256, 240 };//only used for debugging, to check the background;
        olc::Sprite nt_screen{ 256, 240 };//only used for debugging;
        
        //counters;
        Counter frame = 0;
        Word scanline = 0;
        Word cycle = 0;
        
        //PPU internal VRAM address;
        LOOPY_REG vram_addr;//current vram address;
        LOOPY_REG temp_addr;//temp vram addr, or as the address of the top left onscreen tile;
        Byte fine_x = 0;//fine x scroll; low 3 bits address 8 pixels;
        bool is_first_write = false;//toggle: 1 = after 1st write; 0 = after 2nd write / init;
        Byte addr_increment = 1;
        Byte temp_fine_x = 0;
        bool scroll_updated_while_rendering = false;

        //internal variable for tile fetch;
        Word bkgr_addr = 0;
        Byte bkgr_next_id = 0;
        Byte bkgr_next_attr = 0;
        Byte bkgr_next_lsb = 0;
        Byte bkgr_next_msb = 0;

        //background shifters;
        Word bkgr_shifter_patt_lo = 0;
        Word bkgr_shifter_patt_hi = 0;
        Word bkgr_shifter_attr_lo = 0;
        Word bkgr_shifter_attr_hi = 0;

        //background render buffers;
        Word extract_mask = 0;
        Byte bkgr_pixel = 0;//low 2bits of pixel color index, choosing color byte within a palette;
        Byte bkgr_attrb = 0;//high 2btis, choosing from 4 palettes;

        //internal variable for evaluating sprite;
        Byte soam_idx = 0;
        Byte eval_idx = 0;
        Byte eval_off = 0;
        Byte eval_state = 0;
        Byte eval_data = 0;

        //internal variables for sprite fetch;
        Byte sprt_num = 0;
        Word sprt_addr = 0;
        Byte sprt_fetch_idx = 0;
        Byte sprt_shifters_lo[8]{};
        Byte sprt_shifters_hi[8]{};
        Byte sprt_attr_latches[8]{};
        Byte sprt_x_counters[8]{};

        //sprite render buffers;
        Byte sprt_pixel = 0;
        Byte sprt_attrb = 0;
        Byte sprt_priority = 0;

        //internal state register;
        Byte sprite_size = 8;
        Byte greyscale_mask = 0xff;
        bool nmi_out = false;//active low signal outputted from PPU, can be seen by bus and CPU;
        bool frame_complete = false;
        bool fetch_toggle = false;//0: fetch low byte; 1: fetch high byte;
        //toggles for sprite0hit;
        bool sp0_pres_nl = false;//check for the next line while evaluating sprites;
        bool sp0_present = false;//indicate sp0 is present in current line;
        bool first_being_rendered = false;
        bool sp0_hit_flag = false;

        //internal variables for rendering;
        Byte palet_idx = 0;
        Byte output_pixel = 0;
        Byte output_attrb = 0;

        //debug:
        Word scanline_counter_counts = 0;
        Word ppu_bus_latch = 0;

        std::shared_ptr<Mapper> mapper = nullptr;

        static constexpr MICOP micop_mtx[21] = {
            & PPU::IDLE_DOT,
            & PPU::RENDER_N,
            & PPU::IN_FE_BG,
            & PPU::V_F_B_SE,
            & PPU::IN_FE_NT,
            & PPU::IN_FE_AT,
            & PPU::INC_HR_P,
            & PPU::SP_FETCH,
            & PPU::INC_HORI,
            & PPU::V_F_A_SE,
            & PPU::V_F_N_SE,
            & PPU::V_F_B_CS,
            & PPU::V_F_N_CS,
            & PPU::V_F_A_CS,
            & PPU::RST_HORI,
            & PPU::RST_VERT,
            & PPU::INC_VT_P,
            & PPU::INC_VERT,
            & PPU::CLR_FLAG,
            & PPU::ODD_SKIP,
            & PPU::FET_NT_N,

        };

        //micro operation matrix: 
        static constexpr Byte opcode_mtx[2][341] = {
                // row 0 refers to the pre-render scanline;  row 1 refers to visible scanlines;
            {

                0,	18,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	16,	14,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	15,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	7,	0,	7,	0,	15,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	19,	0,

                //0,	
                //18,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	16,	
                //14,	0,	0,	0,	6,	0,	6,	0,	0,	0,	0,	0,	6,	0,	6,	0,	
                //0,	0,	0,	0,	6,	0,	6,	15,	0,	0,	0,	0,	6,	0,	6,	0,	
                //0,	0,	0,	0,	6,	0,	6,	0,	0,	0,	0,	6,	0,	6,	0,	15,	
                //0,	0,	0,	0,	6,	0,	6,	0,	0,	0,	0,	0,	6,	0,	6,	0,	
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	
                //4,	0,	20,	0,

            },
            {

                0,	12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	17,	14,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	0,	0,	0,	0,	7,	0,	7,	0,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	5,	0,	2,	0,	2,	6,	4,	0,	20,	0,

                //0,	
                //12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	//16
                //12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	//32 ^
                //12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	//48 |
                //12,	1,	13,	1,	11,	1,	11,	8,	12,	1,	13,	1,	11,	1,	11,	8,	//64 +-- visible bg fetches with clear sec oam;
                //19,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//80
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//96
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//112
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//128
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//144
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//160
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//176
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//192
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//208
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//224 ^
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	8,	//240 |
                //10,	1,	9,	1,	3,	1,	3,	8,	10,	1,	9,	1,	3,	1,	3,	17,	//256 +-- visible bg fetches with sprite eval;
                //14,	0,	0,	0,	6,	0,	6,	0,	0,	0,	0,	0,	6,	0,	6,	0,	//272
                //0,	0,	0,	0,	6,	0,	6,	0,	0,	0,	0,	0,	6,	0,	6,	0,	//288 ^
                //0,	0,	0,	0,	6,	0,	6,	0,	0,	0,	0,	0,	6,	0,	6,	0,	//304 |
                //0,	0,	0,	0,	6,	0,	6,	0,	0,	0,	0,	0,	6,	0,	6,	0,	//320 +-- sprite fetches;
                //4,	0,	5,	0,	2,	0,	2,	7,	4,	0,	5,	0,	2,	0,	2,	7,	//336 <-- first two tiles fetch;
                //4,	0,	21,	0,                                                  //340 <-- unused nt fetches;

            }
        };
    };

};//end nes

#endif