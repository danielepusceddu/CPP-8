#include "Chip8_SFML.hpp"
#include <iostream>
#include <string>

void parseOptions(int argc, char** argv, bool& chip48, int& resolutionScale);

int main(int argc, char** argv){
    //If no rom path provided
    if(argc < 2){
        std::cout << "Usage: " << argv[0] << " <chip8_rom> [chip48]" << std::endl;
    }
    else{
        //Default chip8 options
        bool chip48 = false;
        int scale = 10;

        //Read options from command line and initialize chip8
        parseOptions(argc, argv, chip48, scale);
        Chip8_SFML chip8{argv[1], scale};
        chip8.setChip48(chip48);

        //Run the interpreter
        chip8.run();
    }

    return 0;
}

void parseOptions(int argc, char** argv, bool& chip48, int& resolutionScale){
    //argv[1] is the rom filename
    for(int i = 2; i < argc; i++){
        std::string param{argv[i]};

        if(param == "chip48"){
            chip48 = true;
        }
        else if(param == "-s" && i < argc - 1){
            i++;
            resolutionScale = std::atoi(argv[i]);
        }
    }
}