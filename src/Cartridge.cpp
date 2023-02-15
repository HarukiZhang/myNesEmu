#include<iostream>
#include<fstream>
#include<sys/stat.h>

#include "Cartridge.h"

namespace nes {

    Cartridge::Cartridge(){}

    Cartridge::~Cartridge() {
        if (!prg_ram.empty()) {
            std::ofstream ofs{ "D:\\haruk\\Projects\\nesEmu\\myNESEmu\\test\\save_ram_out", std::ios_base::out | std::ios_base::trunc };
            if (!ofs.is_open()) {
                std::clog << "Output file open failed." << std::endl;
                return;
            }

            //    Text output
            //    ---------- -
            //    Tests generally print information on screen.They also output the same
            //    text as a zero - terminted string beginning at $6004, allowing examination
            //    of output in an NSF player, or a NES emulator without a working PPU.The
            //    tests also work properly if the PPU doesn't set the VBL flag properly or
            //    doesn't implement it at all.
            //    The final result is displayed and also written to $6000.Before the test
            //    starts, $80 is written there so you can tell when it's done. If a test
            //    needs the NES to be reset, it writes $81 there.
            // 
            //    In addition, $DE $B0 $61
            //    is written to $6001 - $6003 to allow an emulator to detect when a test is
            //    being run, as opposed to some other NES program.In NSF builds, the
            //    final result is also reported via a series of beeps(see below).

            ofs << "Save-RAM data : ";
            if (prg_ram[1] == 0xDE && prg_ram[2] == 0xB0 && prg_ram[3] == 0x61)
                ofs << "valid" << std::endl;
            else ofs << "invalid" << std::endl;
            ofs << "======================================" << std::endl;
            if (prg_ram[0x4])
                ofs << reinterpret_cast<char*>(&prg_ram[0x4]);
            ofs << std::endl;

            ofs.close();
        }
    }

    Cartridge::Cartridge(const char *file_path){
        load_file(file_path);
    };

    bool Cartridge::read_prg_rom(Word addr, Byte &data){
        //need mapper to give an appropriate address
        //for bank shifting;
        data = prg_rom[addr];
        return true;
    }

    bool Cartridge::read_chr_rom(Word addr, Byte &data){
        data = chr_rom[addr];
        return true;
    }

    bool Cartridge::read_prg_ram(Word addr, Byte &data){
        data = prg_ram[addr];
        return true;
    }

    bool Cartridge::write_prg_ram(Word addr, Byte data){
        prg_ram[addr] = data;
        return true;
    }

    const NESHeader &Cartridge::get_header(){
        return header;
    }

    bool Cartridge::load_test_rom(const char* file_path, bool create_prg_ram)
    {
        std::ifstream ifs{ file_path, std::ios_base::binary | std::ios_base::in };
        if (!ifs.is_open()) {
            std::cerr << "Test ROM file open : failed." << std::endl;
            return false;
        }
        bool ret = false;
        ifs.read(reinterpret_cast<char*>(&header), kNES_HEAD_SIZE);
        if (header.NES1A[0] != 'N' || header.NES1A[1] != 'E' || header.NES1A[2] != 'S' || header.NES1A[3] != 0x1A){
            printf("This is not an iNES file.\n");
            ret = false;
        }
        else {
            print_info_v_iNES();

            //need to pre-check Mapper type;
            unsigned int mapper_num = header.n_mapper();
            switch (mapper_num) {
            case Mapper_Type::NROM:
                load_content(ifs, create_prg_ram);
                ret = true;
                break;
            default:
                std::clog << "Mapper " << mapper_num << " is not support now." << std::endl;
                ret = false;
                break;
            }
        }
        ifs.close();
        return ret;
    }

    bool Cartridge::load_file(const char *file_path){
        std::ifstream ifs {file_path, std::ios_base::binary | std::ios_base::in};
        if (!ifs.is_open()){
            std::cerr << "ROM file open failed." << std::endl;
            return false;
        }

        bool ret = false;

        ifs.read(reinterpret_cast<char*>(&header), kNES_HEAD_SIZE);
        // verify header magic word;
        if (header.NES1A[0] != 'N'
            || header.NES1A[1] != 'E'
            || header.NES1A[2] != 'S'
            || header.NES1A[3] != 0x1A  )
        {
            printf("This is not an iNES file.\n");
            ret = false;
        }
        else {

            // Recommended detection procedure for distinguish format version:
            //      If byte 7 AND $0C = $08, and the size taking into account byte 9 
            //          does not exceed the actual size of the ROM image, then NES 2.0.
            //      If byte 7 AND $0C = $04, archaic iNES.
            //      If byte 7 AND $0C = $00, and bytes 12-15 are all 0, then iNES.
            //      Otherwise, iNES 0.7 or archaic iNES.
        
            switch (header.nes_2_sign){
            case NES_VER::NES_2_0 :
            {
                // check size of rom file comparing to the info;
                // 12bits addr should address max to 64MB (as 16KB for each bank)
                //size_t info_rom_size = header.tailBytes[0] & 0xf; //tailBytes[0] = Byte 9;
                //info_rom_size <<= 8;
                //info_rom_size &= 0xf00;
                //info_rom_size |= header.num_prg_rom;
                //info_rom_size *= 0x4000;
                //struct stat statbuf;
                //stat(file_path, &statbuf);
                //if (info_rom_size >= statbuf.st_size)
                //    std::clog << "Notice: rom file size is smaller than inferred from header." << std::endl;

                std::clog << "The NES 2.0 file is correctly loaded." << std::endl;
                /* Read info according to NES 2.0 standard */
                std::clog << "NES 2.0 format is on the work." << std::endl;

                ret = false;
            }
            break;
            case NES_VER::archaic_iNES :
            {
                std::clog << "The archaic iNES file is correctly loaded." << std::endl;
                /* Read info according to archaic iNES standard */
                std::clog << "Archaic iNES format is on the work." << std::endl;
                
                ret = false;
            }
            break;
            case NES_VER::iNES_1_0 :
            {
                std::clog << "The iNES file is correctly loaded." << std::endl;
                std::clog << std::endl;

                //check whether Bytes 12 - 15 are all zero;
                for (int i = 3; i < 7; ++i){
                    if (header.tailBytes[i] != 0){
                        std::clog << "Notice: the tail of header is not all zeros, the content:" << std::endl;
                        for (int j = i; j < 7; ++j){
                            std::clog << header.tailBytes[j];
                        }
                        std::clog << std::endl << std::endl;
                        break;
                    }
                }

                print_info_v_iNES();

                //check mapper for how to load rom content;
                switch (header.n_mapper()) {
                case Mapper_Type::NROM:
                    load_content(ifs);//set the prg_ram only when file indicates;
                    ret = true;
                    break;
                default:
                    std::clog << "Mapper #" << (int)header.n_mapper() << " is not support now." << std::endl;
                    ret = false;
                    break;
                }

            }
            break;
            default  :
            {
                std::clog << "The archaic iNES / iNES 0.7 file is correctly loaded." << std::endl;
                /* Read info according to archaic iNES standard */
                std::clog << "Archaic iNES / iNES 0.7 format is on the work." << std::endl;
                
                ret = false;
            }
            break;
            }
        }

        ifs.close();
        return ret;
    }

    void Cartridge::print_info_v_iNES(){
        std::clog << "16KB PRG-ROM banks  : "
            << static_cast<int>(header.num_prg_rom) << std::endl;
        std::clog << "8 KB CHR-ROM banks  : "
            << static_cast<int>(header.num_chr_rom) << std::endl;
        std::clog << "Mirroring Type      : ";
        if (header.mirror_hv) std::clog << "vertical" << std::endl;
        else std::clog << "horizontal" << std::endl;
        std::clog << "8 KB Save/PRG-RAM   : ";
        if (header.save_ram) std::clog << "Y" << std::endl;
        else std::clog << "N" << std::endl;
        std::clog << "512 Byte Trainer    : ";
        if (header.trainer) std::clog << "Y" << std::endl;
        else std::clog << "N" << std::endl;
        std::clog << "4-screeen mirroring : ";
        if (header.four_screen) std::clog << "Y" << std::endl;
        else std::clog << "N" << std::endl;
        std::clog << "Mapper #            : "
            << static_cast<int>(header.n_mapper()) << std::endl;
        // Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility)
        // std::clog << "8 KB PRG-RAM        : ";
        // if (header.save_ram) std::clog << "Y" << std::endl;
        // else std::clog << "N" << std::endl;
        std::clog << std::endl;
        std::clog << "iNES header info reading complete." << std::endl;
    }

    void Cartridge::load_content(std::ifstream& ifs, bool create_ram){
        if (header.trainer) {//if has trainer within rom file;
            //just ignore the trainer;
            ifs.seekg(kTRAINER_SIZE, std::ios_base::cur);
            std::clog << "The trainer has been ignored." << std::endl;
        }

        if (size_prg_rom = (size_t)kPRG_ROM_SIZE * header.num_prg_rom) {//Attention: size could be 0;
            prg_rom.resize(size_prg_rom);
            ifs.read(reinterpret_cast<char*>(&prg_rom[0]), size_prg_rom);
        }

        if (size_chr_rom = (size_t)kCHR_ROM_SIZE * header.num_chr_rom) {
            chr_rom.resize(size_chr_rom);
            ifs.read(reinterpret_cast<char*>(&chr_rom[0]), size_chr_rom);
        }

        if (header.save_ram || create_ram) {
            //8KiB battery-backed or persistent memory mapped to $6000 - $7FFF;
            prg_ram.resize(kPRG_RAM_SIZE);
            std::clog << "PRG-RAM is prepared." << std::endl;
            if (create_ram) header.save_ram = 1;//to inform mapper;
        }
    }

};//end nes