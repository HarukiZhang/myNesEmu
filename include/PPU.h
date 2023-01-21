#ifndef PPU_H
#define PPU_H

#include "HybridBus.h"

namespace nes {

    class PPU {
    public:
        void connect(HybridBus *_hb_bus);
        bool read_regs(Word addr, Byte &data);
        bool write_regs(Word addr, Byte data);
    private:
        void print_frame();
        void print_scanline();
        bool fetch(Word addr, Byte &data);
        bool put_pixel();
        bool obj_compare(Byte n_scanl, Byte y_coord);
    private:
        PPU_REG ppu_regs;
        OAM oam;
        OAM_BUF oam_buf;
        
        Byte ppu_gen_latch;

        Word ppu_obj_comp;//object evaluation comparator reg;
        Word obj_eval_ul;//object evalutaion upper limit;

        HybridBus *hb_bus = nullptr;
    };

};//end nes

#endif