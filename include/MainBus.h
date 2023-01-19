#ifndef MAINBUS_H
#define MAINBUS_H

#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"
#include "Mapper.h"

namespace nes {

    class MainBus {
    public:
        MainBus();
        void connect(CPU *_cpu, RAM *_ram);
        bool read(Word addr, Byte &data);
        bool write(Word addr, Byte data);
    private:
        CPU *cpu = nullptr;
        PPU *ppu = nullptr;
        // Cartridge *cart = nullptr;  do not need to directly link to cart;
        Mapper *mapper = nullptr;
        RAM *ram = nullptr;
        // OAM *oam = nullptr;
        // OAM_BUF *oam_buf = nullptr;
    
    //temporarily, used for testing cpu to read instructions;
    public:
        Byte *cart_space = nullptr;
    };

};//end nes

#endif