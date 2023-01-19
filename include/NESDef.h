#ifndef NESDEF_H
#define NESDEF_H

#include<cstdint>

namespace nes {

    using Word = std::uint16_t;
    using Byte = std::uint8_t;

    constexpr size_t kMAX_INSTR_MTX_SIZE = 0x100;

    constexpr size_t kMAX_ZERO_PAGE = 0x100;
    constexpr size_t kMAX_SYS_STACK = 0x100;
    constexpr size_t kSTACK_BASE = 0x100;
    constexpr size_t kMAX_INTER_RAM = 0x600;
    constexpr size_t kMAX_RAM = 0x800;

    constexpr size_t kMAX_VRAM = 0x800;
    constexpr size_t kMAX_NAME_TBL = 0x40;
    constexpr size_t kMAX_ATTR_TBL = 0x3C0;
    constexpr size_t kMAX_OBJ_ATTR = 0x40;
    constexpr size_t kMAX_OAM_BUFF = 0x8;
    constexpr size_t kMAX_CUR_PALET = 0x10;

    constexpr size_t kSPR_HEIGHT_8 = 0x8;
    constexpr size_t kSPR_HEIGHT_16 = 0x10;

    constexpr size_t kNES_HEAD_SIZE = 0x10;
    constexpr size_t kTRAINER_SIZE = 0x200;
    constexpr size_t kPRG_ROM_SIZE = 0x4000;
    constexpr size_t kCHR_ROM_SIZE = 0x2000;

    //RAM for CPU, 2KB;
    typedef union _RAM {
        struct {
            Byte zero_page[kMAX_ZERO_PAGE];
            Byte sys_stack[kMAX_SYS_STACK];
            Byte inter_ram[kMAX_INTER_RAM];
        };
        Byte vals[kMAX_RAM];
    } RAM;

    //RAM for PPU, 2KB;
    typedef union _VRAM {
        struct {
            Byte name_table_0[kMAX_NAME_TBL];
            Byte attribute_table_0[kMAX_ATTR_TBL];
            Byte name_table_1[kMAX_NAME_TBL];
            Byte attribute_table_1[kMAX_ATTR_TBL];
        };
        Byte vals[kMAX_VRAM];
    } VRAM;

    //object attribute entry;
    typedef struct _OBJ_ATTR {
        Byte y_coord_1;
        Byte index;
        union {
            struct {
                Byte n_palette : 2;
                Byte dummy0 : 3;
                Byte prior : 1;
                Byte flip_h : 1;
                Byte flip_v : 1;
            };
            Byte val;
        } attr;
        Byte x_coord;
    } OBJ_ATTR;

    //Object Attribute Management, 256B;
    typedef struct _OAM {
        OBJ_ATTR obj_attr[kMAX_OBJ_ATTR];
    } OAM;

    //secondary OAM, 32B;
    typedef struct _OAM_BUF {
        OBJ_ATTR obj_attr[kMAX_OAM_BUFF];
    } OAM_BUF;

    //palette memory
    typedef struct _Palette {
        Byte image_palette[kMAX_CUR_PALET];
        Byte sprite_palette[kMAX_CUR_PALET];
    } Palette;

    typedef union _PPU_CTRL {
        struct {
            Byte NN : 2;//nametable select;
            Byte I : 1; //increment mode;
            Byte S : 1; //sprite tile select;
            Byte B : 1; //background tile select;
            Byte H : 1; //sprite height;
            Byte P : 1; //PPU master/slave mode;
            Byte V : 1; //NMI enable/ V-Blank enable;
        };
        Byte val;
    } PPU_CTRL;

    typedef union _PPU_MASK {
        struct {
            Byte G : 1;  //greyscale;
            Byte m : 1;  //background left column enable;
            Byte M : 1;  //sprite left column enable;
            Byte b : 1;  //background enable;
            Byte s : 1;  //sprite enable;
            Byte BGR : 3;//color emphasis;
        };
        Byte val;
    } PPU_MASK;

    typedef union _PPU_STATUS {
        struct {
            Byte dummy0 : 5;
            Byte O : 1; //Sprite Overflow Flag;
            Byte S : 1; //Sprite 0 Hit Flag;
            Byte V : 1; //V-Blank Flag;
        };
        Byte val;
    } PPU_STATUS;

    typedef union _PPU_REG {
        struct {
            PPU_CTRL ppu_ctrl;
            PPU_MASK ppu_mask;
            PPU_STATUS ppu_status;
            Byte oam_addr;
            Byte oam_data;
            Byte ppu_scroll;
            Byte ppu_addr;
            Byte ppu_data;
        };
        Byte vals[8];
    } PPU_REG;

};//end nes

#endif