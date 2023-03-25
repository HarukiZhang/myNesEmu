#include <iostream>
#include <fstream>
#include <cstdint>
#include <memory>

#include "CPU.h"
#include "PPU.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Demo : public olc::PixelGameEngine {
public:
	static constexpr int kSTATUS_BAR_X = 10;
	static constexpr int kSTATUS_BAR_Y = 480 - 25;

	Demo(){ sAppName = "myNesEmu_demo"; }
	void set_path(const char* file_path) {
		_rom_file_path = file_path;
	}
private:
	bool OnUserCreate() override {

		if (cart.load_file(_rom_file_path)) {
			std::clog << "Demo::OnUserCreate : Cartridge loading : success" << std::endl;
		}
		else {
			std::clog << "Demo::OnUserCreate : Cartridge loading : failed" << std::endl;
			return false;
		}
		mapp = nes::Mapper::create_mapper(cart);
		if (mapp) std::clog << "Demo::OnUserCreate : Mapper creation : success" << std::endl;
		else {
			std::clog << "Demo::OnUserCreate : Mapper creation : failed" << std::endl;
			return false;
		}
		mbus.connect(&cpu, &ppu, mapp);
		std::clog << "Demo::OnUserCreate : Mapper usecount : " << mapp.use_count() << std::endl;
		
		mbus.reset();
		cpu.reset(false);//hard reset;

		std::clog << "Demo::OnUserCreate : Initiation : complete" << std::endl;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override {
		Clear(olc::DARK_BLUE);

		mbus.controller[0] = 0x0;
		mbus.controller[0] |= GetKey(olc::Key::K).bHeld ? nes::Button::A : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::J).bHeld ? nes::Button::B : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::SHIFT).bHeld ? nes::Button::SELECT : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::ENTER).bHeld ? nes::Button::START : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::W).bHeld ? nes::Button::UP : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::S).bHeld ? nes::Button::DOWN : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::A).bHeld ? nes::Button::LEFT : 0x00;
		mbus.controller[0] |= GetKey(olc::Key::D).bHeld ? nes::Button::RIGHT : 0x00;

		if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::R).bPressed) cpu.reset(true);//soft reset;

		//adjust emulator speed;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::COMMA).bPressed) fVariableFrameRate += 0.001f;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::PERIOD).bPressed) fVariableFrameRate -= 0.001f;
		if (GetKey(olc::Key::CTRL).bHeld && GetKey(olc::Key::M).bPressed) fVariableFrameRate = 0.0f;

		DrawSprite(0, 0, &ppu.get_screen(), 2);//composite;


		if (bEmulationRun) {
			if (fFadeOutTime > 0.0f) {
				DrawString(kSTATUS_BAR_X, kSTATUS_BAR_Y, "Auto Run", olc::WHITE, 2);
				fFadeOutTime -= fElapsedTime;
			}
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
			fFadeOutTime = 2.0f;
			DrawString(kSTATUS_BAR_X, kSTATUS_BAR_Y, "Pause", olc::WHITE, 2);
		}

		return true;

	}
private:

	//_gfs::path _rom_file_path;
	const char* _rom_file_path = nullptr;

	bool bEmulationRun = false;

	float fResidualTime = 0.0f;
	float fVariableFrameRate = 0.0f;

	float fFadeOutTime = 0.0f;

	nes::CPU cpu;
	nes::PPU ppu;
	nes::MainBus mbus;
	nes::Cartridge cart;
	std::shared_ptr<nes::Mapper> mapp;

};


int main(int argc, char* argv[]) {

	Demo demo;
	if (argc == 1) {
		char path_buf[512]{};
		std::cout << "Please enter rom file path in the console." << std::endl;
		std::cin.getline(path_buf, 512);
		if (path_buf[0] == '\"') {
			int i = 1;
			while (path_buf[i] != 0 && path_buf[i] != '\"') ++i;
			path_buf[i] = 0;
			demo.set_path(&path_buf[1]);
		}
		else {
			demo.set_path(path_buf);
		}
	}
	else if (argc == 2) {
		demo.set_path(argv[1]);
	}
	else {
		std::cout << "Arguments are too much." << std::endl;
		exit(0);
		return 0;
	}

	demo.Construct(256 * 2, 240 * 2, 1, 1);
	demo.Start();

	return 0;

}