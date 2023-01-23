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
    private:
        RAM ram;//ram for CPU;
        IO_REG io_regs;
        // union IO_Regs {
        //     struct {
        //         Byte apu_regs[0x14];
        //         Byte dma_reg;       //$4014
        //         Byte audio_channel; //$4015
        //         Byte joystick_1;    //$4016
        //         Byte joystick_2;    //$4017
        //     };
        //     Byte vals[0x18];
        // } io_regs;

        CPU *cpu = nullptr;
        PPU *ppu = nullptr;
        std::shared_ptr<Mapper> mapper = nullptr;
        
        // bool cycle_rw = false;//true: read cycle; false: write cycle;
        //   ^---- deprecated;
        bool cpu_halt = false;//used by OAM_DMA;
        Byte extra_dma_cycles = 0;//used by OAM_DMA;
    };

};//end nes

#endif