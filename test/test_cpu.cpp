#include <fstream>
#include "MainBus.h"

nes::CPU cpu;
nes::MainBus bus;
nes::RAM ram;

uint8_t cspace[0x8000]{};

int main(){
    cpu.connect(&bus);
    bus.connect(&cpu, &ram);
    bus.cart_space = cspace;

    //reset vector;
    cspace[0xFFFC - 0x8000] = 0x00;
    cspace[0xFFFD - 0x8000] = 0x80;

    //irq vector;
    cspace[0xFFFE - 0x8000] = 0x00;
    cspace[0xFFFF - 0x8000] = 0xC0;

    std::ifstream fs;
    fs.open("D:\\haruk\\Projects\\nesEmu\\myNESEmu\\test\\test_cpu.nes", std::ios_base::in | std::ios_base::binary);
    fs.seekg(0x10, fs.beg);
    fs.read(reinterpret_cast<char*>(cspace), 0x2F);
    fs.close();

    std::ofstream ofstm;
    ofstm.open("./test_cpu.log", std::ios_base::out);

    cpu.reset();
    cpu.clear_cycle();

    while (ram.inter_ram[0x5ff] == 0){
        cpu.clock();
        cpu.print_status(ofstm);
        cpu.clear_cycle();
    }
    ofstm.close();
    
    return 0;
}