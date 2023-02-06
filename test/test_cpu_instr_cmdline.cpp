#include <iostream>
#include <chrono>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "PPU.h"
#include "CPU.h"

using SPAN = std::chrono::duration<double>;
using TP = std::chrono::steady_clock::time_point;
using SCLK = std::chrono::steady_clock;

nes::CPU cpu;
nes::PPU ppu;
nes::MainBus mbus;
nes::HybridBus hbus;
nes::Cartridge cart;
std::shared_ptr<nes::Mapper> mapp;

std::map<nes::Word, std::string> map_asm;

TP t0, t1;
SPAN span;

enum PHASE {
	initiation,
	testStart,
	waitToLoad,
	testRunning,
	testDone,
};

PHASE phase = PHASE::initiation;
size_t global_counter = 0;
size_t cycles = 0;

inline bool initiate(){
	if (cart.load_test_rom("D:\\haruk\\Projects\\nesEmu\\ROMs\\test_roms\\01-implied.nes")) {
		std::clog << "Cartridge loading : success" << std::endl;
	}
	else {
		std::clog << "Cartridge loading : failed" << std::endl;
		return false;
	}
	mapp = nes::Mapper::create_mapper(cart);
	if (mapp) std::clog << "Mapper creation : success" << std::endl;
	else {
		std::clog << "Mapper creation : failed" << std::endl;
		return false;
	}
	mbus.connect(&cpu, &ppu, mapp);
	hbus.connect(mapp);
	ppu.connect(&hbus);

	map_asm = cpu.disassemble(0x0000, 0xFFFF);

	mbus.reset();
	std::clog << "Initiation : complete" << std::endl;
	phase = PHASE::testStart;
	return true;
}

inline bool doClock() {
	// The final result is displayed and also written to $6000.
	// Before the test starts, $80 is written there so you can tell when it's done. 
	// If a test needs the NES to be reset, it writes $81 there.
	// In addition, $DE $B0 $61 is written to $6001 - $6003 to allow an emulator to detect when a test is being run.
	bool ret = false;
	//Test related:
	if (cart.get_prg_ram(0) == 0x81) cpu.reset();

	switch (phase) {
	case PHASE::initiation:
		break;
	case PHASE::testStart:
		if (cart.get_prg_ram(0) == 0x80) {
			std::clog << "Test begins at " << global_counter << " cycles." << std::endl;
			phase = PHASE::waitToLoad;
		}
		break;
	case PHASE::waitToLoad:
		if (cart.get_prg_ram(1) == 0xDE && cart.get_prg_ram(2) == 0xB0 && cart.get_prg_ram(3) == 0x61)
			phase = PHASE::testRunning;
		break;
	case PHASE::testRunning:
		if (cart.get_prg_ram(1) != 0xDE || cart.get_prg_ram(2) != 0xB0 || cart.get_prg_ram(3) != 0x61) {
			phase = PHASE::testDone;
			return true;
		}
		break;
	case PHASE::testDone:
		break;
	default:
		break;
	}

	//General running:
	cpu.exe_instr();
	//do { cpu.clock(); } while (!cpu.complete());
	
	++global_counter;

	return ret;
}

inline void print_fps() {
	span = std::chrono::duration_cast<SPAN>(t1 - t0);
	if (1.0 - span.count() < 0.0) {
		std::clog << (global_counter - cycles) << " fps";
		std::clog << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		t0 = t1;
		cycles = global_counter;
	}
}

int main() {
	initiate();
	t0 = SCLK::now();
	while (true) {
		if (doClock()) return 0;
		t1 = SCLK::now();
		print_fps();
	}
	return 0;
}