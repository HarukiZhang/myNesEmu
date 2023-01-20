#ifndef MAINBUS_H
#define MAINBUS_H

#include "PPU.h"
#include "Mapper.h"

namespace nes {

    class MainBus {
    public:
        MainBus();
        void connect(PPU *_ppu, Mapper *_mapp);
        bool read(Word addr, Byte &data);
        bool write(Word addr, Byte data);
    private:
        RAM ram;

        PPU *ppu = nullptr;
        Mapper *mapper = nullptr;


        //TODO: APU:
        //TODO: DMA;
        //TODO: JoySticks IO;
    };

};//end nes

#endif