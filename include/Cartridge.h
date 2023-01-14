#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include<sys/stat.h>
#include<fstream>

#include "NESDef.h"

namespace nes {

    //forward declaration;
    class Mapper;

    class Cart {
    public:
        Cart(){
            stat(rom_path, &statbuf);
            load_file();
            read_header();
            set_mapper();
        };
        ~Cart(){
            rom_stream.close();
        }
        Byte read_prg_rom();
        Byte read_chr_rom();
        void set_mapper();
        Mapper *get_mapper(){return mapper;}
    private:
        bool load_file(){
            rom_stream.open(rom_path, std::ios_base::binary | std::ios_base::in);
            if (!rom_stream){
                std::cout << "ROM file open failed." << std::endl;
                return false;
            }
            return true;
        }

        void read_header(){

            rom_stream.read(reinterpret_cast<char*>(&header), 0x10);

            // verify header magic word;
            if (header.NES1A[0] != 'N'
                || header.NES1A[1] != 'E'
                || header.NES1A[2] != 'S'
                || header.NES1A[3] != 0x1a
            )
            {
                printf("This is not a iNES file.\n");
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
                    if (info_rom_size >= statbuf.st_size)
                        std::cout << "Notice: rom file size is smaller than inferred from header." << std::endl;

                    std::cout << "The NES 2.0 file is correctly loaded." << std::endl;
                    /* Read info according to NES 2.0 standard */
                    std::cout << "NES 2.0 format is on the work." << std::endl;
                }
                break;
                case 0x4 :
                {
                    std::cout << "The archaic iNES file is correctly loaded." << std::endl;
                    /* Read info according to archaic iNES standard */
                    std::cout << "Archaic iNES format is on the work." << std::endl;
                }
                break;
                case 0x0 :
                {
                    std::cout << "The iNES file is correctly loaded." << std::endl;
                    std::cout << std::endl;

                    //check whether Bytes 12 - 15 are all zero;
                    for (int i = 3; i < 7; ++i){
                        if (header.tailBytes[i] != 0){
                            std::cout << "Notice: the tail of header is not all zeros, the content:" << std::endl;
                            for (int j = i; j < 7; ++j){
                                std::cout << header.tailBytes[j];
                            }
                            std::cout << std::endl << std::endl;
                            break;
                        }
                    }

                    print_info_v_iNES();
                }
                break;
                default  :
                {
                    std::cout << "The archaic iNES / iNES 0.7 file is correctly loaded." << std::endl;
                    /* Read info according to archaic iNES standard */
                    std::cout << "Archaic iNES / iNES 0.7 format is on the work." << std::endl;
                }
                break;
                }
            }
            return;
        }

        void load_mapper();

        void print_info_v_iNES(){
            std::cout << "16KB PRG-ROM banks  : "
                << static_cast<int>(header.num_prg_rom) << std::endl;
            std::cout << "8 KB CHR-ROM banks  : "
                << static_cast<int>(header.num_chr_rom) << std::endl;
            std::cout << "Mirroring Type      : ";
            if (header.byte_6.mirror_hv) std::cout << "vertical" << std::endl;
            else std::cout << "horizontal" << std::endl;
            std::cout << "Save-RAM            : ";
            if (header.byte_6.save_ram) std::cout << "Y" << std::endl;
            else std::cout << "N" << std::endl;
            std::cout << "512-byte Trainer    : ";
            if (header.byte_6.trainer) std::cout << "Y" << std::endl;
            else std::cout << "N" << std::endl;
            std::cout << "4-screeen mirroring : ";
            if (header.byte_6.four_screen) std::cout << "Y" << std::endl;
            else std::cout << "N" << std::endl;
            std::cout << "Mapper #            : "
                << static_cast<int>(
                    (header.byte_6.val >> 4) | (header.byte_7.val & 0xf0)
                    ) << std::endl;
            // Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility)
            std::cout << "8 KB PRG-RAM        : ";
            if (header.num_prg_ram == 0) std::cout << 1 << std::endl;
            else std::cout << static_cast<int>(header.num_prg_ram) << std::endl;
            std::cout << std::endl;
            std::cout << "iNES header info reading complete." << std::endl;
        }

    private:
        char *rom_path;
        std::ifstream rom_stream;
        struct stat statbuf;
        NESHeader header;
        Mapper *mapper;
    };

};//end nes

#endif