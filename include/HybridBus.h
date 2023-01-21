#ifndef HYBRIDBUS_H
#define HYBRIDBUS_H

#include "Mapper.h"

namespace nes {

    class HybridBus {
    public:
        void connect(Mapper *_mapp);
        bool read(Word addr, Byte &data);
        bool write(Word addr, Byte data);
        
    private:
        VRAM vram;
        Palette palette;

        Mapper *mapper = nullptr;

        NT_Mirror nt_mirror;
    };



};//end nes

#endif