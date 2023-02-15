#ifndef NESDEF_H
#define NESDEF_H

#include<cstdint>

namespace nes {

	using Word = std::uint16_t;
	using Byte = std::uint8_t;
	using Counter = signed long long; //counter should prevent decrementing from 0 to a huge number;

	constexpr Word kMAX_INSTR_MTX_SIZE = 0x100;  //max size of instruction matrix;

	constexpr Word kMAX_ZERO_PAGE = 0x100;       //max size of zero page;
	constexpr Word kMAX_SYS_STACK = 0x100;       //max size of system stack;
	constexpr Word kSTACK_BASE = 0x100;          //base address of system stack;
	constexpr Word kMAX_INTER_RAM = 0x600;       //max size of internal assigned RAM;
	constexpr Word kMAX_RAM = 0x800;             //max size of physical CPU RAM;

	constexpr Word kNMI_VECTOR = 0xFFFA;         //low byte address of nmi handler vector;
	constexpr Word kRESET_VECTOR = 0xFFFC;       //low byte address of reset handler vector;
	constexpr Word kIRQ_VECTOR = 0xFFFE;         //low byte address of irq handler vector;

	constexpr bool signof(Byte val) { return (val & (Byte)0x80); }
	constexpr bool signof(Word val) { return (val & (Word)0x0080); }

	constexpr Word kMAX_VRAM = 0x800;            //max size of PPU Vedio RAM;
	constexpr Word kMAX_NAME_TBL = 0x3C0;        //max size of name table byte;
	constexpr Word kMAX_ATTR_TBL = 0x40;         //max size of attribute table byte;
	constexpr Word kOBJ_ATTR_SIZE = 0x4;         //size of one object attribute entry, byte;
	constexpr Word kMAX_OBJ_ATTR = 0x40;         //max number of object attribute entry within OAM;
	constexpr Word kMAX_OAM_BUFF = 0x8;          //max number of object attribute entry within secondary OAM buffer;
	constexpr Word kMAX_CUR_PALET = 0x10;        //size of one palette memory for background/sprite;

	constexpr Word kNAME_TBL_BASE = 0x2000;      //base address of name table;
	constexpr Word kATTR_TBL_BASE = 0x23C0;      //base address of attribute table;
	constexpr Word kPALETTE_BASE = 0x3F00;       //base address of palette memory;

	constexpr Word kSPR_HEIGHT_8 = 0x8;          //constant value of the sprite height;
	constexpr Word kSPR_HEIGHT_16 = 0x10;        //constant value of the sprite height;

	constexpr Word kNES_HEAD_SIZE = 0x10;        //size of header of iNES 1.0 file format;
	constexpr Word kTRAINER_SIZE = 0x200;        //size of trainer of iNES 1.0 file format;
	constexpr Word kPRG_ROM_SIZE = 0x4000;       //size of programable-ROM of iNES 1.0 file format;
	constexpr Word kCHR_ROM_SIZE = 0x2000;       //size of character-ROM of iNES 1.0 file format;;
	constexpr Word kPRG_RAM_SIZE = 0x2000;       //size of programable-RAM of iNES 1.0 file format;

	//CPU status flag;
	enum FLAG {
		C = 0b0000'0001,
		Z = 0b0000'0010,
		I = 0b0000'0100,
		D = 0b0000'1000,
		B = 0b0001'0000,
		U = 0b0010'0000,
		V = 0b0100'0000,
		N = 0b1000'0000,
	};

	enum class Instr_Phase {
		instr_fetch,
		execute,
		extra,
		interrupt,
		reset,
	};

	enum IRQ_SRC : Byte {
		External =		0b0001,
		FrameCounter =	0b0010,
		DMC =			0b0100,
	};

	enum Mapper_Type : Byte {
		NROM = 0x00,
		MMC1 = 0x01,
		UxROM = 0x02,
		Mapper_003 = 0x03,
		MMC3 = 0x04,
		// ...
	};

	//name table mirroring;
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

	//CPU status flag register;
	union STATUS_FLAGS {
		struct {
			Byte C : 1;      //carry;              bit0
			Byte Z : 1;      //zero
			Byte I : 1;      //interrupt disable
			Byte D : 1;      //decimal mode
			Byte B : 1;      //break command
			Byte U : 1;      //unused
			Byte V : 1;      //overflow
			Byte N : 1;      //negative;           bit7
		};
		Byte val;

		STATUS_FLAGS() {
			val = 0;
		}
	};

	struct Interrupt_Mask {
		Byte val;
	};

	//RAM for CPU, 2KB;
	struct RAM {
		Byte zero_page [kMAX_ZERO_PAGE];
		Byte sys_stack [kMAX_SYS_STACK];
		Byte inter_ram [kMAX_INTER_RAM];

		Byte &operator[](Word _addr){
			return zero_page[_addr];
		}
		void reset() {
			size_t* it = reinterpret_cast<size_t*>(this);
			size_t* itend = reinterpret_cast<size_t*>(&inter_ram[kMAX_INTER_RAM]);
			while (it < itend) {
				*it = 0;
				++it;
			}
		}
	};

	//I/O registers in CPU addressing space, 32 Byte;
	struct IO_REG {
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
	};

	//RAM for PPU, 2KB;
	struct VRAM {
		Byte name_table_0        [kMAX_NAME_TBL];
		Byte attribute_table_0   [kMAX_ATTR_TBL];
		Byte name_table_1		 [kMAX_NAME_TBL];
		Byte attribute_table_1	 [kMAX_ATTR_TBL];
		
		Byte &operator[](Word _addr){
			return name_table_0[_addr];
		}
		void reset() {
			size_t* it = reinterpret_cast<size_t*>(this);
			size_t* itend = reinterpret_cast<size_t*>(&attribute_table_1[kMAX_ATTR_TBL]);
			while (it < itend) {
				*it = 0;
				++it;
			}
		}
	};

	//object attribute entry, 4 Byte;
	struct OBJ_ATTR {
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
	};

	//Object Attribute Management, 256 Byte
	struct OAM {
		OBJ_ATTR obj_attr[kMAX_OBJ_ATTR];

		Byte &operator[](Word _addr){
			return reinterpret_cast<Byte*>(this)[_addr];
		}
	};

	//secondary OAM, 32 Byte;
	struct OAM_BUF {
		OBJ_ATTR obj_attr[kMAX_OAM_BUFF];

		Byte &operator[](Word _addr){
			return reinterpret_cast<Byte*>(this)[_addr];
		}
	};

	//palette memory, 32 Byte;
	struct Palette {
		Byte image_palette[kMAX_CUR_PALET];
		Byte sprite_palette[kMAX_CUR_PALET];

		Byte &operator[](Word _addr){
			return image_palette[_addr];
		}
	};

	struct PPU_CTRL {
		Byte nt_select : 2;//nametable select;              bit 0
		Byte incr_mode : 1; //increment mode;
		Byte spr_select : 1; //sprite tile select;
		Byte bkgr_select : 1; //background tile select;
		Byte sprite_h : 1; //sprite height;
		Byte slave_mode : 1; //PPU master/slave mode;
		Byte nmi_enable : 1; //NMI enable/ V-Blank enable;  bit 7

		PPU_CTRL() {
            *reinterpret_cast<Byte*>(this) = 0;
		}
		Byte &operator=(Byte _data){
			return *reinterpret_cast<Byte*>(this) = _data;
		}
	};

	struct PPU_MASK {
		Byte greyscale : 1;  //greyscale;
		Byte bkgr_col_enable : 1;  //background left column enable;
		Byte spr_col_enable : 1;  //sprite left column enable;
		Byte bkgr_enable : 1;  //background enable;
		Byte spr_enable : 1;  //sprite enable;
		Byte color_emphasis : 3;//color emphasis;
		
		PPU_MASK() {
			*reinterpret_cast<Byte*>(this) = 0;
		}
		Byte &operator=(Byte _data){
			return *reinterpret_cast<Byte*>(this) = _data;
		}
	};

	struct PPU_STATUS {
		Byte dummy0 : 5;
		Byte sprite_overflow : 1; //Sprite Overflow Flag;   bit 5
		Byte sprite_hit : 1; //Sprite 0 Hit Flag;           bit 6
		Byte vblank_flag : 1; //V-Blank Flag;               bit 7
		
		PPU_STATUS() {
			*reinterpret_cast<Byte*>(this) = 0;
		}
		Byte &operator=(Byte _data){
			return *reinterpret_cast<Byte*>(this) = _data;
		}
		Byte operator&(Byte _val) {
			return *reinterpret_cast<Byte*>(this) & _val;
		}
	};

	//PPU's registers, 8 Byte;
	// typedef struct _PPU_REG {
	//     PPU_CTRL ppu_ctrl;     //$2000 : VPHB SINN : NMI enable (V), PPU master/slave (P), sprite height (H), background tile select (B), sprite tile select (S), increment mode (I), nametable select (NN);
	//     PPU_MASK ppu_mask;     //$2001 : BGRs bMmG : color emphasis (BGR), sprite enable (s), background enable (b), sprite left column enable (M), background left column enable (m), greyscale (G);
	//     PPU_STATUS ppu_status; //$2002 : VSO- ---- : vblank (V), sprite 0 hit (S), sprite overflow (O); read resets write pair for $2005/$2006
	//     Byte oam_addr;         //$2003 : OAM read/write address;
	//     Byte oam_data;         //$2004 : OAM data read/write;
	//     Byte ppu_scroll;       //$2005 : fine scroll position (two writes: X scroll then Y scroll);
	//     Byte ppu_addr;         //$2006 : PPU read/write address (two writes: MSB then LSB);
	//     Byte ppu_data;         //$2007 : PPU data read/write 
	//     Byte &operator[](Word _addr){
	//         return reinterpret_cast<Byte*>(this)[_addr];
	//     }
	// } PPU_REG;

	//PPU internal register format for scrolling, defined by loopy;
	union LOOPY_REG {
		struct {
			//coarse_x * coarse_y can address 1 KB NameTable;
			Word coarse_x	: 5;//0-31 x-axis within NameTable;(unit : Byte)
			Word coarse_y	: 5;//0-31 y-axis within NameTable;
			Word nt_select	: 2;//selcet from 2 1KB-NameTable;
			Word fine_y		: 3;//fine_x * fine_y address 8*8 pixels within a tile;
			Word dummy0		: 1;//unused bit;
		};
		struct {
			Word low_byte   : 8;
			Word high_byte	: 6;
			Word msb		: 1;
			Word dummy1		: 1;
		};
		struct {
			Word nt_fetch_addr	: 12;
			Word dummy2			: 4;
		};
		struct {
			Word dummy3	: 2;
			Word at_x	: 3;
			Word dummy4 : 2;
			Word at_y	: 3;
			Word dummy5 : 6;
		};
		Word val;//used as a 14-bits VRAM addr;
		
		LOOPY_REG() {
			val = 0;
		}
		Word get_nt_addr(){
			// 0x2000 | (SL YYYYY XXXXX)
			return kNAME_TBL_BASE | nt_fetch_addr;
		}
		Word get_at_addr(){
			//Base address of attribute table : 0x23C0;
			//nt_select keep its position to select table;
			//Since the area of attribute tile is more coarse than pattern tile
			//	only the upper 3 bits of coarse_y are used as part of address;
			//So as do the upeer 3 bits of coarse_x;
			return kATTR_TBL_BASE | (nt_select << 10) | (at_y << 3) | at_x;
		}
		void inc_x(bool render_enabled){
			if (render_enabled){
				//Note: incrementing x has to be separated from incrementing y;
				//because when reaching the boundary of one name table,
				//it is not necessarily that coarse_y should be incremented;
				//rather, when camera scrolling crossed two nametable fields,
				//address should jump to the adjacent nametable according to mirroring;
				if (coarse_x == 31){
					coarse_x = 0;
					nt_select ^= 0x1;
				}
				else ++coarse_x;
			}
		}
		void inc_y(bool render_enabled){
			if (render_enabled){
				//y coordinate should only be incremented according to rendering routine;
				//but not be influenced by increment of x coordinate;
				if (fine_y < 7) ++fine_y;
				else {
					//fine_y == 7 indicates a tile is completely rendered;
					//so it is time to pick up the next tile;
					fine_y = 0;
					if (coarse_y == 29){
						coarse_y = 0;
						nt_select ^= 0b10;
					}
					//else if (coarse_y == 31) coarse_y = 0;
					else ++coarse_y;
				}
			}
		}
		void transfer_x(LOOPY_REG &reg){
			coarse_x = reg.coarse_x;
			nt_select |= reg.nt_select & 0x1;
		}
		void transfer_y(LOOPY_REG &reg){
			coarse_y = reg.coarse_y;
			fine_y = reg.fine_y;
			nt_select |= reg.nt_select & 0b10;
		}
	};

	struct NESHeader {
		char NES1A[4];           //Btye 0-3
		Byte num_prg_rom;        //Btye 4
		Byte num_chr_rom;        //Byte 5
		//Byte 6:            <-    Byte 6
		Byte mirror_hv : 1;      //0:horizontal; 1:vertical;
		Byte save_ram : 1;       //Cartridge contains battery-backed PRG RAM ($6000-7FFF) 
						         //		or other persistent memory
		Byte trainer : 1;		 //512-byte trainer at $7000-$71FF (stored before PRG data)
		Byte four_screen : 1;    //		within cart provide extra VRAM;
		Byte n_mapper_low : 4;   //lower 4bits of mapper, stood at upper 4bits in Byte 6;
		//Byte 6 end;
		//Byte 7:            <-    Byte 7
		Byte vs_unisys : 1;
		Byte playchoice : 1;
		Byte nes_2_sign : 2;
		Byte n_mapper_high : 4;  //upper 4bits of mapper, at upper 4bits in Byte 7;
		//Byte 7 end;
		Byte num_prg_ram;        //Byte 8
		char tailBytes[7];       //Byte 9-15

		Byte &operator[](Word _addr){
			return reinterpret_cast<Byte*>(this)[_addr];
		}
		Byte n_mapper() const {
			return static_cast<Byte>((n_mapper_high << 4) | n_mapper_low);
		}
	};

};//end nes

#endif