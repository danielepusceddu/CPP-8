#pragma once
#include <SFML/Graphics.hpp>
#include "Chip8.hpp"

class Chip8_SFML : public Chip8{
    public:
        Chip8_SFML(std::string romFilename);
        void handleInput() override;
        void display() override; 

    private:
    //DATA
        sf::RenderWindow window;

    //METHODS
        //Helper method used in handleInput
        void handleKeyEvent(sf::Event e);
};