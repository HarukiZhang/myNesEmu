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
        Byte &mirror_horizontal(Word addr);
        Byte &mirror_vertical(Word addr);
        Byte &mirror_single_screen(Word addr);
        Byte &mirror_four_screen(Word addr);
    private:
        VRAM vram;
        Palette palette;

        std::shared_ptr<Mapper> mapper = nullptr;
        Byte &(*nt_mirror)(Word);

        // NT_Mirror nt_mirror; <-- no need any more;
    };



};//end nes

#endif