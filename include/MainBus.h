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
        void reset();

        //TODO: APU related functions;
        //TODO: JoySticks IO functions;
        
        //TODO: DMA or direct access to OAM;
        void oam_transfer(Word offset, Byte data);
        //TODO: DMC DMA fucntion;
        //void DMC_DMA();

        void clock();
        //check signal from each devices;
        //MainBus should only check signals and inform CPU that there is interrupt request,
        //  but not to "command" CPU to be interrupted;
        void check_nmi();


        //debug:
        Byte get_ram(Word addr) {
            return ram[addr];
        }
        void set_irq() {
            irq_detected = true;
        }
        void set_nmi() {
            nmi_detected = true;
        }
    public:
        Byte controller[2];

    private:
        RAM ram;//ram for CPU;
        //IO_REG io_regs;
        Byte controller_state[2];

        CPU *cpu = nullptr;//cpu pointer is used for informing cpu: how many dma extra cycles counted, and interrupts;
        PPU *ppu = nullptr;
        std::shared_ptr<Mapper> mapper = nullptr;



        Counter sys_clock = 0;//count CPU clock cycles;

        friend class CPU;
        bool irq_detected = false;
        bool nmi_detected = false;

        // bool cycle_rw = false;//true: read cycle; false: write cycle;
        //   ^---- deprecated;
        bool cpu_halt = false;//used by OAM_DMA;
        Byte extra_dma_cycles = 0;//used by OAM_DMA;
    };

};//end nes

#endif