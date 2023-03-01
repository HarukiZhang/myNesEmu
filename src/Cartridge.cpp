#include<iostream>
#include<fstream>
#include<sys/stat.h>

#include "Cartridge.h"

namespace nes {

    Cartridge::Cartridge(){}

    Cartridge::~Cartridge() {
        save_sram();
    }

    Cartridge::Cartridge(const char *_file_path){
        load_file(_file_path);
    };

    bool Cartridge::read_prg_rom(Phad addr, Byte &data){
        //need mapper to give an appropriate address
        //for bank shifting;
        data = prg_rom[addr];
        return true;
    }

    bool Cartridge::read_chr_rom(Phad addr, Byte &data){
        data = chr_rom[addr];
        return true;
    }

    bool Cartridge::write_chr_ram(Phad addr, Byte data) {
        //header byte 5 == 0 indicates the board uses CHR-RAM;
        //whether to write to ram should be decided by Mapper;
        chr_rom[addr] = data;
        return true;
    }

    bool Cartridge::read_prg_ram(Phad addr, Byte &data){
        if (!prg_ram.empty()) {
            data = prg_ram[addr];
            return true;
        }
        return false;
    }

    bool Cartridge::write_prg_ram(Phad addr, Byte data){
        if (!prg_ram.empty()) {
            prg_ram[addr] = data;
            return true;
        }
        return false;
    }

    Byte& Cartridge::access_ext_ram(Phad addr)
    {
        if (!ext_ram.empty())
            return ext_ram[addr];
        return null_ret;

    }

    const NESHeader &Cartridge::get_header(){
        return header;
    }

    bool Cartridge::load_test_rom(const char* _file_path, bool create_prg_ram)
    {
        file_path = _file_path;
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
            case Mapper_Type::UxROM:
                struct stat statbuf;
                stat(file_path, &statbuf);
                if (statbuf.st_size > 0x40000) {
                    std::clog << "Rom file loading is aborted due to : rom size is larger than 256 KiB." << std::endl;
                    ret = false;
                }
                else {
                    load_content(ifs, create_prg_ram);
                    ret = true;
                }
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

    bool Cartridge::load_file(const char *_file_path){
        file_path = _file_path;
        std::ifstream ifs {file_path, std::ios_base::binary | std::ios_base::in};
        if (!ifs.is_open()){
            std::cerr << "Cartridge::load_file : ROM file open failed." << std::endl;
            return false;
        }

        bool proceed = true;

        struct stat statbuf;
        stat(file_path, &statbuf);

        //loading of rom that is bigger than 1 MiB will be aborted;
        if (statbuf.st_size > 0x100000) {
            std::clog << "Rom file loading is aborted due to : rom size is larger than 1 MiB." << std::endl;
            proceed = false;
        }

        // verify header magic word;
        if (proceed) {
            ifs.read(reinterpret_cast<char*>(&header), kNES_HEAD_SIZE);
            if (header.NES1A[0] != 'N' || header.NES1A[1] != 'E' || header.NES1A[2] != 'S' || header.NES1A[3] != 0x1A) {
                printf("This is not an iNES file.\n");
                proceed = false;
            }
        }

        // check file format;
        if (proceed) {

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

                proceed = false;
            }
            break;
            case NES_VER::archaic_iNES :
            {
                std::clog << "The archaic iNES file is correctly loaded." << std::endl;
                /* Read info according to archaic iNES standard */
                std::clog << "Archaic iNES format is on the work." << std::endl;
                
                proceed = false;
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

                proceed = true;

            }
            break;
            default  :
            {
                std::clog << "The archaic iNES / iNES 0.7 file is correctly loaded." << std::endl;
                /* Read info according to archaic iNES standard */
                std::clog << "Archaic iNES / iNES 0.7 format is on the work." << std::endl;
                
                proceed = false;
            }
            break;
            }
        }

        //make sure that num_prg_rom is not 0;
        if (proceed) {
            if (header.num_prg_rom == 0) {
                std::clog << "Cartridge::load_file : Loading of rom is aborted : num_prg_rom == 0" << std::endl;
                proceed = false;
            }
        }

        //check mapper and load rom content;
        if (proceed) {
            switch (header.n_mapper()) {
            case Mapper_Type::NROM:
                load_content(ifs);//set the prg_ram only when file indicates;
                proceed = true;
                break;
            case Mapper_Type::MMC1:
                if (header.num_prg_rom > 16) {
                    std::clog << "Cartridge::load_file : Mapper 001 mapping do not support PRG-ROM larger than 256 KiB now." << std::endl;
                    proceed = false;
                }
                else {
                    load_content(ifs, true);//by default, create an 8KiB PRG-RAM;
                    proceed = true;
                }
                break;
            case Mapper_Type::UxROM:
                if (header.num_prg_rom > 16) {
                    std::clog << "Cartridge::load_file : Mapper 002 mapping do not support PRG-ROM larger than 256 KiB now." << std::endl;
                    proceed = false;
                }
                else {
                    load_content(ifs);//Mapper_002 : PRG RAM capacity : None;
                    proceed = true;
                }
                break;
            case Mapper_Type::CNROM:
                if (header.num_chr_rom > 4) {
                    std::clog << "Cartridge::load_file : Mapper 003 mapping do not support CHR-ROM larger than 32 KiB now." << std::endl;
                    proceed = false;
                }
                else {
                    load_content(ifs);//Mapper_003 : PRG RAM capacity : None;
                    proceed = true;
                }
                break;
            case Mapper_Type::MMC3:
                load_content(ifs);//Mapper_004 : allocate PRG-RAM only when cart told so;
                proceed = true;
                break;
            default:
                std::clog << "Cartridge::load_file : Mapper #" << (int)header.n_mapper() << " is not support now." << std::endl;
                proceed = false;
                break;
            }
        }

        ifs.close();
        return proceed;
    }

    bool Cartridge::save_sram()
    {
        //"D:\\haruk\\Projects\\nesEmu\\myNESEmu\\test\\save_ram_out"

        if (!prg_ram.empty() && header.prg_ram) {
            std::ofstream ofs{ make_sav_path(), std::ios_base::out | std::ios_base::trunc };
            if (!ofs.is_open()) {
                std::clog << "Cartridge::save_sram : sav file open failed." << std::endl;
                return false;
            }

            ofs.write(reinterpret_cast<char*>(&prg_ram[0]), kPRG_RAM_SIZE);
            ofs.close();
            std::clog << "Cartridge::save_sram : Save-RAM has been successfully saved." << std::endl;
            
            return true;
        }
        return false;
    }

    bool Cartridge::load_save()
    {
        if (!prg_ram.empty() && header.prg_ram) {//load sav only if the rom told so;
            std::ifstream ifs{ make_sav_path() };
            if (ifs.is_open()) {
                ifs.read(reinterpret_cast<char*>(&prg_ram[0]), kPRG_RAM_SIZE);
                ifs.close();
                std::clog << "Cartridge::load_save : sav file has been successfully loaded." << std::endl;
                return true;
            }
            //ifstream open failure indicates addr path do not exist;
            std::clog << "Cartridge::load_save : sav file does not exist." << std::endl;
        }
        return false;
    }

    void Cartridge::load_content(std::ifstream& ifs, bool create_ram){
        if (header.trainer) {//if has trainer within rom file;
            //just ignore the trainer;
            ifs.seekg(kTRAINER_SIZE, std::ios_base::cur);
            std::clog << "Cartridge::load_content : Trainer has been ignored." << std::endl;
        }

        if (size_prg_rom = (size_t)kPRG_ROM_SIZE * header.num_prg_rom) {//Attention: size could be 0;
            prg_rom.resize(size_prg_rom);
            ifs.read(reinterpret_cast<char*>(&prg_rom[0]), size_prg_rom);
            std::clog << "Cartridge::load_content : PRG-ROM has been loaded." << std::endl;
        }

        if (size_chr_rom = (size_t)kCHR_ROM_SIZE * header.num_chr_rom) {
            chr_rom.resize(size_chr_rom);
            ifs.read(reinterpret_cast<char*>(&chr_rom[0]), size_chr_rom);
            std::clog << "Cartridge::load_content : CHR-ROM has been loaded." << std::endl;
        }
        else {//if header byte 5 == 0, the borad uses CHR-RAM;
            //by default, allocate 8 KiB for the CHR-RAM;
            chr_rom.resize(kPRG_RAM_SIZE, 0);
            std::clog << "Cartridge::load_content : CHR-RAM has been prepared." << std::endl;
        }

        if (header.prg_ram || create_ram) {
            //8KiB battery-backed or persistent memory mapped to $6000 - $7FFF;
            //refer to header byte 8 : "Value 0 infers 8 KB for compatibility"
            prg_ram.resize(kPRG_RAM_SIZE);
            std::clog << "Cartridge::load_content : PRG-RAM has been prepared." << std::endl;
            
            load_save();

            //if (create_ram) header.prg_ram = true;//to inform mapper;
        }

        if (header.four_screen) {
            //2KiB cartridge-nested extra VRAM used for four-screen NT mirrorring;
            ext_ram.resize(kNAME_TBL_SIZE * 2);
            std::clog << "Cartridge::load_content : Extra VRAM has been prepared." << std::endl;
        }
    }

    std::string Cartridge::make_sav_path()
    {
        std::string _path{ file_path };
        int i = _path.size() - 3;
        _path[i + 0] = 's';
        _path[i + 1] = 'a';
        _path[i + 2] = 'v';
        return _path;
        //int len = 0;
        //while (file_path[len]) ++len;
        //std::string s{ "./" };
        //for (int i = len - 1; i >= 0; --i) {
        //    if (file_path[i] == '\\' || file_path[i] == '/') {
        //        ++i;
        //        s += &file_path[i];
        //        int j = s.size() - 3;
        //        s[j + 0] = 's';
        //        s[j + 1] = 'a';
        //        s[j + 2] = 'v';
        //        return s;
        //        break;
        //    }
        //}
        //s.clear();
        //return s;
    }

    void Cartridge::print_info_v_iNES() {
        std::clog << "16KB PRG-ROM banks  : "
            << static_cast<int>(header.num_prg_rom) << std::endl;
        std::clog << "8 KB CHR-ROM banks  : "
            << static_cast<int>(header.num_chr_rom) << std::endl;
        std::clog << "Mirroring Type      : ";
        if (header.mirror_hv) std::clog << "vertical" << std::endl;
        else std::clog << "horizontal" << std::endl;
        std::clog << "8 KB Save/PRG-RAM   : ";
        if (header.prg_ram) std::clog << "Y" << std::endl;
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
        // if (header.prg_ram) std::clog << "Y" << std::endl;
        // else std::clog << "N" << std::endl;
        std::clog << std::endl;
        std::clog << "iNES header info reading complete." << std::endl;
    }

};//end nes