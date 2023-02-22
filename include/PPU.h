#ifndef PPU_H
#define PPU_H

#include "olcPixelGameEngine.h"

#include "Mapper.h"

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
        olc::Sprite& get_pattern_table(Byte tb_sel, Byte palette);
        std::string get_obj_attr_ent(Word index);

        //debug:
        olc::Sprite& get_bkgr();
        olc::Sprite& get_name_table(Word sel);

    private:
        void fetch_bkgr_tile();
        void fetch_sprt_tile();
        //void fetch_tile();
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
    
        //micro operations:
        using MICOP = void(PPU::*)(void);
        void IDLE_DOT() {}
        void CLR_FLAG() {
            PR_FETCH();
            ppu_status = 0;//effectively clear vblank_flag, sprite_hit, and sprite_overflow;
            nmi_out = false;
            //clear sprite rendering buffers;
            for (Byte i = 0; i < 8; ++i) {
                sprt_shifters_lo[i] = 0;
                sprt_shifters_hi[i] = 0;
                sprt_x_counters[i] = 0xff;
                sprt_attr_latches[i] = 0;
            }
        }
        void PR_FETCH() {
            fetch_bkgr_tile();
        }
        void VISIBLES() {
            fetch_bkgr_tile();
            render_pixel();
            update_bkgr_shifters();
            update_sprt_shifters();
        }
        void CLR_SOAM() {
            VISIBLES();
            clear_sec_oam();
        }
        void SPR_EVAL() {
            VISIBLES();
            if (cycle == 65) sp0_pres_nl = false;
            eval_sprite();
        }
        void INC_VERT() {
            //cycle 256, the last visible cycle;
            SPR_EVAL();
            vram_addr.inc_vert();
            //need to clear for every scanline;
            eval_state = eval_idx = eval_off = 0;
            sprt_num = soam_idx;
            soam_idx = 0;
            sprt_fetch_idx = 0;
            sp0_present = false;
            sp0_present = sp0_pres_nl;
        }
        void INC_VT_P() {
            PR_FETCH();
            vram_addr.inc_vert();
        }
        void SP_FETCH() {
            fetch_sprt_tile();
        }
        void RST_HORI() {
            SP_FETCH();
            update_sprt_shifters();
            vram_addr.reset_hori(temp_addr);
        }
        void RST_VERT() {
            SP_FETCH();
            vram_addr.reset_vert(temp_addr);
        }
        void FT_FETCH() {
            fetch_bkgr_tile();
            update_bkgr_shifters();
        }
        void UD_FETCH() {
            switch (cycle & 0x7) {
            case 1://for cycle 337;
                load_bkgr_shifters();
                read(vram_addr.get_nt_addr(), bkgr_next_id);
                break;
            case 3://for cycle 339;
                read(vram_addr.get_nt_addr(), bkgr_next_id);
                if (scanline == 0) ++cycle;//odd frame skip;
                break;
            }
        }

    public:
        bool nmi_out = false;//active low signal outputted from PPU, can be seen by bus and CPU;
        bool frame_complete = false;
    
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

        VRAM vram;
        Palette palette;
        OAM oam;
        SEC_OAM sec_oam;

        olc::Pixel  pal_screen[0x40];//system palette
        olc::Sprite spr_screen{ 256, 240 };//main composite screen;
        olc::Sprite spr_pattern_table[2] = { {128, 128}, {128, 128} };
        olc::Sprite bkgr_screen{ 256, 240 };//only used for debugging, to check the background;
        olc::Sprite nt_screen{ 256, 240 };//only used for debugging;
        
        Counter frame = 0;
        Word scanline = 0;
        Word cycle = 0;
        
        LOOPY_REG vram_addr;//current vram address;
        LOOPY_REG temp_addr;//temp vram addr, or as the address of the top left onscreen tile;
        Byte fine_x = 0;//fine x scroll; low 3 bits address 8 pixels;
        bool is_first_write = false;//toggle: 1 = after 1st write; 0 = after 2nd write / init;
        Byte addr_increment = 1;
        
        Byte temp_fine_x = 0;
        bool scroll_updated_while_rendering = false;

        Word bkgr_addr = 0;
        Byte bkgr_next_id = 0;
        Byte bkgr_next_attr = 0;
        Byte bkgr_next_lsb = 0;
        Byte bkgr_next_msb = 0;

        Word bkgr_shifter_patt_lo = 0;
        Word bkgr_shifter_patt_hi = 0;
        Word bkgr_shifter_attr_lo = 0;
        Word bkgr_shifter_attr_hi = 0;

        Word extract_mask = 0;
        Byte bkgr_pixel = 0;//low 2bits of pixel color index, choosing color byte within a palette;
        Byte bkgr_attrb = 0;//high 2btis, choosing from 4 palettes;

        Byte soam_idx = 0;
        Byte eval_idx = 0;
        Byte eval_off = 0;
        Byte eval_state = 0;
        Byte eval_data = 0;

        Byte sprite_size = 8;

        Byte sprt_num = 0;
        Word sprt_addr = 0;
        Byte sprt_fetch_idx = 0;
        Byte sprt_shifters_lo[8]{};
        Byte sprt_shifters_hi[8]{};
        Byte sprt_attr_latches[8]{};
        Byte sprt_x_counters[8]{};

        Byte sprt_pixel = 0;
        Byte sprt_attrb = 0;
        Byte sprt_priority = 0;

        bool sp0_pres_nl = false;//check for the next line while evaluating sprites;
        bool sp0_present = false;//indicate sp0 is present in current line;
        bool sp0_being_rendered = false;

        Byte output_pixel = 0;
        Byte output_attrb = 0;

        //tempoary variable for internal use;
        Word temp_word = 0;
        Byte temp_byte = 0;
      
        std::shared_ptr<Mapper> mapper = nullptr;

        //micro operation matrix: 
        static constexpr MICOP micop_mtx[2][341] = {
            {
                // row 0 refers to the pre-render scanline;
                // row 1 refers to visible scanlines;
                //    0              1              2              3              4              5              6              7
                &PPU::IDLE_DOT,&PPU::CLR_FLAG,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 0
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 1
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 2
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 3
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 4
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 5
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 6
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 7
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 8
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 9
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 10
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 11
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 12
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 13
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 14
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 15
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 16
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 17
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 18
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 19
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 20
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 21
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 22
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 23
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 24
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 25
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 26
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 27
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 28
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 29
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 30
                &PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH,&PPU::PR_FETCH, // 31
                &PPU::INC_VT_P,&PPU::RST_HORI,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 32
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 33
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 34
                &PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT, // 35
                &PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT, // 36
                &PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT,&PPU::RST_VERT, // 37
                &PPU::RST_VERT,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 38
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 39
                &PPU::SP_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH, // 40
                &PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH, // 41
                &PPU::FT_FETCH,&PPU::UD_FETCH,&PPU::UD_FETCH,&PPU::UD_FETCH,&PPU::UD_FETCH, },                                           // 42
            {
                //    0              1              2              3              4              5              6              7
                &PPU::IDLE_DOT,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 0
                &PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 1
                &PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 2
                &PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 3
                &PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 4
                &PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 5
                &PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 6
                &PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM,&PPU::CLR_SOAM, // 7
                &PPU::CLR_SOAM,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 8
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 9
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 10
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 11
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 12
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 13
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 14
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 15
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 16
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 17
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 18
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 19
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 20
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 21
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 22
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 23
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 24
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 25
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 26
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 27
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 28
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 29
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 30
                &PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL,&PPU::SPR_EVAL, // 31
                &PPU::INC_VERT,&PPU::RST_HORI,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 32
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 33
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 34
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 35
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 36
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 37
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 38
                &PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH,&PPU::SP_FETCH, // 39
                &PPU::SP_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH, // 40
                &PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH,&PPU::FT_FETCH, // 41
                &PPU::FT_FETCH,&PPU::UD_FETCH,&PPU::UD_FETCH,&PPU::UD_FETCH,&PPU::UD_FETCH, }                                            // 42
        };
    };

};//end nes

#endif