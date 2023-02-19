#include<iostream>

#include "MainBus.h"
#include "CPU.h"
#include "PPU.h"

namespace nes {

    MainBus::MainBus(){}

    void MainBus::connect(CPU *_cpu, PPU *_ppu, std::shared_ptr<Mapper> &_mapp){
        cpu = _cpu;
        cpu->connect(this);
        ppu = _ppu;
        mapper = _mapp;
        ppu->connect(mapper);
    }

    void MainBus::clock() {
        ppu->clock();
        ppu->clock();
        ppu->clock();
        check_nmi();
        //if (sys_clock % 3 == 0)
        cpu->clock();
        ++sys_clock;
    }

    inline void MainBus::check_nmi() {
#ifdef S_MODE
        if (ppu->nmi_out) nmi_detected = true;
        else nmi_detected = false;
#else
        if (ppu->nmi_out) {
            ppu->nmi_out = false;
            cpu->nmi();
        }
#endif
    }
    
    bool MainBus::read(Word addr, Byte &data){
        switch ( (addr & 0xf000) >> 12){
        case 0x0 :
        case 0x1 :
            {
                data = ram[addr & 0x7ff];
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
                //no read from $4014;
                if (addr >= 0x4016 && addr <= 0x4017) {
                    data = (controller_state[addr & 0x1] & 0x80) > 0;
                    controller_state[addr & 0x1] <<= 1;
                    return true;
                }
                data = NULL;
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
        switch ( (addr & 0xf000) >> 12){
        case 0x0 :
        case 0x1 :
            {
                ram[addr & 0x7ff] = data;
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
                if (addr == 0x4014){
                    //writing to $4014 will trigger OAM DMA;
                    ppu->oam_addr = 0;//is this clear effective?
                    cpu->oam_halt(data);
                }
                else if (addr >= 0x4016 && addr <= 0x4017) {
                    //Write 1 to $4016 to signal the controller to poll its input
                    //Write 0 to $4016 to finish the poll
                    controller_state[addr & 0x1] = controller[addr & 0x1];
                    return true;
                }

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

    void MainBus::oam_transfer(Byte data){
        ppu->oam[ppu->oam_addr] = data;
        ++ppu->oam_addr;
    }

    //void MainBus::DMC_DMA(){
    //    //Transfer: DMC DMA copies a single byte to the DMC unit's sample buffer. 
    //    //Where is it?
    //
    //    //TODO: how to trigger a DMC_DMA() ?
    //    //This occurs automatically after the DMC enable bit, bit 4, 
    //    //of the sound channel enable register ($4015) is set to 1, 
    //    //which starts DPCM sample playback using the current DMC 
    //    //settings in registers $4010-4013. 
    //    
    //    //??
    //    //DMC DMA is scheduled when all of DPCM playback is enabled, 
    //    //there are bytes left in the sample, and the sample buffer 
    //    //is empty (see Memory reader and Output unit). 
    //
    //    //TODO: how many cycles it takes?
    //    //DMC DMA normally takes 3 or 4 cycles;
    //    //In the common cases, DMC DMA performs a halt cycle, 
    //    //a dummy cycle, an optional alignment cycle, and a get.
    //
    //    cpu->dma_cycles(3);
    //}
    
    void MainBus::reset(){
        sys_clock = 0;
        ram.reset();
        ppu->reset();
        //irq_detected = false;
        //nmi_detected = false;
    }
}; // end nes;