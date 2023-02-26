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

        static std::shared_ptr<Mapper> create_mapper(Cartridge &r_cart);

        //name table mirroring mapping
        Word get_nt_mirror(Word addr);

        Word mirror_horizontal(Word addr);
        Word mirror_vertical(Word addr);
        Word mirror_a_only(Word addr);//also called: one-screen lower bank;
        Word mirror_b_only(Word addr);//also called: one-screen upper bank;
        Word mirror_four_screen(Word addr);

    protected:
        //preserve a function pointer for derivatives;
        Word (Mapper::*nt_mirror_map)(Word);

        Cartridge *cart;
    };

};//end nes;

#endif