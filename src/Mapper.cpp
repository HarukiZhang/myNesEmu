#include "Mapper.h"
#include "Cartridge.h"

namespace nes {

    Mapper::Mapper(Cartridge &_cart){
        cart = &_cart;
    }

    NT_Mirror Mapper::get_nt_mirror(){
        if (cart->header.byte_6.four_screen)
            return NT_Mirror::Four_screen;
        else if (cart->header.byte_6.mirror_hv)
            return NT_Mirror::Vertical;
        else return NT_Mirror::Horizontal;
    }

};//end nes;