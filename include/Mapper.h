#ifndef MAPPER_H
#define MAPPER_H

#include "NESDef.h"
#include "Cartridge.h"

namespace nes {

    class Mapper {
    friend class Cartridge;
    public:
        Mapper(Cartridge &_cart);
        ~Mapper();

        virtual bool cpu_read(Word addr, Byte &data) = 0;
        virtual bool cpu_write(Word addr, Byte data) = 0;

        virtual bool ppu_read(Word addr, Byte &data) = 0;
        virtual bool ppu_write(Word addr, Byte data) = 0;

    protected:
        Cartridge *cart;
    };

};//end nes;

#endif