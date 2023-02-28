#include<iostream>

#include "Mapper.h"

#include "Mapper_000.h"
#include "Mapper_001.h"
#include "Mapper_002.h"
#include "Mapper_004.h"

namespace nes {

    Mapper::Mapper(Cartridge& _cart) : nt_mirror_map{nullptr}, cart{ &_cart }{}

    std::shared_ptr<Mapper> Mapper::create_mapper(Cartridge &r_cart){
        switch ( r_cart.get_header().n_mapper() ){
        case Mapper_Type::NROM :
            return std::make_shared<Mapper_000>(r_cart);
            break;
        case Mapper_Type::MMC1:
            return std::make_shared<Mapper_001>(r_cart);
            break;
        case Mapper_Type::UxROM :
            return std::make_shared<Mapper_002>(r_cart);
            break;
        case Mapper_Type::MMC3:
            return std::make_shared<Mapper_004>(r_cart);
            break;
        default :
            std::cerr << "Mapper::create_mapper : Now, only Mapper 000, 001, 002, 004 is available." << std::endl;
            return nullptr;
            break;
        }
    }

    void Mapper::init_nt_mirror()
    {
        const NESHeader& r_h = cart->get_header();
        if (r_h.four_screen) {
            nt_mirror_map = &Mapper::mirror_four_screen;
        }
        else if (r_h.mirror_hv) {
            nt_mirror_map = &Mapper::mirror_vertical;
        }
        else {
            nt_mirror_map = &Mapper::mirror_horizontal;
        }
    }

    // mirror functions =========================================================
    Byte& Mapper::mirror_horizontal(Word addr){
        /*  0010 0'000 0000 0000 -> $0000
            0010 0'011 1111 1111 -> $03FF
            0010 0'100 0000 0000 -> $0000
            0010 0'111 1111 1111 -> $03FF
            0010 1'000 0000 0000 -> $0400
            0010 1'011 1111 1111 -> $07FF
            0010 1'100 0000 0000 -> $0400
            0010 1'111 1111 1111 -> $07FF */
        //mask=0x400 if bit 11 is set, otherwise 0x0;
        return vram[(addr & 0x3ff) | ((addr & 0x800) >> 1)];
    }

    Byte& Mapper::mirror_vertical(Word addr){
        // 0010 0000 0000 0000 -> $0000
        // 0010 0011 1111 1111 -> 0000 0011 1111 1111
        // 0010 0100 0000 0000 -> 0000 0100 0000 0000
        // 0010 0111 1111 1111 -> 0000 0111 1111 1111
        // 0010 1000 0000 0000 -> $0000
        // 0010 1011 1111 1111 -> 0000 0011 1111 1111
        // 0010 1100 0000 0000 -> 0000 0100 0000 0000
        // 0010 1111 1111 1111 -> 0000 0111 1111 1111
        return vram[addr & 0x7ff];
    }

    Byte& Mapper::mirror_a_only(Word addr){
        //map $2000 ~ $2FFF to $2000 ~ $23FF;
        return vram[addr & 0x3ff];
    }

    Byte& Mapper::mirror_b_only(Word addr) {
        //map $2000 ~ $2FFF to $2400 ~ $27FF;
        return vram[(addr & 0x3ff) + kNAME_TBL_SIZE];
    }

    Byte& Mapper::mirror_four_screen(Word addr){
        if (addr & 0x800){//high addr nested in cart;
            return cart->access_ext_ram(addr & 0x7ff);
        }
        else {
            return vram[addr & 0x7ff];
        }
    }

};//end nes;