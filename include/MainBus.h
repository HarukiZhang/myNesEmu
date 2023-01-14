#ifndef MAINBUS_H
#define MAINBUS_H

#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"

namespace nes {

    class MainBus {
    public:
        MainBus(){}
        void connect(CPU *_cpu, RAM *_ram){
            cpu = _cpu;
            ram = _ram;
        }
        bool read(Word addr, Byte &data){
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
                cart->get_mapper()->read(addr, data);
                return true;
            }
        }
        bool write(Word addr, Byte data){
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
                cart->get_mapper()->read(addr, data);
                return true;
            }
        }
        Word discrete_logic(Word addr);
    private:
        CPU *cpu = nullptr;
        PPU *ppu = nullptr;
        Cart *cart = nullptr;
        RAM *ram = nullptr;
        // OAM *oam = nullptr;
        // OAM_BUF *oam_buf = nullptr;
    };

};//end nes

#endif