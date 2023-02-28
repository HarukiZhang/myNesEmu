#ifndef MAPPER_H
#define MAPPER_H

#include <memory>

#include "Cartridge.h"

namespace nes {

    class Mapper {
    public:
        Mapper(Cartridge &_cart);
        virtual ~Mapper() = default;

        virtual void reset() = 0;

        virtual bool cpu_read(Word addr, Byte &data) = 0;
        virtual bool cpu_write(Word addr, Byte data) = 0;

        virtual bool ppu_read(Word addr, Byte &data) = 0;
        virtual bool ppu_write(Word addr, Byte data) = 0;

        virtual bool irq_state() = 0;
        virtual bool count_scanline() = 0;

        static std::shared_ptr<Mapper> create_mapper(Cartridge &r_cart);

        //name table mirroring mapping
        void init_nt_mirror();

        Byte& mirror_horizontal(Word addr);
        Byte& mirror_vertical(Word addr);
        Byte& mirror_a_only(Word addr);//also called: one-screen lower bank;
        Byte& mirror_b_only(Word addr);//also called: one-screen upper bank;
        Byte& mirror_four_screen(Word addr);

    protected:
        //preserve a function pointer for derivatives;
        Byte& (Mapper::* nt_mirror_map)(Word);

        Cartridge *cart;

        VRAM vram;//PPU Video-RAM;
    };

};//end nes;

#endif