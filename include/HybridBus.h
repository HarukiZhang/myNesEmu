#ifndef HYBRIDBUS_H
#define HYBRIDBUS_H

#include "NESDef.h"
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
        OAM oam;
        OAM_BUF oambuf;

        Mapper *mapper = nullptr;

        NTMirror ntmirror;
    };



};//end nes

#endif