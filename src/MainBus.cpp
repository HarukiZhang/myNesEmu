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
        if (addr < 0x2000){
            data = ram->vals[addr & 0x7ff];
            return true;
        }
        else if (addr < 0x4000){
            addr &= 0x2007;
            return ppu->read_regs(addr - 0x2000, data);
        }
        else if (addr < 0x4020){
            std::cerr << "Try to read from APU reg, but no APU now." << std::endl;
            return false;
        }
        else {
            //use mapper to locate ROM/RAM in Cart;
            // cart->get_mapper()->read(addr, data);
            //simplify the address space;
            if (addr < 0x8000){
                std::cerr << "Try to read from $4020 - $7FFF, where is not valid now." << std::endl;
                return false;
            }
            else if (addr < 0x10000){
                data = cart_space[addr - 0x8000];
                return true;
            }
        }
        return false;
    }

    bool MainBus::write(Word addr, Byte data){
        if (addr < 0x2000){
            data = ram->vals[addr & 0x7ff];
            return true;
        }
        else if (addr < 0x4000){
            addr &= 0x2007;
            return ppu->read_regs(addr - 0x2000, data);
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