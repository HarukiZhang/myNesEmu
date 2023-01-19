#include<iostream>

#include "MainBus.h"
#include "Mapper.h"

namespace nes {

    MainBus::MainBus(){}

    void MainBus::connect(CPU *_cpu, RAM *_ram){
        cpu = _cpu;
        ram = _ram;
    }

    bool MainBus::read(Word addr, Byte &data){
        //prioritize mapper to control the space mapping;
        if (mapper->cpu_read(addr, data)) return true;
        //NROM mapper ignore addr below 0x6000;
        if (addr < 0x2000){
            data = ram->vals[addr & 0x7ff];
            return true;
        }
        else if (addr < 0x4000){
            return ppu->read_regs(addr & 0x0007, data);
        }
        else if (addr < 0x4018){// <-- modify to : < 0x6000 ?
            //TODO: support for I/O regs;
            return false;
            // 0100 0000 0001 0100 : $4014 : DMA control
            // 0100 0000 0001 0101 : $4015 : Sound Channel
            // 0100 0000 0001 0110 : $4016 : JoyStick 1
            // 0100 0000 0001 0111 : $4017 : JoyStick 2
        }
        //TODO: there's a gap between 0x4018 - 0x5FFF;
        return false;
    }

    bool MainBus::write(Word addr, Byte data){
        if (mapper->cpu_write(addr, data)) return true;
        if (addr < 0x2000){
            ram->vals[addr & 0x7ff] = data;
            return true;
        }
        else if (addr < 0x4000){
            return ppu->write_regs(addr & 0x0007, data);
        }
        else if (addr < 0x4020){
            std::cerr << "Try to write to APU reg, but no APU now." << std::endl;
            return false;
        }
        else {
            //use mapper to locate ROM/RAM in Cart;
            // cart->get_mapper()->write(addr, data);
            // return true;

            //simplify the address space;
            if (addr < 0x8000){
                std::cerr << "Try to write to $4020 - $7FFF, where is not valid now." << std::endl;
                return false;
            }
            else if (addr < 0x10000){
                std::cerr << "Try to write to cart_space, where is read-only." << std::endl;
                return false;
            }
        }
        return false;
    }

};//end nes;