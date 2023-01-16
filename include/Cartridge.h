#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include<sys/stat.h>
#include<fstream>

#include "NESDef.h"

namespace nes {

    //forward declaration;
    class Mapper;

    class Cartridge {
    public:
        Cartridge();
        ~Cartridge();
        bool read_prg_rom(Word addr, Byte &data);
        bool read_chr_rom(Word addr, Byte &data);
        void set_mapper();
        inline Mapper *get_mapper();
    private:
        bool load_file();

        void read_header();

        void load_mapper();

        void print_info_v_iNES();

    public:
        struct NESHeader {
            char NES1A[4];           //Btye 0-3
            uint8_t num_prg_rom;     //Btye 4
            uint8_t num_chr_rom;     //Byte 5
            // uint8_t rom_ctrl;
            union {                  //Byte 6
                struct {
                    uint8_t mirror_hv : 1;
                    uint8_t save_ram : 1;
                    uint8_t trainer : 1;
                    uint8_t four_screen : 1;
                    uint8_t n_mapper_low : 4;
                };
                uint8_t val;
            } byte_6;
            union {                  //Byte 7
                struct {
                    uint8_t vs_unisys : 1;
                    uint8_t playchoice : 1;
                    uint8_t nes_2_sign : 2;
                    uint8_t n_mapper_up : 4;
                };
                uint8_t val;
            } byte_7;
            uint8_t num_prg_ram;     //Byte 8
            char tailBytes[7];       //Byte 9-15
        };

    private:
        char *rom_path;
        std::ifstream rom_stream;
        struct stat statbuf;
        NESHeader header;
        Mapper *mapper;
    };

};//end nes

#endif