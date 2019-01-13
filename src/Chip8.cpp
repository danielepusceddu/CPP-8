#include "Chip8.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>

Chip8::Chip8(std::string romFilename)
//Seed the random engine
: randEng{std::chrono::high_resolution_clock::now().time_since_epoch().count()}
{
    //Clear the screen
    screen.fill(false);

    //Place the font in memory
    std::copy(hexSprites.begin(), hexSprites.end(), mem.begin());

    //TODO: Load Rom
}


void Chip8::step()
{
    //Opcodes are made of 2 bytes each.
    std::uint8_t high = mem[PC];
    std::uint8_t low = mem[PC+1];

    //Get the various parts of the opcode
    //For NXYN instructions
    std::uint8_t x = high & 0x0F;
    std::uint8_t y = low >> 4;

    //For ?NNN instructions
    std::uint16_t nnn = getNNN(high, low);

    //By default the PC will be pointed to the next opcode.
    //But this variable might be modified by jump instructions and others.
    std::uint16_t nextAddr = PC + 2;


    //Find out which opcode we're dealing with
    switch(high & 0xF0){
        //opcodes starting with 0
        case 0x00:
            switch(low){
                //00E0 - CLS
                //Clear the display.
                case 0xE0:
                    screen.fill(false);
                break;

                //00EE - RET
                //Set PC to address at top of the stack, then dec SP
                case 0xEE:
                    nextAddr = stack[SP - 1];
                    SP--;
                break;

                default:
                    reportCode(high, low);
                break;
            }
        break;

        //1NNN - JP ADDR
        //Set PC to NNN
        case 0x10:
            nextAddr = nnn;
        break;

        //2NNN - CALL ADDR
        //Inc SP, then put current PC on top of stack. Then PC = NNN
        case 0x20:
            SP++;
            stack[SP - 1] = PC;
            nextAddr = nnn;
        break;

        //3XKK - SE VX, BYTE
        //Skip next instruction if Vx == kk
        case 0x30:
            if(V[x] == low){
                nextAddr = PC + 4;
            }
        break;

        //4XKK - SNE VX, BYTE
        //Skip next instruction if Vx != kk
        case 0x40:
            if(V[x] != low){
                nextAddr = PC + 4;
            }
        break;

        //opcodes starting with 5
        case 0x50:
            switch(low & 0x0F){
                //5XY0 - SE Vx, Vy
                //Skip next instruction if Vx == Vy
                case 0x00:
                    if(V[x] == V[y]){
                        nextAddr = PC + 4;
                    }
                break;

                default:
                    reportCode(high, low);
                break;
            } //End 5??? switch
        break;

        //6XKK - LD Vx, byte
        //Set Vx = kk
        case 0x60:
            V[x] = low;
        break;

        //7XKK - ADD Vx, byte
        case 0x70:
            V[x] += low;
        break;

        //opcodes starting with 8
        case 0x80:
            switch(low & 0x0F){
                //8XY0 - LD Vx, Vy
                //Set Vx = Vy
                case 0x00:
                    V[x] = V[y];
                break;

                //8XY1 - OR Vx, Vy
                //Set Vx = Vx OR Vy
                case 0x01:
                    V[x] |= V[y];
                break;

                //8XY2 - AND Vx, Vy
                //Set Vx = Vx AND Vy
                case 0x02:
                    V[x] &= V[y];
                break;

                //8XY3 - XOR Vx, Vy
                //Set Vx = Vx XOR Vy
                case 0x03:
                    V[x] ^= V[y];
                break;

                //8XY4 - ADD Vx, Vy
                //Set Vx = Vx + Vy. 
                //Set VF to 1 if result is greater than 255
                //Otherwise, set VF to 0
                //Only the 8 lowest bits of the result are stored in Vx
                case 0x04:{
                    std::uint16_t result = V[x] + V[y];
                    V[0xF] = (result > 255) ? 1 : 0;
                    V[x] = result & 0x00FF;
                }
                break;

                //8XY5 - SUB Vx, Vy
                //VF is set to NOT borrow.
                //If Vx > Vy, VF is set to 1, otherwise 0.
                //Then Vx = Vx - Vy
                case 0x05:
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];
                break;

                //8XY6 - SHR VX
                //If least-significant bit of Vx is 1, then VF is set to 1.
                //Otherwise, it is set to 0.
                //Then Vx is divided by 2.
                //Y seems to be ignored.
                case 0x06:
                    V[0xF] = V[x] & 1;
                    V[x] >>= 1;
                break;

                //8XY7 - SUBN Vx, Vy
                //Vx = Vy - Vx
                //Set VF = NOT borrow
                case 0x07:
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                break;

                //8XYE - SHL VX
                //If most-significant bit of Vx is 1, then VF is set to 1.
                //Otherwise, it is set to 0.
                //Then Vx is multiplied by 2.
                //Y seems to be ignored.
                case 0x0E:
                    V[0xF] = (V[x] & 128) >> 7;
                    V[x] <<= 1;
                break;

                default:
                    reportCode(high, low);
                break;
            } //End 8??? switch
        break;

        //opcodes starting with 9
        case 0x90:
            switch(low & 0x0F){
                //SNE Vx, Vy
                //Skip next instruction if Vx != Vy
                case 0x00:
                    if(V[x] != V[y]){
                        nextAddr = PC + 4;
                    }
                break;

                default:
                    reportCode(high, low);
                break;
            } //End 9??? switch
        break;

        //ANNN - LD I, ADDR
        //Set I = NNN
        case 0xA0:
            I = nnn;
        break;

        //BNNN - JP V0, ADDR
        //JMP to NNN + V0
        case 0xB0:
            nextAddr = nnn + V[0];
        break;

        //CXKK - RND Vx, byte
        //Set Vx = randBye AND kk
        case 0xC0:
            V[x] = intDist(randEng) & low;
        break;

        //DXYN - DRW Vx, Vy, nibble
        //Display N-Bythe sprite starting at memory location I,
        //placing it at (Vx, Vy).
        //Set VF = collision.
        case 0xD0:
            V[0xF] = drawSprite(V[x], V[y], I, low & 0x0F);
        break;

    } //End first nibble switch

    //Go to next address
    PC = nextAddr;
}


//Helper method for XNNN instructions
std::uint16_t getNNN(std::uint8_t high, std::uint8_t low){
    std::uint16_t nnn = high & 0x0F;
    nnn <<= 8;
    nnn += low;
    return nnn;
}


//Report an unknown opcode
void reportCode(std::uint8_t high, std::uint8_t low){
    std::cerr << "Unknown Opcode: " << std::hex << high << low << std::dec << std::endl;
}



//Helper method for draw instruction
//Returns true if collifion happened
bool Chip8::drawSprite(int x, int y, std::uint16_t addr, std::size_t len){
    bool collision = false;

    //Each byte is a sprite row containing 8 pixels
    for(int spriteY = 0; spriteY < len; spriteY++){

        for(int spriteX = 0; spriteX < 8; spriteX++){
            //Take the sprite's row. AND it with power of 2
            //Shift the result so we get the pixel boolean
            bool spritePixel = (mem[addr + spriteY] & (128 >> spriteX)) >> (7 - spriteX);
            
            //Get a reference to the screen's pixel
            bool& screenPixel = screen[((y + spriteY) * 64) + (x + spriteX)]; 

            //Collision is true if both pixels are 1.
            //The pixel will be erased as result of the XOR
            if(screenPixel && spritePixel)
                collision = true;

            //XOR the pixel onto the screen
            screenPixel ^= spritePixel;
        } 
    }
    return collision;
}