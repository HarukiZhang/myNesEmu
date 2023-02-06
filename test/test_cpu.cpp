#include <iostream>
#include <sstream>

#include "CPU.h"
#include "PPU.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

constexpr int kRIGHT_COL = 330;

class Demo : public olc::PixelGameEngine {
public:
	Demo() { sAppName = "myNesEmu_test_cpu"; }
	std::string hex(uint32_t num, uint8_t radix) {
		std::string str(radix, '0');//fill constructor: fill str with radix number of '0';
		for (int i = radix - 1; i >= 0; --i) {
			str[i] = "0123456789ABCDEF"[num & 0xf];
			num >>= 4;
		}
		return str;
	}
	bool OnUserCreate() override {
		if (cart.load_test_rom("D:\\haruk\\Projects\\nesEmu\\ROMs\\test_roms\\01-implied.nes")) {
			std::clog << "Cartridge loading success" << std::endl;
		}
		else {
			std::clog << "Cartridge loading failed" << std::endl;
			return false;
		}
		mapp = nes::Mapper::create_mapper(cart);
		if (mapp) std::clog << "Mapper creation success" << std::endl;
		else {
			std::clog << "Mapper creation failed" << std::endl;
			return false;
		}
		mbus.connect(&cpu, &ppu, mapp);
		hbus.connect(mapp);
		ppu.connect(&hbus);

		map_asm = cpu.disassemble(0x0000, 0xFFFF);

		mbus.reset();
		std::clog << "Engine creation success" << std::endl;

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override {
		Clear(olc::DARK_BLUE);

		// The final result is displayed and also written to $6000.
		// 
		// Before the test starts, $80 is written there so you can tell when it's done. 
		// 
		// If a test needs the NES to be reset, it writes $81 there.
		// 
		// In addition, $DE $B0 $G1 is written to $6001 - $6003 to allow an emulator to detect when a test is
		// being run, as opposed to some other NES program.

		if (GetKey(olc::Key::SPACE).bPressed) bEmuRun = !bEmuRun;
		if (GetKey(olc::Key::R).bPressed) cpu.reset();
		if (GetKey(olc::Key::I).bPressed) cpu.irq();
		if (GetKey(olc::Key::N).bPressed) cpu.nmi();

		//Test related:
		if (cart.get_prg_ram(0) == 0x81) cpu.reset();
		if (!bTestStart &&  cart.get_prg_ram(0) == 0x80) {
			std::clog << "Test begins at " << global_counter << "cycles" << std::endl;
			bTestStart = true;
		}
		if (cart.get_prg_ram(1) == 0xDE && cart.get_prg_ram(2) == 0xB0)
			DrawString(2, 2, "Test is running ...", olc::WHITE, 2);
		else if (bTestStart)
			std::clog << "Test is done." << std::endl;
		
		//General running:
		if (bEmuRun) {
			do { cpu.clock(); } while (!cpu.complete());
			++global_counter;
		}
		else if (GetKey(olc::Key::C).bPressed) {//step-by-step;
			do { cpu.clock(); } while (!cpu.complete());
			++global_counter;
		}

		// Draw Ram Page 0x00		
		DrawCpu(kRIGHT_COL, 2);
		DrawCode(kRIGHT_COL, 72, 26);

		return true;
	}
	void DrawCpu(int x, int y) {
		//std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", cpu.P.val & nes::FLAG::N ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", cpu.P.val & nes::FLAG::V ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", cpu.P.val & nes::FLAG::U ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", cpu.P.val & nes::FLAG::B ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", cpu.P.val & nes::FLAG::D ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", cpu.P.val & nes::FLAG::I ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", cpu.P.val & nes::FLAG::Z ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", cpu.P.val & nes::FLAG::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(cpu.PC, 4));
		DrawString(x, y + 20, "A: $" + hex(cpu.A, 2) + "  [" + std::to_string(cpu.A) + "]");
		DrawString(x, y + 30, "X: $" + hex(cpu.X, 2) + "  [" + std::to_string(cpu.X) + "]");
		DrawString(x, y + 40, "Y: $" + hex(cpu.Y, 2) + "  [" + std::to_string(cpu.Y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(cpu.S, 4));
	}
	void DrawCode(int x, int y, int nLines) {
		auto it_a = map_asm.find(cpu.PC);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != map_asm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != map_asm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = map_asm.find(cpu.PC);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != map_asm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != map_asm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}
private:
	nes::CPU cpu;
	nes::PPU ppu;
	nes::MainBus mbus;
	nes::HybridBus hbus;
	nes::Cartridge cart;
	std::shared_ptr<nes::Mapper> mapp;

	std::map<nes::Word, std::string> map_asm;

	bool bEmuRun = false;
	size_t global_counter = 0;

	bool bTestStart = false;
};

Demo demo;

int main() {
	demo.Construct(520, 300, 1, 1);
	demo.Start();
	return 0;
}