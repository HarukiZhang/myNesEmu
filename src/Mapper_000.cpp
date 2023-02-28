#include "Mapper_000.h"

namespace nes {

    Mapper_000::Mapper_000(Cartridge &_cart)
        : Mapper{_cart}
    {
        const NESHeader &cr_h = _cart.get_header();
        if(cr_h.num_prg_rom == 2)
            addr_mask = 0x7FFF;//for NROM_256;
        
        //Fixed H or V, controlled by solder pads (*V only)
        Mapper::init_nt_mirror();
    }

    Mapper_000::~Mapper_000(){
        //trivial: nothing to release;
    }

    void Mapper_000::reset() {
        //nothing to do;
    }

    bool Mapper_000::cpu_read(Word addr, Byte &data){
        //MainBus wont give addresses below $6000;
        if (addr < 0x8000){
            // 0110 0000 0000 0000 - 0111 1111 1111 1111
            // -> $0000            - 0001 1111 1111 1111
            return Mapper::cart->read_prg_ram(addr - 0x6000, data);//whether the cart has a PRG-RAM is up to the cart itself;
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

    bool Mapper_000::cpu_write(Word addr, Byte data){
        //MainBus wont give addresses below $6000;
        if (addr < 0x8000){
            return Mapper::cart->write_prg_ram(addr - 0x6000, data);
        }
        //it should be non-writable to prg-rom and chr-rom;
        return false;
    }

    bool Mapper_000::ppu_read(Word addr, Byte &data){
        //PPU gives addresses below $3F00;
        if (addr < 0x2000) {
            //even if num_chr_rom == 0, it will automatically allocate 8KiB as a CHR-RAM;
            return Mapper::cart->read_chr_rom(addr, data);
        }
        else {//$2000 ~ $3EFF;
            //map $3XXX to $2XXX;
            data = (this->*Mapper::nt_mirror_map)(addr & 0x2fff);
            return true;
        }
    }

    bool Mapper_000::ppu_write(Word addr, Byte data){
        //PPU gives addresses below $3F00;
        if (addr < 0x2000) {
            //whether to write should be up to the program;
            return Mapper::cart->write_chr_ram(addr, data);
        }
        else {//$2000 ~ $3EFF;
            //map $3XXX to $2XXX;
            (this->*Mapper::nt_mirror_map)(addr & 0x2fff) = data;
            return true;
        }
    }

    bool Mapper_000::irq_state()
    {
        return false;
    }

    bool Mapper_000::count_scanline()
    {
        return false;
    }

};//end nes;