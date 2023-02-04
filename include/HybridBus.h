#ifndef HYBRIDBUS_H
#define HYBRIDBUS_H

#include "Mapper.h"

namespace nes {

    class HybridBus {
    public:
        void connect(std::shared_ptr<Mapper> &_mapp);
        bool read(Word addr, Byte &data);
        bool write(Word addr, Byte data);
    
    private:
        VRAM vram;
        Palette palette;

        std::shared_ptr<Mapper> mapper = nullptr;

    };

};//end nes

#endif