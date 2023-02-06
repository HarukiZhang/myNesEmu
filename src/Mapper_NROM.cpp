#include "Mapper_NROM.h"
#include "Cartridge.h"

namespace nes {

    Mapper_NROM::Mapper_NROM(Cartridge &_cart)
        : Mapper{_cart}
    {
        const NESHeader &cr_h = _cart.get_header();
        if(cr_h.num_prg_rom == 2)
            addr_mask = 0x7FFF;//for NROM_256;
        if (cr_h.save_ram)
            has_prg_ram = true;//is this correct ?
        
        //Fixed H or V, controlled by solder pads (*V only)
        Mapper::nt_mirror_map = cr_h.mirror_hv ? 
            &Mapper::mirror_vertical : &Mapper::mirror_horizontal;
    }

    Mapper_NROM::~Mapper_NROM(){
        //trivial: nothing to release;
    }

    bool Mapper_NROM::cpu_read(Word addr, Byte &data){
        //MainBus wont give addresses below $6000;
        if (addr < 0x8000){
            // 0110 0000 0000 0000 - 0111 1111 1111 1111
            // -> $0000            - 0001 1111 1111 1111
            if (has_prg_ram)//              <-- is this if-judge necessary?
                return Mapper::cart->read_prg_ram(addr - 0x6000, data);
            else return false;
        }
        else {//program should guarantee addr not beyond $FFFF;
            return Mapper::cart->read_prg_rom(addr & addr_mask, data);
            // num_bank  |  low_addr      |  high_addr     |
            //   one     | $8000 -> $0000 | $C000 -> $0000 |
            //           | $BFFF -> $3FFF | $FFFF -> $3FFF |
            //   two     | $8000 -> $0000 | $C000 -> $4000 |
            //           | $BFFF -> $3FFF | $FFFF -> $7FFF |
        }
        return false;
    }

    bool Mapper_NROM::cpu_write(Word addr, Byte data){
        //MainBus wont give addresses below $6000;
        if (addr < 0x8000){
            if (has_prg_ram)
                return Mapper::cart->write_prg_ram(addr - 0x6000, data);
            else return false;
        }
        //it should be non-writable to prg-rom and chr-rom;
        return false;
    }

    bool Mapper_NROM::ppu_read(Word addr, Byte &data){
        //HybridBus only gives addr below 0x2000;
        return Mapper::cart->read_chr_rom(addr, data);
    }
    bool Mapper_NROM::ppu_write(Word addr, Byte data){
        //CHR-ROM is not writable;
        return false;
    }

};//end nes;