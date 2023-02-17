#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <memory>
#include <string>
#include <chrono>
#include <ctime>

#include "CPU.h"
#include "PPU.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using Byte = uint8_t;
using Word = uint16_t; 
using SPAN = std::chrono::duration<double>;
using TP = std::chrono::steady_clock::time_point;
using SCLK = std::chrono::steady_clock;

constexpr int kRIGHT_COL = 518;


class Demo : public olc::PixelGameEngine {
public:
	Demo() { sAppName = "myNesEmu_test_ppu"; }
private:
	bool OnUserCreate() override {
		if (cart.load_file("D:\\haruk\\Projects\\nesEmu\\ROMs\\IceClimber.nes")) {
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
		std::clog << "Mapper usecount : " << mapp.use_count() << std::endl;
		
		map_asm = cpu.disassemble(0x8000, 0xFFFF);

		mbus.reset();
		cpu.reset(false);//hard reset;

		std::clog << "Initiation : complete" << std::endl;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override {
		Clear(olc::DARK_BLUE);

		if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(olc::Key::R).bPressed) cpu.reset(true);//soft reset;
		if (GetKey(olc::Key::I).bPressed) mbus.set_irq();
		if (GetKey(olc::Key::N).bPressed) mbus.set_nmi();
		if (GetKey(olc::Key::P).bPressed) pal_sel = (pal_sel + 1) & 0x7;

		if (bEmulationRun) {
			if (fResidualTime > 0.0f)
				fResidualTime -= fElapsedTime;
			else
			{
				fResidualTime += (1.0f / 60.0f) - fElapsedTime;
				do { 
					mbus.clock(); 
				} while (!ppu.frame_complete);
				ppu.frame_complete = false;
			}
		}
		else {
			// Emulate code step-by-step
			if (GetKey(olc::Key::C).bPressed)
			{
				// Clock enough times to execute a whole CPU instruction
				do { mbus.clock(); } while (!cpu.complete());

				// CPU clock runs slower than system clock, so it may be
				// complete for additional system clock cycles. Drain those out
				do { mbus.clock(); } while (cpu.complete());
			}

			// Emulate one whole frame
			if (GetKey(olc::Key::F).bPressed)
			{
				// Clock enough times to draw a single frame
				do { mbus.clock(); } while (!ppu.frame_complete);
				// Use residual clock cycles to complete current instruction
				do { mbus.clock(); } while (!cpu.complete());
				// Reset frame completion flag
				ppu.frame_complete = false;
			}
		}

		DrawSprite(0, 0, &ppu.get_screen(), 2);

		//DrawCpu(kRIGHT_COL, 2);
		//DrawCode(kRIGHT_COL, 72, 16);
		const int swatch_size = 6;
		for (int pal = 0; pal < 8; ++pal) {
			for (int clr = 0; clr < 4; ++clr) {
				FillRect(
					kRIGHT_COL + pal * (swatch_size * 5) + clr * swatch_size,
					140,
					swatch_size,
					swatch_size,
					ppu.get_color(pal, clr)
				);
			}
		}
		DrawRect((kRIGHT_COL + pal_sel * (swatch_size * 5) - 1), 139, (4 * swatch_size), swatch_size);


		olc::Sprite& temp_spr = ppu.get_pattern_table(1, pal_sel);
		DrawSprite(kRIGHT_COL, 5, &ppu.get_pattern_table(0, pal_sel), 1);
		DrawSprite(kRIGHT_COL + 130, 5, &temp_spr, 1);

		//for (uint8_t y = 0; y < 30; ++y) {
		//	for (uint8_t x = 0; x < 32; ++x) {
		//		uint8_t id = ppu.vram[y * 32 + x];
		//		DrawPartialSprite(x * 16, y * 16, &temp_spr, (id & 0x0f) << 3, ((id >> 4) & 0x0f) << 3, 8, 8, 2);
		//	}
		//}

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
		std::map<nes::Word, std::string>::iterator it_a, it_t;
		it_t = map_asm.find(cpu.PC);
		if (it_t == map_asm.end()) return;
		if (it_t != map_asm.begin()) --it_t;
		int nLineY = (nLines >> 1) * 10 + y;
		it_a = it_t;
		DrawString(x, nLineY, (*it_a).second, olc::CYAN);//instr to be executed;
		while (nLineY < (nLines * 10) + y)
		{
			nLineY += 10;
			if (++it_a != map_asm.end())
			{
				DrawString(x, nLineY, (*it_a).second);
			}
		}
		it_a = it_t;
		nLineY = (nLines >> 1) * 10 + y;
		while (nLineY > y)
		{
			nLineY -= 10;
			if (--it_a != map_asm.end())
			{
				DrawString(x, nLineY, (*it_a).second);
			}
		}
	}
	std::string hex(uint32_t n, uint8_t r) {
		std::string s(r, '0');
		for (int i = r - 1; i >= 0; --i, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xf];
		return s;
	}
private:

	bool bEmulationRun = false;

	float fResidualTime = 0.0f;
	size_t global_counter = 0;
	uint8_t pal_sel = 0;

	nes::CPU cpu;
	nes::PPU ppu;
	nes::MainBus mbus;
	nes::Cartridge cart;
	std::shared_ptr<nes::Mapper> mapp;

	std::map<nes::Word, std::string> map_asm;


};

TP t0, t1;
SPAN span;

Demo demo;


int main() {

	demo.Construct(780, 520, 1, 1);
	demo.Start();

	return 0;

}