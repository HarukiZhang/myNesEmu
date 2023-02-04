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

        //TODO: APU related functions;
        //TODO: JoySticks IO functions;
        
        //TODO: DMA or direct access to OAM;
        void OAM_DMA();
        //TODO: DMC DMA fucntion;
        void DMC_DMA();
        void clock();
        inline void check_nmi();
    private:
        RAM ram;//ram for CPU;
        IO_REG io_regs;

        size_t sys_clock;

        // bool cycle_rw = false;//true: read cycle; false: write cycle;
        //   ^---- deprecated;
        bool cpu_halt = false;//used by OAM_DMA;
        Byte extra_dma_cycles = 0;//used by OAM_DMA;

        //cpu pointer is used for informing cpu:
        //how many dma extra cycles counted,
        //and interrupts;
        CPU *cpu = nullptr;
        PPU *ppu = nullptr;
        std::shared_ptr<Mapper> mapper = nullptr;
        
    };

};//end nes

#endif