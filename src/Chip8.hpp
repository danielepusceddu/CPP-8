#pragma once
#include <chrono>
#include <optional>
#include <array>
#include <stack>
#include <memory>
#include <random>
#include <cstdint>
#include <string>

class Chip8{
    public:
        class FileNotFound : public std::exception{};
        class FileTooBig : public std::exception{};

        //This method attempts copying the contents of
        //a file to chip8's RAM, starting at addr 0x200
        //The file might be too big to fit in the RAM,
        //or we might be unable to open it.
        //In that case, this method will throw the appropriate exception.
        //outputScale sets the output resolution to DISPLAY_WIDTH*scale by DISPLAY_HEIGHT*scale
        //It cannot be less than 1 and the default is 10.
        Chip8(std::string romFilename, int outputScale);

        //This method runs until user input stops the execution
        void run();

        //Set chip48 mode
        void setChip48(bool b);

        //Get resolution scaling
        int getScale();

        //Virtual destructor
        virtual ~Chip8() = default;

    protected:
    //CONSTANTS
        static constexpr int DISPLAY_WIDTH = 64;
        static constexpr int DISPLAY_HEIGHT = 32;
    
    //METHODS
        //Input and output is up to subclasses to implement
        virtual void playSound() = 0;
        virtual void handleInput() = 0;
        virtual void draw(const std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT>& screen) = 0;

        //These methods will be called by handleInput
        void pressKey(std::uint8_t key);
        void releaseKey(std::uint8_t key);
        void togglePause(); //Pauses / unpauses execution
        void stop();    //Stops execution

    private:
    //VARIABLES
        //This is so we don't waste time redrawing the same thing
        bool screenUpdated = false;

        //Pause status
        bool pause = false;

        //Running status
        bool running = true;

        //Output resolution will be DISPLAY_WIDTH*scale by DISPLAY_HEIGHT*scale
        int scale = 10;

        //Hertz and ms between each cycle
        int hz = 500;
        std::chrono::milliseconds timeBetweenCycles{1000 / hz};

        //This flag is used to know if we should do
        //chip-8 or chip-48 shift instructions
        bool chip48 = false;

        //16 general purpose 8-bit registers
        //Referred to as Vx, where x is a hex digit.
        //VF is a Flag register used by some instructions
        std::uint8_t V[16];

        //Stack
        std::stack<std::uint16_t> stack;

        //4KB of RAM.
        //Chip-8 programs should start at 0x200 (512)
        std::array<std::uint8_t, 4096> mem;

        //Program Counter
        //Used to store the currently executing address
        std::uint16_t PC = 0x200;

        //This register is usually used to store addresses
        std::uint16_t I = 0;

        //Shorthands for chrono utilities
        using Clock = std::chrono::high_resolution_clock;
        using ms = std::chrono::milliseconds;
        using time = std::chrono::time_point<Clock, Clock::duration>;

        //Delay and Sound timers
        //When they are non-zero, they are decremented at a rate of 60hz
        static constexpr ms decWait{17}; //Roughly 60hz
        struct timer{
            Clock::time_point lastModified;
            std::uint8_t ticks = 0;
        };
        timer delayTimer;
        timer soundTimer;
        //std::uint8_t delayTimer = 0;
        //std::uint8_t soundtimer = 0;

        //Utilities for random number generation
        std::default_random_engine randEng;
        std::uniform_int_distribution<std::uint8_t> intDist{0, 255};

        //Helper variables for "wait for keypress" opcode
        //When waiting, k will be filled with the next keypress
        //Then waiting status will be reset when the instruction is executed again
        std::optional<std::uint8_t> k;
        bool waitingForKey = false;

        //Input and Output
        std::array<bool, DISPLAY_WIDTH*DISPLAY_HEIGHT> screen;
        std::array<bool, 16> keys;

    
    //STRUCTS
    ms msBuf;
    time lastCycle;

    //METHODS
        //Function called in main loop
        void mainLoopFunc();
        static void mainLoopFunc_emscripten(void* params);

        //Execute the instruction pointed by the program counter
        void step();

        //Helper method for draw instruction
        //Returns true if collifion happened
        bool drawSprite(int x, int y, std::uint16_t addr, std::size_t len);

        //Helper method for XNNN instructions
        std::uint16_t getNNN(std::uint8_t high, std::uint8_t low);

        //Report an unknown opcode
        void reportCode(std::uint8_t high, std::uint8_t low);

        //Helper method for constructor
        void loadRom(std::ifstream& rom);

        //Decrement a timer if enough time has passed
        //Returns the amount of times it was decremented
        int decrementTimer(timer& timer);

    //CONSTANTS
        //This is a group of sprites representing the hex digits
        //They will be stored starting from RAM 0x000 
        //Each sprite is 5 bytes long
        //This array is not pretty but I don't see
        //better ways to do this.
        static constexpr std::array<std::uint8_t, 16*5> hexSprites =
            {
                //0
                0b1111'0000,
                0b1001'0000,
                0b1001'0000,
                0b1001'0000,
                0b1111'0000,

                //1
                0b0010'0000,
                0b0110'0000,
                0b0010'0000,
                0b0010'0000,
                0b0111'0000,

                //2
                0b1111'0000,
                0b0001'0000,
                0b1111'0000,
                0b1000'0000,
                0b1111'0000,

                //3
                0b1111'0000,
                0b0001'0000,
                0b1111'0000,
                0b0001'0000,
                0b1111'0000,

                //4
                0b1001'0000,
                0b1001'0000,
                0b1111'0000,
                0b0001'0000,
                0b0001'0000,

                //5
                0b1111'0000,
                0b1000'0000,
                0b1111'0000,
                0b0001'0000,
                0b1111'0000,

                //6
                0b1111'0000,
                0b1000'0000,
                0b1111'0000,
                0b1001'0000,
                0b1111'0000,

                //7
                0b1111'0000,
                0b0001'0000,
                0b0010'0000,
                0b0100'0000,
                0b0100'0000,

                //8
                0b1111'0000,
                0b1001'0000,
                0b1111'0000,
                0b1001'0000,
                0b1111'0000,

                //9
                0b1111'0000,
                0b1001'0000,
                0b1111'0000,
                0b0001'0000,
                0b1111'0000,

                //A
                0b1111'0000,
                0b1001'0000,
                0b1111'0000,
                0b1001'0000,
                0b1001'0000,

                //B
                0b1110'0000,
                0b1001'0000,
                0b1110'0000,
                0b1001'0000,
                0b1110'0000,

                //C
                0b1111'0000,
                0b1000'0000,
                0b1000'0000,
                0b1000'0000,
                0b1111'0000,

                //D
                0b1110'0000,
                0b1001'0000,
                0b1001'0000,
                0b1001'0000,
                0b1110'0000,

                //E
                0b1111'0000,
                0b1000'0000,
                0b1111'0000,
                0b1000'0000,
                0b1111'0000,

                //F
                0b1111'0000,
                0b1000'0000,
                0b1111'0000,
                0b1000'0000,
                0b1000'0000,
             };
};
