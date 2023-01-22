#ifndef MAPPER_H
#define MAPPER_H

#include <memory>

#include "Cartridge.h"

namespace nes {

    class Mapper {
    public:
        Mapper(Cartridge &_cart);
        virtual ~Mapper() = default;

        virtual bool cpu_read(Word addr, Byte &data) = 0;
        virtual bool cpu_write(Word addr, Byte data) = 0;

        virtual bool ppu_read(Word addr, Byte &data) = 0;
        virtual bool ppu_write(Word addr, Byte data) = 0;

        NT_Mirror get_nt_mirror();

        static std::shared_ptr<Mapper> create_mapper(Cartridge &cr_cart);

    protected:
        Cartridge *cart;
    };

};//end nes;

#endif