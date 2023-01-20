#include<iostream>
#include<fstream>
#include<sys/stat.h>

#include "Cartridge.h"

namespace nes {

    Cartridge::Cartridge(){}

    Cartridge::Cartridge(const char *file_path){
        load_file(file_path);
    };

    Cartridge::~Cartridge(){}

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

    bool Cartridge::read_cart_ram(Word addr, Byte &data){
        data = cart_ram[addr];
        return true;
    }

    bool Cartridge::write_cart_ram(Word addr, Byte data){
        cart_ram[addr] = data;
        return true;
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
            || header.NES1A[3] != 0x1a  )
        {
            printf("This is not an iNES file.\n");
        }
        else {

            // Recommended detection procedure for distinguish format version:
            //      If byte 7 AND $0C = $08, and the size taking into account byte 9 
            //          does not exceed the actual size of the ROM image, then NES 2.0.
            //      If byte 7 AND $0C = $04, archaic iNES.
            //      If byte 7 AND $0C = $00, and bytes 12-15 are all 0, then iNES.
            //      Otherwise, iNES 0.7 or archaic iNES.
            
        
            switch (header.byte_7.val & 0xC){
            case 0x8 :
            {
                // check size of rom file comparing to the info;
                // 12bits addr should address max to 64MB (as 16KB for each bank)
                size_t info_rom_size = header.tailBytes[0] & 0xf; //tailBytes[0] = Byte 9;
                info_rom_size <<= 8;
                info_rom_size &= 0xf00;
                info_rom_size |= header.num_prg_rom;
                info_rom_size *= 0x4000;
                struct stat statbuf;
                stat(file_path, &statbuf);
                if (info_rom_size >= statbuf.st_size)
                    std::clog << "Notice: rom file size is smaller than inferred from header." << std::endl;

                std::clog << "The NES 2.0 file is correctly loaded." << std::endl;
                /* Read info according to NES 2.0 standard */
                std::clog << "NES 2.0 format is on the work." << std::endl;
            }
            break;
            case 0x4 :
            {
                std::clog << "The archaic iNES file is correctly loaded." << std::endl;
                /* Read info according to archaic iNES standard */
                std::clog << "Archaic iNES format is on the work." << std::endl;
            }
            break;
            case 0x0 :
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

                if (header.byte_6.trainer){//if has trainer within rom file;
                    //just ignore the trainer;
                    ifs.seekg(kTRAINER_SIZE, std::ios_base::cur);
                    std::clog << "The trainer has been ignored." << std::endl;
                }

                prg_rom.resize(kPRG_ROM_SIZE * header.num_prg_rom);
                ifs.read(reinterpret_cast<char*>(&prg_rom[0]), kPRG_ROM_SIZE * header.num_prg_rom);

                chr_rom.resize(kCHR_ROM_SIZE * header.num_chr_rom);
                ifs.read(reinterpret_cast<char*>(&chr_rom[0]), kCHR_ROM_SIZE * header.num_chr_rom);
                
                if (header.byte_6.save_ram){ //if has save_ram;
                    cart_ram.resize(kPRG_RAM_SIZE);
                    std::clog << "Save-RAM prepared." << std::endl;
                }

                ret = true;
            }
            break;
            default  :
            {
                std::clog << "The archaic iNES / iNES 0.7 file is correctly loaded." << std::endl;
                /* Read info according to archaic iNES standard */
                std::clog << "Archaic iNES / iNES 0.7 format is on the work." << std::endl;
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
        if (header.byte_6.mirror_hv) std::clog << "vertical" << std::endl;
        else std::clog << "horizontal" << std::endl;
        std::clog << "Save-RAM            : ";
        if (header.byte_6.save_ram) std::clog << "Y" << std::endl;
        else std::clog << "N" << std::endl;
        std::clog << "512-byte Trainer    : ";
        if (header.byte_6.trainer) std::clog << "Y" << std::endl;
        else std::clog << "N" << std::endl;
        std::clog << "4-screeen mirroring : ";
        if (header.byte_6.four_screen) std::clog << "Y" << std::endl;
        else std::clog << "N" << std::endl;
        std::clog << "Mapper #            : "
            << static_cast<int>(
                (header.byte_6.val >> 4) | (header.byte_7.val & 0xf0)
                ) << std::endl;
        // Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility)
        std::clog << "8 KB PRG-RAM        : ";
        if (header.num_prg_ram == 0) std::clog << 1 << std::endl;
        else std::clog << static_cast<int>(header.num_prg_ram) << std::endl;
        std::clog << std::endl;
        std::clog << "iNES header info reading complete." << std::endl;
    }

};//end nes