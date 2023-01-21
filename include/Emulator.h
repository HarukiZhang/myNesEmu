#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "MainBus.h"
#include "PPU.h"
#include "HybridBus.h"
#include "Cartridge.h"

#include "Mapper_NROM.h"

namespace nes {

    class Emulator {
        Emulator(){
            mbus.connect(&cpu, &ppu, mapper);
            ppu.connect(&hbus);
            //TODO: according to cart, allocate a Mapper;
            mapper = new Mapper_NROM{cart};
        }
    private:
        CPU cpu;
        PPU ppu;
        MainBus mbus;
        HybridBus hbus;
        Cartridge cart;
        Mapper *mapper = nullptr;
    };//end class
    
};//end nes

#endif