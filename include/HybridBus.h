#ifndef HYBRIDBUS_H
#define HYBRIDBUS_H

#include "NESDef.h"
namespace nes {

    class HybridBus {
    public:
        bool write(Word addr, Byte data){
            if (addr < 0x2000){//pattern table
                //depend on mapper;
            }
            else if (addr < 0x3000){//NameTable/AttributeTable
                //depend on mapper;
            }
            else if (addr < 0x3F00){//mirrors of $2000 - $2EFF;
                //The PPU does not render from this address range, 
                //so this space has negligible utility.
            }
            else if (addr < 0x3F20){//palette;

            }
            else if (addr < 0x4000){}
            else return false;
        }
        
    private:
        Palette *palette;
    };



};//end nes

#endif

0010 1111 0000 0000 : $2F00

$3000 :-> $0000
$3001 :-> $0001

$3EFF :-> $2EFF