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
    constexpr size_t kOBJ_ATTR_SIZE = 0x4;
    constexpr size_t kMAX_OBJ_ATTR = 0x40;
    constexpr size_t kMAX_OAM_BUFF = 0x8;
    constexpr size_t kMAX_CUR_PALET = 0x10;

    constexpr Word kPALETTE_BASE = 0x3F00;

    constexpr size_t kSPR_HEIGHT_8 = 0x8;
    constexpr size_t kSPR_HEIGHT_16 = 0x10;

    constexpr size_t kNES_HEAD_SIZE = 0x10;
    constexpr size_t kTRAINER_SIZE = 0x200;
    constexpr size_t kPRG_ROM_SIZE = 0x4000;
    constexpr size_t kCHR_ROM_SIZE = 0x2000;
    constexpr size_t kPRG_RAM_SIZE = 0x2000;

    enum Mapper_Type : Byte {
        NROM = 0x00,
        MMC1 = 0x01,
        UxROM = 0x02,
        Mapper_003 = 0x03,
        MMC3 = 0x04,
        // ...
    };

    enum NT_Mirror {
        Horizontal = 0,
        Vertical = 1,
        Single_Screen,
        Four_screen,
        Others
    };

    enum NES_VER {
        iNES_1_0 = 0x0,
        archaic_iNES = 0x4,
        NES_2_0 = 0x8,
    };

    //RAM for CPU, 2KB;
    typedef struct _RAM {
        Byte zero_page[kMAX_ZERO_PAGE];
        Byte sys_stack[kMAX_SYS_STACK];
        Byte inter_ram[kMAX_INTER_RAM];

        Byte &operator[](Word _addr){
            return zero_page[_addr];
        }
    } RAM;

    //I/O registers in CPU addressing space, 32 Byte;
    typedef struct _IO_REG {
        //Pulse 1:
        Byte sq1_vol;   //$4000 : duty cycle and volume;
        Byte sq1_sweep; //$4001 : sweep control;
        Byte sq1_lo;    //$4002 : low byte of period;
        Byte sq1_hi;    //$4003 : high byte of period & length counter value;
        //Pulse 2:
        Byte sq2_vol;   //$4004 : duty cycle and volume;
        Byte sq2_sweep; //$4005 : sweep control;
        Byte sq2_lo;    //$4006 : low byte of period;
        Byte sq2_hi;    //$4007 : high byte of period & length counter value;
        //Triangle:
        Byte tri_linear;//$4008 : linear counter;
        Byte dummy0;    //$4009 : unused;
        Byte tri_lo;    //$400A : low byte of period;
        Byte tri_hi;    //$400B : hight byte of period & length counter value;
        //Noise:
        Byte noise_vol; //$400C : volume;
        Byte dummy1;    //$400D : unused;
        Byte noise_lo;  //$400E : period & waveform shape;
        Byte noise_hi;  //$400F : length counter value;
        //DMC: (APU's delta modulation channel)
        Byte dmc_freq;  //$4010 : IRQ flag, loop flag, frequency;
        Byte dmc_raw;   //$4011 : 7-bit DAC;
        Byte dmc_start; //$4012 : Start address = $C000 + $40*$xx ;
        Byte dmc_len;   //$4013 : Sample length = $10*$xx + 1 bytes (128*$xx + 8 samples);
        //Others:
        Byte oam_dma;   //$4014 : OAM DMA control;
        Byte snd_chn;   //$4015 : write: sound channels enable; read: sound channels & IRQ status;
        Byte joy_1;     //$4016 : write: joystick strobe; read: joystick 1 data;
        Byte joy_2;     //$4017 : write: frame counter control; read: joystick 2 data;
        //Unused:
        Byte dummy2[0x4020 - 0x4018]; //APU and I/O functionality that is normally disabled;

        Byte &operator[](Word _addr){
            return reinterpret_cast<Byte*>(this)[_addr];
        }
    } IO_REG;

    //RAM for PPU, 2KB;
    typedef struct _VRAM {
        Byte name_table_0[kMAX_NAME_TBL];
        Byte attribute_table_0[kMAX_ATTR_TBL];
        Byte name_table_1[kMAX_NAME_TBL];
        Byte attribute_table_1[kMAX_ATTR_TBL];
        
        Byte &operator[](Word _addr){
            return name_table_0[_addr];
        }
    } VRAM;

    //object attribute entry, 4 Byte;
    typedef struct _OBJ_ATTR {
        Byte y_coord_1;
        Byte index;
        //attribute Byte
        Byte n_palette : 2;
        Byte dummy0 : 3;
        Byte prior : 1;
        Byte flip_h : 1;
        Byte flip_v : 1;
        //attribute end
        Byte x_coord;

        Byte &operator[](Word _addr){
            return (&y_coord_1)[_addr];
        }
    } OBJ_ATTR;

    //Object Attribute Management, 256 Byte
    typedef struct _OAM {
        OBJ_ATTR obj_attr[kMAX_OBJ_ATTR];

        Byte &operator[](Word _addr){
            return reinterpret_cast<Byte*>(this)[_addr];
        }
    } OAM;

    //secondary OAM, 32 Byte;
    typedef struct _OAM_BUF {
        OBJ_ATTR obj_attr[kMAX_OAM_BUFF];

        Byte &operator[](Word _addr){
            return reinterpret_cast<Byte*>(this)[_addr];
        }
    } OAM_BUF;

    //palette memory, 32 Byte;
    typedef struct _Palette {
        Byte image_palette[kMAX_CUR_PALET];
        Byte sprite_palette[kMAX_CUR_PALET];

        Byte &operator[](Word _addr){
            return image_palette[_addr];
        }
    } Palette;

    typedef struct _PPU_CTRL {
        Byte nt_select : 2;//nametable select;
        Byte incr_mode : 1; //increment mode;
        Byte spr_select : 1; //sprite tile select;
        Byte bkgr_select : 1; //background tile select;
        Byte sprite_h : 1; //sprite height;
        Byte agent_mode : 1; //PPU master/slave mode;
        Byte nmi_enable : 1; //NMI enable/ V-Blank enable;

        Byte &operator=(Byte _data){
            return *reinterpret_cast<Byte*>(this) = _data;
        }
    } PPU_CTRL;

    typedef struct _PPU_MASK {
        Byte greyscale : 1;  //greyscale;
        Byte bkgr_col_enable : 1;  //background left column enable;
        Byte spr_col_enable : 1;  //sprite left column enable;
        Byte bkgr_enable : 1;  //background enable;
        Byte spr_enable : 1;  //sprite enable;
        Byte color_emphasis : 3;//color emphasis;
        
        Byte &operator=(Byte _data){
            return *reinterpret_cast<Byte*>(this) = _data;
        }
    } PPU_MASK;

    typedef struct _PPU_STATUS {
        Byte dummy0 : 5;
        Byte sprite_overflow : 1; //Sprite Overflow Flag;
        Byte sprite_hit : 1; //Sprite 0 Hit Flag;
        Byte vblank_flag : 1; //V-Blank Flag;
        
        Byte &operator=(Byte _data){
            return *reinterpret_cast<Byte*>(this) = _data;
        }
    } PPU_STATUS;

    //PPU's registers, 8 Byte;
    typedef struct _PPU_REG {
        PPU_CTRL ppu_ctrl;     //$2000 : VPHB SINN : NMI enable (V), PPU master/slave (P), sprite height (H), background tile select (B), sprite tile select (S), increment mode (I), nametable select (NN);
        PPU_MASK ppu_mask;     //$2001 : BGRs bMmG : color emphasis (BGR), sprite enable (s), background enable (b), sprite left column enable (M), background left column enable (m), greyscale (G);
        PPU_STATUS ppu_status; //$2002 : VSO- ---- : vblank (V), sprite 0 hit (S), sprite overflow (O); read resets write pair for $2005/$2006
        Byte oam_addr;         //$2003 : OAM read/write address;
        Byte oam_data;         //$2004 : OAM data read/write;
        Byte ppu_scroll;       //$2005 : fine scroll position (two writes: X scroll then Y scroll);
        Byte ppu_addr;         //$2006 : PPU read/write address (two writes: MSB then LSB);
        Byte ppu_data;         //$2007 : PPU data read/write
        
        Byte &operator[](Word _addr){
            return reinterpret_cast<Byte*>(this)[_addr];
        }
    } PPU_REG;

    //PPU internal registers for scrolling, defined by loopy;
    typedef union _LOOPY_REG {
        struct {
            //coarse_x * coarse_y can address 1 KB NameTable;
            Word coarse_x : 5;//0-31 x-axis within NameTable;(unit Byte)
            Word coarse_y : 5;//0-31 y-axis within NameTable; 
            Word nt_select : 2;//selcet from 2 1KB-NameTable;
            Word fine_y : 3;//fine_x * fine_y address 8*8 pixels within a tile;
            Word dummy0 : 1;//unused bit;
        };
        struct {
            Byte low_byte;
            Byte high_byte : 6;
            Byte msb : 1;
            Byte dummy1 : 1;
        };
        Word val;//used as a 14-bits VRAM addr;
        
        void incr_scroll_x(bool render_enabled){
            if (render_enabled){
                if (coarse_x == 31){
                    coarse_x = 0;
                    nt_select ^= 0x1;
                }
                else ++coarse_x;
            }
        }
        void incr_scroll_y(bool render_enabled){
            if (render_enabled){
                if (fine_y < 7) ++fine_y;
                else {
                    //fine_y == 7 indicates a tile is completely rendered;
                    //so it is time to pick up the next tile;
                    fine_y = 0;
                    if (coarse_y == 29){
                        coarse_y = 0;
                        nt_select ^= 0b10;
                    }
                    else if (coarse_y == 31) coarse_y = 0;
                    else ++coarse_y;
                }
            }
        }
        void transfer_addr_x(_LOOPY_REG &reg){
            coarse_x = reg.coarse_x;
            nt_select |= reg.nt_select & 0x1;
        }
        void transfer_addr_y(_LOOPY_REG &reg){
            coarse_y = reg.coarse_y;
            fine_y = reg.fine_y;
            nt_select |= reg.nt_select & 0b10;
        }
    } LOOPY_REG;

    // typedef struct _LOOPY_REG {
    //     Word coarse_x : 5;
    //     Word coarse_y : 5;
    //     Word nt_select : 2;
    //     Word fine_y : 3;
    //     Word dummy0 : 1;

    //     Word &operator=(Word val){
    //         return *reinterpret_cast<Word*>(this) = val;
    //     }
    //     Byte &get_low(){
    //         return *reinterpret_cast<Byte*>(this);
    //     }
    //     Byte &get_high(){
    //         return *(reinterpret_cast<Byte*>(this) + 1);
    //     }
    //     Byte &get_msb(){
    //         return 
    //     }

    // } LOOPY_REG;

    typedef struct _NESHeader {
        char NES1A[4];           //Btye 0-3
        Byte num_prg_rom;        //Btye 4
        Byte num_chr_rom;        //Byte 5
        //Byte 6:            <-    Byte 6
        Byte mirror_hv : 1;//0:horizontal; 1:vertical;
        Byte save_ram : 1;//Cartridge contains battery-backed PRG RAM ($6000-7FFF) 
                          //or other persistent memory
        Byte trainer : 1;//512-byte trainer at $7000-$71FF (stored before PRG data)
        Byte four_screen : 1;//within cart provide extra VRAM;
        Byte n_mapper_low : 4;
        //Byte 6 end;
        //Byte 7:            <-    Byte 7
        Byte vs_unisys : 1;
        Byte playchoice : 1;
        Byte nes_2_sign : 2;
        Byte n_mapper_high : 4;
        //Byte 7 end;
        Byte num_prg_ram;        //Byte 8
        char tailBytes[7];       //Byte 9-15

        Byte &operator[](Word _addr){
            return reinterpret_cast<Byte*>(this)[_addr];
        }
        Byte n_mapper() const {
            Byte ret = n_mapper_high;
            ret << 4;
            return static_cast<Byte>(ret | n_mapper_low);
        }
    } NESHeader;

};//end nes

#endif