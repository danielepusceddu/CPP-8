#include "Chip8_SFML.hpp"
#include <iostream>

int main(int argc, const char** argv){
    if(argc < 2){
        std::cout << "Usage: " << argv[0] << " <chip8_rom> [chip48]" << std::endl;
    }
    else{
        Chip8_SFML chip8{argv[1]};
        bool chip48 = false;

        if(argc >= 3 && std::string{argv[2]} == "chip48"){
            chip48 = true;
        }

        chip8.setChip48(chip48);
        chip8.run();
    }

    return 0;
}