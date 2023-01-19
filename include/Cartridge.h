#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include<vector>

#include "NESDef.h"

namespace nes {

    class Cartridge {
    public:
        Cartridge();
        Cartridge(char *file_path);
        ~Cartridge();
        bool read_prg_rom(Word addr, Byte &data);
        bool read_chr_rom(Word addr, Byte &data);
    private:
        bool load_file(char *file_path);
        void print_info_v_iNES();

    public:
        struct NESHeader {
            char NES1A[4];           //Btye 0-3
            uint8_t num_prg_rom;     //Btye 4
            uint8_t num_chr_rom;     //Byte 5
            // uint8_t rom_ctrl;
            union {                  //Byte 6
                struct {
                    uint8_t mirror_hv : 1;//0:horizontal; 1:vertical;
                    uint8_t save_ram : 1;
                    uint8_t trainer : 1;
                    uint8_t four_screen : 1;//within cart provide extra VRAM;
                    uint8_t n_mapper_low : 4;
                };
                uint8_t val;
            } byte_6;
            union {                  //Byte 7
                struct {
                    uint8_t vs_unisys : 1;
                    uint8_t playchoice : 1;
                    uint8_t nes_2_sign : 2;
                    uint8_t n_mapper_high : 4;
                };
                uint8_t val;
            } byte_7;
            uint8_t num_prg_ram;     //Byte 8
            char tailBytes[7];       //Byte 9-15
        } header;

    private:
        std::vector<Byte> prg_rom;
        std::vector<Byte> chr_rom;
    };

};//end nes

#endif