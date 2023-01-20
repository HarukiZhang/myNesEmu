#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "MainBus.h"
#include "PPU.h"
#include "HybridBus.h"
#include "Cartridge.h"

namespace nes {

    class Emulator {
    private:
        CPU cpu;
        MainBus mbus;
        PPU ppu;
        HybridBus hbus;
        Cartridge cart;

        Mapper *mapper = nullptr;
    };//end class
    
};//end nes

#endif