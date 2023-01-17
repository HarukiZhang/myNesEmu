#ifndef MAINBUS_H
#define MAINBUS_H

#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"

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
        Cartridge *cart = nullptr;
        RAM *ram = nullptr;
        // OAM *oam = nullptr;
        // OAM_BUF *oam_buf = nullptr;
    
    public:
        Byte *cart_space = nullptr;//temporarily;
    };

};//end nes

#endif