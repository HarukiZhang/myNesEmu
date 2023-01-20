#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include<vector>

#include "NESDef.h"

namespace nes {

    class Cartridge {
    public:
        Cartridge();
        Cartridge(const char *file_path);
        ~Cartridge();
        bool read_prg_rom(Word addr, Byte &data);
        bool read_chr_rom(Word addr, Byte &data);
        bool read_cart_ram(Word addr, Byte &data);
        bool write_cart_ram(Word addr, Byte data);
    private:
        bool load_file(const char *file_path);
        void print_info_v_iNES();

    public:
        NESHeader header;

    private:
        std::vector<Byte> prg_rom;
        std::vector<Byte> chr_rom;
        std::vector<Byte> cart_ram;
    };

};//end nes

#endif