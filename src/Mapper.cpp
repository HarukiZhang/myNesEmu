#include "Mapper.h"

#include "Mapper_NROM.h"

namespace nes {

    Mapper::Mapper(Cartridge &_cart) : cart{&_cart}{}

    std::shared_ptr<Mapper> Mapper::create_mapper(Cartridge &r_cart){
        switch ( r_cart.get_header().n_mapper() ){
        case Mapper_Type::NROM :
            return std::make_shared<Mapper_NROM>(r_cart);
            break;
        default :
            return nullptr;
            break;
        }
    }

    inline Word Mapper::get_nt_mirror(Word addr){
        return (this->*nt_mirror_map)(addr);
    }

    // mirror functions =========================================================
    Word Mapper::mirror_horizontal(Word addr){
        /*  0010 0'000 0000 0000 -> $0000
            0010 0'011 1111 1111 -> $03FF
            0010 0'100 0000 0000 -> $0000
            0010 0'111 1111 1111 -> $03FF
            0010 1'000 0000 0000 -> $0400
            0010 1'011 1111 1111 -> $07FF
            0010 1'100 0000 0000 -> $0400
            0010 1'111 1111 1111 -> $07FF */
        //mask=0x400 if bit 11 is set, otherwise 0x0;
        return (addr & 0x3ff) | ((addr & 0x800) >> 1);
    }

    Word Mapper::mirror_vertical(Word addr){
        // 0010 0000 0000 0000 -> $0000
        // 0010 0011 1111 1111 -> 0000 0011 1111 1111
        // 0010 0100 0000 0000 -> 0000 0100 0000 0000
        // 0010 0111 1111 1111 -> 0000 0111 1111 1111
        // 0010 1000 0000 0000 -> $0000
        // 0010 1011 1111 1111 -> 0000 0011 1111 1111
        // 0010 1100 0000 0000 -> 0000 0100 0000 0000
        // 0010 1111 1111 1111 -> 0000 0111 1111 1111
        return addr & 0x7ff;
    }

    Word Mapper::mirror_single_screen(Word addr){
        return addr & 0x3ff;
    }

    Word Mapper::mirror_four_screen(Word addr){
        if (addr & 0x800){
            //high addr need to call mapper;
            //TODO: how to address cart-nested ram?
            return addr & 0x7ff;//  <--- this is wrong now;
        }
        else
            return addr & 0x7ff;
    }

};//end nes;