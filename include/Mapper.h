#ifndef MAPPER_H
#define MAPPER_H

#include "NESDef.h"
#include "Cartridge.h"

namespace nes {

    class Mapper {
    public:
        bool read(Word addr, Byte &data);
    private:
        Cartridge *cart;
    };

};//end nes;

#endif