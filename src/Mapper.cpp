#include "Mapper.h"

namespace nes {

    Mapper::Mapper(Cartridge &_cart){
        cart = &_cart;
    }

    Mapper::~Mapper(){}

};//end nes;