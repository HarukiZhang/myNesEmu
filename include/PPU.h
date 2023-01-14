#ifndef PPU_H
#define PPU_H

#include "NESDef.h"


namespace nes {

    class PPU {
    public:
        bool read_regs(Word addr, Byte &data){
            switch (addr){
            //only reg#2, 4, 7 are readable;
            case 2 :
            case 4 :
            case 7 :
                data = ppu_gen_latch = ppu_regs.vals[addr];
                return true;
            break;
            default:
                //reading a "write-only" reg returns the latch's
                //current value, as do the unused bits of PPUSTATUS;
                data = ppu_gen_latch;
                return false;
            break;
            }
        }
        bool write_regs(Word addr, Byte data){
            switch (addr){
            case 0 :
            case 1 :
            case 3 :
            case 4 :
            case 5 :
            case 6 :
            case 7 :
                ppu_gen_latch = ppu_regs.vals[addr] = data;
                return true;
            break;
            default:
                ppu_gen_latch = data;
                return false;
            break;
            }
        }
    private:
        void print_frame();
        void print_scanline();
        bool fetch(Word addr, Byte &data);
        bool put_pixel();

        bool obj_compare(Byte n_scanl, Byte y_coord){
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
    private:
        PPU_REG ppu_regs;
        
        Byte ppu_gen_latch;

        Word ppu_obj_comp;//object evaluation comparator reg;
        Word obj_eval_ul;//object evalutaion upper limit;
    };

};//end nes

#endif