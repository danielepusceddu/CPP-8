#include "Chip8.hpp"
#include <thread>
#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

//This method attempts copying the contents of
//a file to chip8's RAM, starting at addr 0x200
//The file might be too big to fit in the RAM,
//or we might be unable to open it.
//In that case, this method will throw the appropriate exception.
Chip8::Chip8(std::string romFilename, int outputScale)
//Seed the random engine
: randEng{static_cast<unsigned long>(std::chrono::high_resolution_clock::now().time_since_epoch().count())}
{
    std::ifstream rom{romFilename, std::ios::in | std::ios::binary};
    
    //If file could not be opened
    if(!rom){
        std::cerr << "File \"" << romFilename << "\"not found!" << std::endl;
        throw FileNotFound{};
    }
    else{
        //Might throw rom too big exception
        loadRom(rom);

        //Clear the screen
        screen.fill(false);

        //Set all keys to up
        keys.fill(false);

        //Place the font in memory
        std::copy(hexSprites.begin(), hexSprites.end(), mem.begin());

        //set scale if valid
        if(outputScale > 0){
            scale = outputScale;
        }
        else{
            std::cerr << "Bad scale parameter\n";
        }
    }
}

//Set chip48 mode
void Chip8::setChip48(bool b){
    chip48 = b;
}


int Chip8::getScale(){
    return scale;
}

void Chip8::run(){
    msBuf=timeBetweenCycles;
    lastCycle=Clock::now();
    running = true;

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(mainLoopFunc_emscripten, this, hz, 1);
    #else

    while(running){
        mainLoopFunc();
        std::this_thread::sleep_for(timeBetweenCycles);
    }

    #endif
}

void Chip8::mainLoopFunc(){
    //If intepreter is not paused, do a full cycle
    if(pause == false){
        time now{Clock::now()};
        ms sinceLastCycle = std::chrono::duration_cast<ms>(now - lastCycle);

        //Step for each timeBetweenCycles in msBuf
        for(msBuf += sinceLastCycle; msBuf >= timeBetweenCycles; msBuf -= timeBetweenCycles){
            handleInput();
            decrementTimer(delayTimer);

            //Play sound if soundTimer was decremented to 0
            if(decrementTimer(soundTimer) && soundTimer.ticks == 0){
                playSound();
            }
        
            step();

            if(screenUpdated){
                draw(screen);
                screenUpdated = false;
            }
            lastCycle = now;
        }
    }
    //If interpreter is paused, just check for input
    else{
        handleInput();
    } 
}


void Chip8::mainLoopFunc_emscripten(void* chip8ptr){
    static_cast<Chip8*>(chip8ptr)->mainLoopFunc();
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
                    screenUpdated = true;
                break;

                //00EE - RET
                //Set PC to the instruction after the one pointed by the top of the stack, then dec SP
                case 0xEE:
                    nextAddr = stack.top() + 2;
                    stack.pop();
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
            stack.push(PC);
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

                //This instruction has 2 versions
                case 0x06:
                    //CHIP-48 / SUPER CHIP-8
                    //8XY6 - SHR VX
                    //If least-significant bit of Vx is 1, then VF is set to 1.
                    //Otherwise, it is set to 0.
                    //Then Vx is divided by 2.
                    //Y seems to be ignored.
                    if(chip48){
                        V[0xF] = V[x] & 1;
                        V[x] >>= 1;
                    }

                    //CHIP-8
                    //Store the value of register VY shifted right one bit in register VX
                    //Set register VF to the least significant bit prior to the shift
                    else{
                        V[0xF] = V[y] & 1;
                        V[x] = V[y] >> 1;
                    }
                break;

                //8XY7 - SUBN Vx, Vy
                //Vx = Vy - Vx
                //Set VF = NOT borrow
                case 0x07:
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                break;

                //This instruction has 2 versions
                case 0x0E:
                    //CHIP-48 / SUPER CHIP-8
                    //8XYE - SHL VX
                    //If most-significant bit of Vx is 1, then VF is set to 1.
                    //Otherwise, it is set to 0.
                    //Then Vx is multiplied by 2.
                    //Y seems to be ignored.
                    if(chip48){
                        V[0xF] = (V[x] & 128) >> 7;
                        V[x] <<= 1;
                    }
                    //CHIP-8
                    //Store the value of register VY shifted left one bit in register VX
                    //Set register VF to the most significant bit prior to the shift
                    else{
                        V[0xF] = (V[y] & 128) >> 7;
                        V[x] = V[y] << 1;
                    }
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
            screenUpdated = true;
        break;


        //opcodes starting with E
        case 0xE0:
            switch(low){

                //EX9E - SKP Vx
                //Skip next instruction if key Vx is pressed
                case 0x9E:
                    if(keys[V[x]]){
                        nextAddr = PC + 4;
                    }
                break;

                //EXA1 - SKNP Vx
                //Skip next instruction if key Vx is not pressed
                case 0xA1:
                    if(keys[V[x]] == false){
                        nextAddr = PC + 4;
                    }
                break;

                default:
                    reportCode(high, low);
                break;
            }
        break; //end opcodes starting with E

        //opcodes starting with F
        case 0xF0:
            switch(low){

                //FX07 - LD Vx, DT
                //Set Vx = delay timer
                case 0x07:
                    V[x] = delayTimer.ticks;
                break;

                //FX0A - LD Vx, K
                //Wait for keypress, store keypress in Vx
                case 0x0A:
                    if(waitingForKey == false){
                        waitingForKey = true;
                        nextAddr = PC;
                    }
                    else if(k.has_value()){
                        V[x] = k.value();
                        waitingForKey = false;
                        k.reset();
                        nextAddr = PC + 2;
                    }
                    else{
                        nextAddr = PC;
                    }
                break;

                //FX15 - LD DT, Vx
                //Set delay timer to Vx
                case 0x15:
                    delayTimer.ticks = V[x];
                    delayTimer.lastModified = Clock::now();
                break;

                //FX18 - LD ST, Vx
                //Set sound timer to Vx
                case 0x18:
                    soundTimer.ticks = V[x];
                    soundTimer.lastModified = Clock::now();
                break;

                //FX1E - ADD I, Vx
                //Set I = I + Vx
                case 0x1E:
                    I += V[x];
                break;

                //FX29 - LD F, Vx
                //Set I = location of sprite for digit Vx
                case 0x29:
                    //Each sprite is 5 bytes long.
                    //They are stored in crescent order,
                    //starting at addr 0.
                    I = V[x] * 5;
                break;

                //FX33 - LD B, Vx
                //Store BCD representation in memory locations I, I+1 and I+2
                //Hundreds digit at I, tens at I+1, ones at I+2
                case 0x33:
                    mem[I] = V[x] / 100;
                    mem[I+1] = (V[x] / 10) % 10;
                    mem[I+2] = V[x] % 10;
                break;

                //FX55 - LD [I], Vx
                //Store registers V0 through Vx,
                //starting at location I.
                case 0x55:
                    for(int i = 0; i <= x; i++)
                        mem[I+i] = V[i];
                break;

                //FX65 - LD Vx, [I]
                //Read registers V0 through Vx from memory,
                //starting at location I
                case 0x65:
                    for(int i = 0; i <= x; i++)
                        V[i] = mem[I+i];
                break;
            }
        break;
        

    } //End first nibble switch

    //Go to next address
    PC = nextAddr;
}


//Helper method for XNNN instructions
std::uint16_t Chip8::getNNN(std::uint8_t high, std::uint8_t low){
    std::uint16_t nnn = high & 0x0F;
    nnn <<= 8;
    nnn += low;
    return nnn;
}


//Report an unknown opcode
void Chip8::reportCode(std::uint8_t high, std::uint8_t low){
    std::cerr << "Unknown Opcode: " << std::hex << high << low << std::dec << std::endl;
}


//This method will be called by handleInput
void Chip8::pressKey(std::uint8_t key){
    if(key > 0xF){
        std::cerr << "Key does not exist! " << key << std::endl;
    }
    else{
        keys[key] = true;

        if(waitingForKey){
            k = key;
        }
    }
}

//This method will be called by handleInput
void Chip8::releaseKey(std::uint8_t key){
    if(key > 0xF){
        std::cerr << "Key does not exist! " << key << std::endl;
    }
    else{
        keys[key] = false;
    }
}

void Chip8::togglePause(){
    pause = !pause;
}

void Chip8::stop(){
    running = false;
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
            //If coordinate is outside of the screen, wrap around
            bool& screenPixel = screen[(((y + spriteY) * DISPLAY_WIDTH) + (x + spriteX)) % screen.max_size()]; 

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

//Helper method for constructor
void Chip8::loadRom(std::ifstream& rom){
    std::uint16_t i = 0x200;
    std::uint8_t buf;

    for(buf = rom.get(); rom.good(); i++){
        //If we're going over memory, throw
        if(i >= mem.max_size()){
            throw FileTooBig{};
        }
        //Else write byte to memory and read new byte
        else{
            mem[i] = buf;
            buf = rom.get();
        }
    }
}


//Decrement a timer if enough time has passed
//Returns the amount of times it was decremented
int Chip8::decrementTimer(timer& timer){
    int times = 0; //Times we decrement the timer

    //If timer is non-zero
    if(timer.ticks > 0){
        std::chrono::time_point now = Clock::now();
        
        //Dec timer for every decWait that has passed since last modification
        while(timer.ticks && now - timer.lastModified >= decWait * (times + 1)){
            timer.ticks--;
            times++;
        }

        //If we have decremented, change lastModified
        if(times) timer.lastModified = now;
    }

    return times;
}
