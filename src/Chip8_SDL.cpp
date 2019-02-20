#include "Chip8_SDL.hpp"
#include <iostream>

Chip8_SDL::Chip8_SDL(std::string romFilename, int scale)
: Chip8{romFilename, scale}
{
    //If SDL_Init error
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
    }
    //Else init window and check if it is null
    else if(window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH*scale, DISPLAY_HEIGHT*scale, SDL_WINDOW_SHOWN);
            window == NULL){
            std::cerr << "SDL Window Error: " << SDL_GetError() << "\n";
    }
    //If no error, init rect and renderer
    else{
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Black screen
        SDL_RenderClear(renderer);
        rect.h = scale; rect.w = scale;
    }

}

Chip8_SDL::~Chip8_SDL(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
void Chip8_SDL::handleInput(){

}

void Chip8_SDL::draw(const std::array<bool, DISPLAY_WIDTH*DISPLAY_HEIGHT>& screen){
    int scale = getScale();
    //window.clear();

    //Use black to clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //Use white for the pixels
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //White, no transparency

    //For each coordinate
    for(int y = 0; y < Chip8::DISPLAY_HEIGHT; y++){
        for(int x = 0; x < Chip8::DISPLAY_WIDTH; x++){

            //If pixel is turned on, draw it
            if(screen[(y * Chip8::DISPLAY_WIDTH) + x]){
                //rect.setPosition(x * scale, y * scale);
                rect.x = x * scale; rect.y = y * scale;
                SDL_RenderFillRect(renderer, &rect);
                //window.draw(rect);
            }

        }
    }

    //Update screen
    SDL_RenderPresent(renderer);
    //window.display();
}

void Chip8_SDL::playSound(){

}