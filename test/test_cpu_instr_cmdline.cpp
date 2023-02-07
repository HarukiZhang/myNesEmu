#include <cstdio>
#include <iostream>
#include <fstream>
#include <chrono>
#include <queue>
#include <string>

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
std::queue<std::string> trace_que;

constexpr int MAX_TRACE_LINES = 0xFFFF;
constexpr int MAX_INSTR_LINES = 0xFFFFF;

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

inline bool initiate(const char* file_path){
	if (cart.load_test_rom(file_path)) {
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
		if (cart.get_prg_ram(1) == 0xDE && cart.get_prg_ram(2) == 0xB0 && cart.get_prg_ram(3) == 0x61) {
			std::clog << "Test is running ..." << std::endl;
			phase = PHASE::testRunning;
		}
		break;
	case PHASE::testRunning:
		if (cart.get_prg_ram(0) != 0x80) {
			std::clog << "Test is done." << std::endl;
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

//print instructions per sec;
inline void print_ips() {
	span = std::chrono::duration_cast<SPAN>(t1 - t0);
	if (1.0 - span.count() < 0.0) {
		std::cout << "i" << std::setw(10) << global_counter << " : " << std::setw(10) << (global_counter - cycles) << " fps";
		std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		t0 = t1;
		cycles = global_counter;
	}
}

inline bool log_trace() {
	if (trace_que.size() >= MAX_TRACE_LINES)
		return true;//Emu should stop due to trace limit reaches;
	auto it = map_asm.find(cpu.PC);
	if (it != map_asm.end()) {
		trace_que.push(it->second);
	}
	//no else;
	return false;//Emu could continue;
}

inline void output_trace(const char* o_path) {
	if (!trace_que.empty()) {
		std::ofstream ofs{ o_path, std::ios_base::trunc };
		if (ofs.is_open()) {
			while (!trace_que.empty()) {
				ofs << trace_que.front() << std::endl;
				trace_que.pop();
			}
		}
		ofs.close();
	}
}

int main() {
	FILE* redef;
	freopen_s(&redef, "D:\\haruk\\Projects\\nesEmu\\myNESEmu\\test\\test_cpu_instr.log", "a", stderr);
	std::clog << std::endl;
	std::clog << "============================================" << std::endl;
	std::clog << "Test: 11-special" << std::endl;
	
	
	if (!initiate("D:\\haruk\\Projects\\nesEmu\\ROMs\\test_roms\\rom_singles\\11-special.nes"))
		return 0;

	bool bTraceStop = false;
	t0 = SCLK::now();
	while (true) {
		if (doClock()) break;
		t1 = SCLK::now();
		//if (phase == PHASE::testRunning)
		//	bTraceStop = log_trace();
		//if (bTraceStop) {
		//	std::clog << "Test stopped due to trace limit reaches." << std::endl;
		//	break;
		//}
		print_ips();

		//if (global_counter >= 0xfffff) {
		//	std::clog << "Test stopped due to number of instruction limit ($"
		//		<< std::hex << MAX_INSTR_LINES << ") reaches." << std::endl;
		//	break;
		//}
	}

	//output_trace("D:\\haruk\\Projects\\nesEmu\\myNESEmu\\test\\trace_log.log");

	if (redef) fclose(redef);
	return 0;
}