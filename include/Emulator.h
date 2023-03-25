#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "MainBus.h"
#include "PPU.h"
#include "HybridBus.h"
#include "Cartridge.h"


namespace nes {

    class Emulator {
        Emulator(){
            mbus.connect(&cpu, &ppu, mapper);
            //ppu.connect(&hbus);
            //TODO: according to cart, allocate a Mapper;
        }
    private:
        CPU cpu;
        PPU ppu;
        MainBus mbus;
        HybridBus hbus;
        Cartridge cart;
        std::shared_ptr<Mapper> mapper = nullptr;
    };//end class
    
};//end nes

#endif