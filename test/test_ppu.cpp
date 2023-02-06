#include<iostream>
#include <sstream>
#include<memory>
#include<cstdint>

#include "CPU.h"
#include "PPU.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using Byte = uint8_t;
using Word = uint16_t; 

class Demo : public olc::PixelGameEngine {
public:
	Demo() { sAppName = "myNesEmu_test_ppu"; }
private:
	bool OnUserCreate() override {
		if (cart.load_file("D:\\haruk\\Projects\\nesEmu\\ROMs\\SuperMarioBros.nes")) {
			std::clog << "cartridge loading success" << std::endl;
		}
		else {
			std::clog << "cartridge loading failed" << std::endl;
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

		mbus.reset();

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override {
		Clear(olc::DARK_BLUE);

		if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(olc::Key::R).bPressed) mbus.reset();

		if (bEmulationRun) {
			if (fResidualTime > 0.0f)
				fResidualTime -= fElapsedTime;
			else
			{
				fResidualTime += (1.0f / 60.0f) - fElapsedTime;
				do { mbus.clock(); } while (!ppu.frame_complete);
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
				// complete for additional system clock cycles. Drain
				// those out
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

		DrawSprite(0, 0, &ppu.get_screen(), 1);

		return true;

	}
private:

	bool bEmulationRun = false;

	float fResidualTime = 0.0f;

	nes::CPU cpu;
	nes::PPU ppu;
	nes::MainBus mbus;
	nes::HybridBus hbus;
	nes::Cartridge cart;
	std::shared_ptr<nes::Mapper> mapp;

};

/*
1>try1.obj : error LNK2019: 无法解析的外部符号
"public: bool __thiscall nes::CPU::complete(void)"，
函数 "private: virtual bool __thiscall Demo::OnUserUpdate(float)" 中引用了该符号

1>HybridBus.obj : error LNK2019: 无法解析的外部符号
"public: unsigned short __thiscall nes::Mapper::get_nt_mirror(unsigned short)" ，
函数 "public: bool __thiscall nes::HybridBus::read(unsigned short,unsigned char &)" 中引用了该符号

1>Mapper.obj : error LNK2019: 无法解析的外部符号
"public: struct nes::_NESHeader const & __thiscall nes::Cartridge::get_header(void)"，
函数 "public: static class std::shared_ptr<class nes::Mapper> __cdecl nes::Mapper::create_mapper(class nes::Cartridge &)"
中引用了该符号

1>Mapper_NROM.obj : error LNK2001: 无法解析的外部符号
"public: struct nes::_NESHeader const & __thiscall nes::Cartridge::get_header(void)"

*/

Demo demo;

int main() {

	demo.Construct(320, 240, 1, 1);
	demo.Start();

	return 0;

}