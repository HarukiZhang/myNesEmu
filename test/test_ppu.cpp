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

TP t0, t1;
SPAN span;
size_t frame_counter = 0;
double frame_sum = 0;

class Demo : public olc::PixelGameEngine {
public:
	static constexpr int kRIGHT_COL = 518;
	static constexpr int kSTATUS_BAR_X = 10;
	static constexpr int kFPS_BAR_X = 600;
	static constexpr int kSTATUS_BAR_Y = 730;
	static constexpr int kOAM_N_LINE = 30;

	Demo() { sAppName = "myNesEmu_test_ppu"; }
private:
	bool OnUserCreate() override {
		/*
			D:\\haruk\\Projects\\nesEmu\\ROMs\\test_roms\\nestest\\nestest.nes
			D:\\haruk\\Projects\\nesEmu\\ROMs\\test_roms\\sprite_hit_tests\\01.basics.nes
			D:\\haruk\\Projects\\nesEmu\\ROMs\\SMB.nes

			SMB             OK
			IceClimber      OK
			Macross         Glitch behind status bar;
			Xevious
			10-YardFight    OK
			DonkeyKong3     OK
			DonkeyKongJr    OK
			DuckHunt        No controller
			Excitebike      OK
			F1Race          Glitch on Route
			KungFu          Separate field
			Millipede       OK
			Pac-Man         OK
			Pinball         OK
		*/

		if (cart.load_file("D:\\haruk\\Projects\\nesEmu\\ROMs\\Macross.nes")) {
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

		mbus.controller[0] = 0x0;
		mbus.controller[0] |= GetKey(olc::Key::J).bHeld ? nes::Button::A : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::K).bHeld ? nes::Button::B : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::SHIFT).bHeld ? nes::Button::SELECT : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::ENTER).bHeld ? nes::Button::START : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::W).bHeld ? nes::Button::UP : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::S).bHeld ? nes::Button::DOWN : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::A).bHeld ? nes::Button::LEFT : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::D).bHeld ? nes::Button::RIGHT : 0x00;

		if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::R).bPressed) cpu.reset(true);//soft reset;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::P).bPressed) pal_sel = (pal_sel + 1) & 0x7;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::G).bPressed) bGridOn = !bGridOn;

		//adjust emulator speed;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::COMMA).bPressed) fVariableFrameRate += 0.001f;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::PERIOD).bPressed) fVariableFrameRate -= 0.001f;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::M).bPressed) fVariableFrameRate = 0.0f;

		if (bEmulationRun) {
			DrawString(kSTATUS_BAR_X, kSTATUS_BAR_Y, "Auto Run", olc::WHITE, 2);
			if (fResidualTime > 0.0f) {
				fResidualTime -= fElapsedTime;
				fResidualTime += fVariableFrameRate;
			}
			else
			{
				fResidualTime += (1.0f / 60.0f) - fElapsedTime;
				do { 
					mbus.clock();
				} while (!ppu.is_frame_complete());
				//clear fram_complete flag built-in;
			}
		}
		else {
			DrawString(kSTATUS_BAR_X, kSTATUS_BAR_Y, "Pause", olc::WHITE, 2);
			// Emulate code step-by-step
			if (GetKey(olc::Key::C).bPressed)
			{
				DrawString(kSTATUS_BAR_X, kSTATUS_BAR_Y, "By Step", olc::WHITE, 2);
				// Clock enough times to execute a whole CPU instruction
				do { mbus.clock(); } while (!cpu.complete());

				// CPU clock runs slower than system clock, so it may be
				// complete for additional system clock cycles. Drain those out
				do { mbus.clock(); } while (cpu.complete());
			}

			// Emulate one whole frame
			if (GetKey(olc::Key::F).bPressed)
			{
				DrawString(kSTATUS_BAR_X, kSTATUS_BAR_Y, "By Frame", olc::WHITE, 2);
				// Clock enough times to draw a single frame
				do { mbus.clock(); } while (!ppu.is_frame_complete());//reset frame_complete built-in;
				// Use residual clock cycles to complete current instruction
				do { mbus.clock(); } while (!cpu.complete());
			}
		}

		DrawSprite(0, 0, &ppu.get_screen(), 2);//composite;
		//DrawSprite(256 + 2, 0, &ppu.get_bkgr(), 1);//only for check bkgr;

		//Name Table:
		DrawSprite(0, 480 + 2, &ppu.get_name_table(0), 1);
		DrawSprite(256 + 2, 480 + 2, &ppu.get_name_table(1), 1);

		if (bGridOn) draw_grid(0, 0);
		//draw_cpu(kRIGHT_COL, 160);
		//draw_code(kRIGHT_COL, 232, 20);

		//Palette:
		draw_palette(kRIGHT_COL, 140, pal_sel);

		//Pattern Table:
		DrawSprite(kRIGHT_COL, 5, &ppu.get_pattern_table(0, pal_sel), 1);
		DrawSprite(kRIGHT_COL + 130, 5, &ppu.get_pattern_table(1, pal_sel), 1);

		//OAM entries:
		if (GetKey(olc::Key::UP).bPressed && oam_start_line > 0) --oam_start_line;
		if (GetKey(olc::Key::DOWN).bPressed && oam_start_line < (64 - kOAM_N_LINE)) ++oam_start_line;
		draw_oam(kRIGHT_COL, 160, oam_start_line, kOAM_N_LINE);

		//FPS:
		//draw_fps(kFPS_BAR_X, kSTATUS_BAR_Y);

		return true;

	}
	void draw_fps(int x, int y) {
		++frame_counter;
		std::string frame_str{ "Frames  : " };
		DrawString(x, y, frame_str + std::to_string(frame_counter));
		t1 = SCLK::now();
		span = std::chrono::duration_cast<SPAN>(t1 - t0);
		frame_str.clear();
		frame_str = "FPS     : ";
		double cur_frame = 1.0 / span.count();
		frame_sum += cur_frame;
		frame_str += std::to_string(static_cast<int>(cur_frame));
		DrawString(x, y + 10, frame_str);
		frame_str.clear();
		frame_str = "Mean FPS: ";
		frame_str += std::to_string(frame_sum / frame_counter);
		for (Byte i = 0; frame_str[i]; ++i) {
			if (frame_str[i] == '.') {
				frame_str[i + 4] = 0;
			}
		}
		DrawString(x, y + 20, frame_str);
		t0 = t1;
	}
	void draw_grid(int x, int y) {
		for (int i = 0; i <= 32; ++i)
			DrawLine(x + i * 16, y, x + i * 16, y + 480, olc::RED);
		for (int i = 0 ; i <= 30; ++i)
			DrawLine(x, y + i * 16, x + 512, y + i * 16, olc::RED);
	}
	void draw_palette(int x, int y, int idx) {
		const int swatch_size = 6;
		for (int pal = 0; pal < 8; ++pal) {
			for (int clr = 0; clr < 4; ++clr) {
				FillRect(
					x + pal * (swatch_size * 5) + clr * swatch_size, y,
					swatch_size, swatch_size,
					ppu.get_color(pal, clr)
				);
			}
		}
		DrawRect((x + idx * (swatch_size * 5) - 1), y - 1, (4 * swatch_size), swatch_size);
	}
	void draw_oam(int x, int y, int nStart, int nLines) {
		DrawString(x, y, "#  POS      ID  VHP    PL");
		for (int ent = nStart; (ent < (nStart + nLines)) && ent < 64; ++ent) {
			DrawString(x, (ent - nStart + 1) * 10 + y, hex(ent,2) + " " + ppu.get_obj_attr_ent(ent));
		}
	}
	void draw_cpu(int x, int y) {
		//std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", cpu.P.val & nes::Negative ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", cpu.P.val & nes::Overflow ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", cpu.P.val & nes::U_bit ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", cpu.P.val & nes::B_bit ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", cpu.P.val & nes::D_Mode ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", cpu.P.val & nes::I_Flag ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", cpu.P.val & nes::Zero ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", cpu.P.val & nes::Carry ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(cpu.PC, 4));
		DrawString(x, y + 20, "A: $" + hex(cpu.A, 2) + "  [" + std::to_string(cpu.A) + "]");
		DrawString(x, y + 30, "X: $" + hex(cpu.X, 2) + "  [" + std::to_string(cpu.X) + "]");
		DrawString(x, y + 40, "Y: $" + hex(cpu.Y, 2) + "  [" + std::to_string(cpu.Y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(cpu.S, 4));
	}
	void draw_code(int x, int y, int nLines) {
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
	bool bGridOn = false;

	float fResidualTime = 0.0f;
	float fVariableFrameRate = 0.0f;
	size_t global_counter = 0;
	uint8_t pal_sel = 0;
	uint8_t oam_start_line = 0;

	nes::CPU cpu;
	nes::PPU ppu;
	nes::MainBus mbus;
	nes::Cartridge cart;
	std::shared_ptr<nes::Mapper> mapp;

	std::map<nes::Word, std::string> map_asm;


};

Demo demo;


int main() {
	t0 = SCLK::now();
	demo.Construct(780, 780, 1, 1);
	demo.Start();

	return 0;

}