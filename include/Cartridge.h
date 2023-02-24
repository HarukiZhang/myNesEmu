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

        bool load_file(const char* file_path);

        bool read_prg_rom(Phad addr, Byte &data);
        bool read_chr_rom(Phad addr, Byte &data);
        bool write_chr_ram(Phad addr, Byte data);
        bool read_prg_ram(Phad addr, Byte &data);
        bool write_prg_ram(Phad addr, Byte data);

        const NESHeader &get_header();

        //debug:
        bool load_test_rom(const char* file_path, bool create_prg_ram = true);
        Byte get_prg_ram(Word addr) {
            if (!prg_ram.empty()) {
                return prg_ram[addr];
            }
            return NULL;
        }

    private:
        void print_info_v_iNES();
        void load_content(std::ifstream& ifs, bool create_ram = false);

    private:
        NESHeader header;
        size_t size_prg_rom = 0;
        size_t size_chr_rom = 0;
        std::vector<Byte> prg_rom;
        std::vector<Byte> chr_rom;//if header byte 5 == 0, it's chr-ram;
        std::vector<Byte> prg_ram;
    };

};//end nes

#endif