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

    private:
        void fetch_bkgr_tile();
        void fetch_tile();
        inline void load_bkgr_shifters();
        inline void update_shifters();
        
        //internal helper functions:
        bool obj_compare(Byte n_scanl, Byte y_coord);
        Word get_bkgr_patt_addr();
        bool check_render_enabled();
    
    public:
        bool nmi_out = false;//active low signal outputted from PPU, can be seen by bus and CPU;
        bool frame_complete = false;
    
    private:
        //PPU I/O Registers:
        PPU_CTRL ppu_ctrl;     //$2000
        PPU_MASK ppu_mask;     //$2001
        PPU_STATUS ppu_status; //$2002
        Byte oam_addr = 0;     //$2003
        Byte oam_data = 0;     //$2004
        Byte ppu_scroll = 0;   //$2005
        Byte ppu_addr = 0;     //$2006
        Byte ppu_data = 0;     //$2007

        VRAM vram;
        Palette palette;
        OAM oam;
        OAM_BUF oam_buf;

        olc::Pixel  pal_screen[0x40];//system palette
        olc::Sprite spr_screen{ 256, 240 };
        //olc::Sprite sprNameTable[2] = { {256, 240}, {256, 240} };
        olc::Sprite spr_pattern_table[2] = { {128, 128}, {128, 128} };
        
        Counter frame = 0;
        Word scanline = 0;
        Word cycle = 0;
        bool sprite_fetch_enable = false;
        bool sprite_eval_enable = false;
        bool sec_oam_clear = false;
        
        LOOPY_REG vram_addr;//current vram address;
        LOOPY_REG temp_addr;//temp vram addr, or as the address of the top left onscreen tile;
        Byte fine_x = 0;//fine x scroll; low 3 bits address 8 pixels;
        bool is_first_write = false;//toggle: 1 = after 1st write; 0 = after 2nd write / init;
        
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

        //tempoary variable for internal use;
        Word temp_word = 0;
        Byte temp_byte = 0;
      
        std::shared_ptr<Mapper> mapper = nullptr;
    };

};//end nes

#endif