#ifndef MAINBUS_H
#define MAINBUS_H

#include "PPU.h"

namespace nes {

    //forward declaration;
    class CPU;

    class MainBus {
    public:
        MainBus();
        void connect(CPU *_cpu, PPU *_ppu, Mapper *_mapp);
        bool read(Word addr, Byte &data);
        bool write(Word addr, Byte data);

        void DMA(Word addr);
    private:
        RAM ram;

        CPU *cpu = nullptr;
        PPU *ppu = nullptr;
        Mapper *mapper = nullptr;


        //TODO: APU:
        //TODO: DMA or direct access to OAM;
        //TODO: JoySticks IO;
    };

};//end nes

#endif