#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include<vector>

#include "NESDef.h"

namespace nes {

    class Cartridge {
    public:
        Cartridge();
        Cartridge(const char *file_path);
        bool load_file(const char* file_path);

        bool read_prg_rom(Word addr, Byte &data);
        bool read_chr_rom(Word addr, Byte &data);
        bool read_prg_ram(Word addr, Byte &data);
        bool write_prg_ram(Word addr, Byte data);

        const NESHeader &get_header();
    private:
        void print_info_v_iNES();

    private:
        NESHeader header;
        std::vector<Byte> prg_rom;
        std::vector<Byte> chr_rom;
        std::vector<Byte> prg_ram;
    };

};//end nes

#endif