#ifndef MAINBUS_H
#define MAINBUS_H

#include "Mapper.h"

namespace nes {

    //forward declaration;
    class CPU;
    class PPU;

    class MainBus {
    public:
        MainBus();
        void connect(CPU *_cpu, PPU *_ppu, std::shared_ptr<Mapper> &_mapp);
        bool read(Word addr, Byte &data);
        bool write(Word addr, Byte data);

        void DMA();
    private:
        RAM ram;

        CPU *cpu = nullptr;
        PPU *ppu = nullptr;
        std::shared_ptr<Mapper> mapper = nullptr;

        Byte dma_reg;

        bool cycle_rw = false;//true: read cycle; false: write cycle;


        //TODO: APU:
        //TODO: DMA or direct access to OAM;
        //TODO: JoySticks IO;
    };

};//end nes

#endif