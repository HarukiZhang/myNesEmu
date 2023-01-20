#ifndef MAPPER_H
#define MAPPER_H

#include "NESDef.h"

namespace nes {

    //forward declaration;
    class Cartridge;

    class Mapper {
    public:
        Mapper(Cartridge &_cart);
        ~Mapper();

        virtual bool cpu_read(Word addr, Byte &data) = 0;
        virtual bool cpu_write(Word addr, Byte data) = 0;

        virtual bool ppu_read(Word addr, Byte &data) = 0;
        virtual bool ppu_write(Word addr, Byte data) = 0;

        NTMirror get_nt_mirror();

    protected:
        Cartridge *cart;
    };

};//end nes;

#endif