#include<iostream>

#include "CPU.h"
#include "PPU.h"
#include "MainBus.h"

namespace nes {

    MainBus::MainBus(){}

    void MainBus::connect(CPU *_cpu, PPU *_ppu, std::shared_ptr<Mapper> &_mapp){
        cpu = _cpu;
        cpu->connect(this);
        ppu = _ppu;
        mapper = _mapp;
    }

    bool MainBus::read(Word addr, Byte &data){
        if (cpu_halt){//cpu allows halting only when read cycles;
            OAM_DMA();
        }
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
                if (addr < 0x4020){
                    data = io_regs[addr & 0xff];
                    return true;
                }
                else {
                    //TODO: Expansion ROM
                    return false;
                }
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
        //records consecutive write cycles before cpu halting successes;
        ++extra_dma_cycles;
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
                if (addr < 0x4020){
                    io_regs[addr & 0xff] = data;

                    //writing to $4014 will trigger DMA;
                    if (addr == 0x4014){
                        //
                        if (cpu_halt == false) extra_dma_cycles = 0;
                        cpu_halt = true;
                    }

                    return true;
                }
                else {
                    //TODO: Expansion ROM
                    return false;
                }
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

    void MainBus::OAM_DMA(){
        Word addr = io_regs.oam_dma << 8;// ($4014) * $100;
        Byte oam_cycles = 0x201;//512 access cycles + one halt cycle;
        std::memcpy(&ppu->oam[0], &ram[addr], 0x100);
        
        //TODO: condition of: the optional alignment cycle;
        //OAM DMA on its own takes 513 or 514 cycles,
        //that is: exclude cycles before halt success ?

        cpu->dma_cycles(oam_cycles);

        cpu_halt = false;//close;

        //TODO: how to interact when encountered DMC DMA?
        return;
    }

    void MainBus::DMC_DMA(){
        //Transfer: DMC DMA copies a single byte to the DMC unit's sample buffer. 
        //Where is it?

        //TODO: how to trigger a DMC_DMA() ?
        //This occurs automatically after the DMC enable bit, bit 4, 
        //of the sound channel enable register ($4015) is set to 1, 
        //which starts DPCM sample playback using the current DMC 
        //settings in registers $4010-4013. 
        
        //??
        //DMC DMA is scheduled when all of DPCM playback is enabled, 
        //there are bytes left in the sample, and the sample buffer 
        //is empty (see Memory reader and Output unit). 

        //TODO: how many cycles it takes?
        //DMC DMA normally takes 3 or 4 cycles;
        //In the common cases, DMC DMA performs a halt cycle, 
        //a dummy cycle, an optional alignment cycle, and a get.

        cpu->dma_cycles(3);
    }
    void MainBus::clock(){
        cpu->clock();
        
        ppu->clock();
        check_nmi();
        ppu->clock();
        check_nmi();
        ppu->clock();
        check_nmi();


        ++sys_clock;
    }

    inline void MainBus::check_nmi(){
        if (ppu->nmi_out){
            ppu->nmi_out = false;
            cpu->nmi();
        }
    }
}; // end nes;