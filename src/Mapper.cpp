#include "Mapper.h"

#include "Mapper_NROM.h"

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

    std::shared_ptr<Mapper> Mapper::create_mapper(Cartridge &cr_cart){
        Mapper_Type n_mapper = cart->header.byte_7.n_mapper_high;
        n_mapper <<= 4;
        n_mapper |= cart->header.byte_6.n_mapper_low;

        switch (n_mapper){
        case Mapper_Type::NROM :
            return std::make_shared<Mapper_NROM>(cr_cart);
            break;
        default :
            return nullptr;
            break;
        }
    }

};//end nes;