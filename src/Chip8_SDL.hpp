#pragma once
#include <SDL2/SDL.h>
#include "Chip8.hpp"

class Chip8_SDL : public Chip8{
    public:
        Chip8_SDL(std::string romFilename, int scale);
        ~Chip8_SDL();

    private:
    //DATA
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Rect rect;

        //For audio
        bool audioSuccess = false;
        SDL_AudioDeviceID audioDevice;
        SDL_AudioSpec beepSpec;
        std::uint32_t beepLength;
        std::uint8_t* beepBuf;


    //METHODS
        //Helper method used in handleInput
        void handleKeyEvent(SDL_Event e);

        //Overridden I/O methods
        void handleInput() override;
        void playSound() override;
        void draw(const std::array<bool, DISPLAY_WIDTH*DISPLAY_HEIGHT>& screen) override; 
};