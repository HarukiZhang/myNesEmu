#include "Mapper.h"
#include "Cartridge.h"

namespace nes {

    Mapper::Mapper(Cartridge &_cart){
        cart = &_cart;
    }

    Mapper::~Mapper(){}

    NTMirror Mapper::get_nt_mirror(){
        if (cart->header.byte_6.four_screen)
            return NTMirror::Four_screen;
        else if (cart->header.byte_6.mirror_hv)
            return NTMirror::Vertical;
        else return NTMirror::Horizontal;
    }

};//end nes;