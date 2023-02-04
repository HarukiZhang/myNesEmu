#ifndef PPU_H
#define PPU_H

#include "HybridBus.h"

namespace nes {

    //forward declaration;
    class MainBus;

    class PPU {
        friend class MainBus;//for mainbus to access oam memory within ppu;
    public:
        PPU();
        ~PPU();
        void connect(HybridBus *_hb_bus);
        bool read_regs(Word addr, Byte &data);
        bool write_regs(Word addr, Byte data);
        inline bool read(Word addr, Byte &data);
        inline bool write(Word addr, Byte data);
        void clock();
        void reset();
    private:
        void fetch_bkgr_tile();
        inline void load_bkgr_shifters();
        inline void update_shifters();
        
        bool obj_compare(Byte n_scanl, Byte y_coord);
    public:
        bool nmi_out = false;
        bool frame_complete = false;
    private:
        PPU_CTRL ppu_ctrl;     //$2000
        PPU_MASK ppu_mask;     //$2001
        PPU_STATUS ppu_status; //$2002
        Byte oam_addr;         //$2003
        Byte oam_data;         //$2004
        Byte ppu_scroll;       //$2005
        Byte ppu_addr;         //$2006
        Byte ppu_data;         //$2007

        OAM oam;
        OAM_BUF oam_buf;
        
        Counter frame = 0;
        Word scanline = 0;
        Word cycle = 0;
        
        LOOPY_REG vram_addr;//current vram address;
        LOOPY_REG temp_addr;//temp vram addr, or as the address of the top left onscreen tile;
        Byte fine_x;//fine x scroll; low 3 bits address 8 pixels;
        bool is_first_write = false;//toggle: 1 = after 1st write; 0 = after 2nd write / init;
        
        Byte bkgr_next_id;
        Byte bkgr_next_attr;
        Byte bkgr_next_lsb;
        Byte bkgr_next_msb;

        Word bkgr_shifter_patt_lo;
        Word bkgr_shifter_patt_hi;
        Byte bkgr_shifter_attr_lo;
        Byte bkgr_shifter_attr_hi;

        //latch the pixel infos to put onto the screen;
        Byte bkgr_pixel;
        Byte bkgr_palet;

        //tempoary variable for internal use;
        Word temp_word;
        Byte temp_byte;
      
        HybridBus *hb_bus = nullptr;
    };

};//end nes

#endif