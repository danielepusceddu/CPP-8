#include "Chip8_SDL.hpp"
#include <iostream>

Chip8_SDL::Chip8_SDL(std::string romFilename, int scale)
: Chip8{romFilename, scale}
{
    //If SDL_Init error
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
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

        //Sound init
        if(SDL_LoadWAV("../assets/beep.wav", &beepSpec, &beepBuf, &beepLength) == NULL){
            std::cerr << "Error loading sound effect: " << SDL_GetError() << "\n"; 
        }
        else if(audioDevice = SDL_OpenAudioDevice(NULL, 0, &beepSpec, NULL, 0); audioDevice == 0){
            std::cerr << "Failed opening audio device: " << SDL_GetError() << "\n"; 
        }
        else{
            SDL_PauseAudioDevice(audioDevice, 0); //Unpause audio
            audioSuccess = true;
        }

    }

}

Chip8_SDL::~Chip8_SDL(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_CloseAudioDevice(audioDevice);
    SDL_FreeWAV(beepBuf);
    SDL_Quit();
}


void Chip8_SDL::handleInput(){
    SDL_Event buf;

    while(SDL_PollEvent(&buf)){
        if(buf.type == SDL_KEYDOWN || buf.type == SDL_KEYUP){
            handleKeyEvent(buf);
        }
        else if(buf.type == SDL_QUIT){
            stop();
        }
    }
}

void Chip8_SDL::draw(const std::array<bool, DISPLAY_WIDTH*DISPLAY_HEIGHT>& screen){
    int scale = getScale();

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
                rect.x = x * scale; 
                rect.y = y * scale;
                SDL_RenderFillRect(renderer, &rect);
            }

        }
    }

    //Update screen
    SDL_RenderPresent(renderer);
    //window.display();
}

void Chip8_SDL::playSound(){
    if(audioSuccess && SDL_QueueAudio(audioDevice, beepBuf, beepLength) < 0){
        std::cerr << "Failed to play sound effect: " << SDL_GetError() << "\n";
    }
}



void Chip8_SDL::handleKeyEvent(SDL_Event e){
    /*
    Chip8 Key   Keyboard
    ---------   ---------
     1 2 3 C     1 2 3 4
     4 5 6 D     q w e r
     7 8 9 E     a s d f
     A 0 B F     z x c v
     */

    switch(e.type){
        //Key Pressed
        case SDL_KEYDOWN:
            switch(e.key.keysym.sym){
                //ESC - Quit
                case SDLK_ESCAPE:
                    stop();
                break;

                //Pause
                case SDLK_PAUSE:
                case SDLK_F1:
                    togglePause();
                break;

                case SDLK_1:
                    pressKey(1);
                break;

                case SDLK_2:
                    pressKey(2);
                break;

                case SDLK_3:
                    pressKey(3);
                break;

                case SDLK_4:
                    pressKey(0xC);
                break;

                case SDLK_q:
                    pressKey(4);
                break;

                case SDLK_w:
                    pressKey(5);
                break;

                case SDLK_e:
                    pressKey(6);
                break;

                case SDLK_r:
                    pressKey(0xD);
                break;

                case SDLK_a:
                    pressKey(7);
                break;

                case SDLK_s:
                    pressKey(8);
                break;

                case SDLK_d:
                    pressKey(9);
                break;

                case SDLK_f:
                    pressKey(0xE);
                break;

                case SDLK_z:
                    pressKey(0xA);
                break;

                case SDLK_x:
                    pressKey(0);
                break;

                case SDLK_c:
                    pressKey(0xB);
                break;

                case SDLK_v:
                    pressKey(0xF);
                break;

                default:
                break;
            }
        break;

        case SDL_KEYUP:
            switch(e.key.keysym.sym){
                case SDLK_1:
                    releaseKey(1);
                break;

                case SDLK_2:
                    releaseKey(2);
                break;

                case SDLK_3:
                    releaseKey(3);
                break;

                case SDLK_4:
                    releaseKey(0xC);
                break;

                case SDLK_q:
                    releaseKey(4);
                break;

                case SDLK_w:
                    releaseKey(5);
                break;

                case SDLK_e:
                    releaseKey(6);
                break;

                case SDLK_r:
                    releaseKey(0xD);
                break;

                case SDLK_a:
                    releaseKey(7);
                break;

                case SDLK_s:
                    releaseKey(8);
                break;

                case SDLK_d:
                    releaseKey(9);
                break;

                case SDLK_f:
                    releaseKey(0xE);
                break;

                case SDLK_z:
                    releaseKey(0xA);
                break;

                case SDLK_x:
                    releaseKey(0);
                break;

                case SDLK_c:
                    releaseKey(0xB);
                break;

                case SDLK_v:
                    releaseKey(0xF);
                break;

                default:
                break;
            }
        break; 

        default:
        break;
    }
}
