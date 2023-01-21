#include<iostream>

#include "CPU.h"

namespace nes {

    MainBus::MainBus(){}

    void MainBus::connect(CPU *_cpu, PPU *_ppu, Mapper *_mapp){
        cpu = _cpu;
        cpu->connect(this);
        ppu = _ppu;
        mapper = _mapp;
    }

    bool MainBus::read(Word addr, Byte &data){
        switch (addr & 0xf000 >> 12){
        case 0x0 :
        case 0x1 :
            {
                data = ram.vals[addr & 0x7ff];
                return true;
            }
            break;
        case 0x2 :
        case 0x3 :
            {
                return ppu->read_regs(addr & 0x0007, data);
            }
            break;
        case 0x4 :
        case 0x5 :
            {
                // $4000 - $4013 : APU regs;
                // 0100 0000 0001 0100 : $4014 : DMA control
                // 0100 0000 0001 0101 : $4015 : Sound Channel
                // 0100 0000 0001 0110 : $4016 : JoyStick 1
                // 0100 0000 0001 0111 : $4017 : JoyStick 2
                //TODO: Expansion ROM
                return false;
            }
            break;
        default  :
            {
                //generally, only addresses beyond $6000 need to call mapper;
                return mapper->cpu_read(addr, data);
            }
            break;
        }
    }

    bool MainBus::write(Word addr, Byte data){
        switch (addr & 0xf000 >> 12){
        case 0x0 :
        case 0x1 :
            {
                ram.vals[addr & 0x7ff] = data;
                return true;
            }
            break;
        case 0x2 :
        case 0x3 :
            {
                return ppu->write_regs(addr & 0x0007, data);
            }
            break;
        case 0x4 :
        case 0x5 :
            {
                // $4000 - $4013 : APU regs;
                // 0100 0000 0001 0100 : $4014 : DMA control
                // 0100 0000 0001 0101 : $4015 : Sound Channel
                // 0100 0000 0001 0110 : $4016 : JoyStick 1
                // 0100 0000 0001 0111 : $4017 : JoyStick 2
                //TODO: Expansion ROM
                return false;
            }
            break;
        default  :
            {
                //generally, only addresses beyond $6000 need to call mapper;
                return mapper->cpu_write(addr, data);
            }
            break;
        }
    }

};//end nes;