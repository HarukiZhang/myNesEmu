#ifndef PPU_H
#define PPU_H

#include "HybridBus.h"

namespace nes {

    //forward declaration;
    class MainBus;

    class PPU {
        friend class MainBus;
    public:
        void connect(HybridBus *_hb_bus);
        bool read_regs(Word addr, Byte &data);
        bool write_regs(Word addr, Byte data);
        // inline friend void oam_dma(PPU &_ppu, const Byte *_ram_ptr);//only allowed use by DMA;
        inline bool read(Word addr, Byte &data);
        inline bool write(Word addr, Byte data);
        void clock();
    private:
        void fetch_bkgr_tile();
        void print_frame();
        void print_scanline();
        bool fetch(Word addr, Byte &data);
        bool put_pixel();
        bool obj_compare(Byte n_scanl, Byte y_coord);
    public:
        bool nmi_out = false;
    private:
        PPU_REG ppu_regs;
        OAM oam;
        OAM_BUF oam_buf;
        
        Word scanline;
        Word cycle;
        
        LOOPY_REG vram_addr;//current vram address;
        LOOPY_REG temp_addr;//temp vram addr, or as the address of the top left onscreen tile;
        Byte fine_x;//fine x scroll; low 3 bits address 8 pixels;
        bool is_first_write = false;//toggle: 1 = after 1st write; 0 = after 2nd write / init;
        
        Word ppu_obj_comp;//object evaluation comparator reg;
        Word obj_eval_ul;//object evalutaion upper limit;
        
        HybridBus *hb_bus = nullptr;
    };

};//end nes

#endif