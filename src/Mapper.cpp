#include "Mapper.h"

#include "Mapper_NROM.h"

namespace nes {

    Mapper::Mapper(Cartridge &_cart) : cart{&_cart}{}

    NT_Mirror Mapper::get_nt_mirror(){
        const NESHeader &cr_h = cart->get_header();
        if (cr_h.four_screen)
            return NT_Mirror::Four_screen;
        else if (cr_h.mirror_hv)
            return NT_Mirror::Vertical;
        else return NT_Mirror::Horizontal;
    }

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

};//end nes;