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

nes::CPU cpu;
nes::PPU ppu;
nes::MainBus mbus;
nes::HybridBus hbus;
nes::Cartridge cart;
std::shared_ptr<nes::Mapper> mapp;

class Demo : public olc::PixelGameEngine {
public:
	Demo() { sAppName = "LearnPGE"; }
private:
	bool OnUserCreate() override {
		if (!cart.load_file("D:\\haruk\\Projects\\nesEmu\\ROMs\\SuperMarioBros.nes"))
			return false;
		mapp = nes::Mapper::create_mapper(cart);
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


		return true;

	}
private:

	bool bEmulationRun = false;

	float fResidualTime = 0.0f;

};

int main() {



	int pause = 0;

	return 0;

}