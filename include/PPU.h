#ifndef PPU_H
#define PPU_H

#include "NESDef.h"


namespace nes {

    class PPU {
    public:
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
        
        Byte ppu_gen_latch;

        Word ppu_obj_comp;//object evaluation comparator reg;
        Word obj_eval_ul;//object evalutaion upper limit;
    };

};//end nes

#endif