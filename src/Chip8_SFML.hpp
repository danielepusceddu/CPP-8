#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Chip8.hpp"

class Chip8_SFML : public Chip8{
    public:
        Chip8_SFML(std::string romFilename, int scale);

    private:
    //DATA
        sf::RenderWindow window;
        sf::RectangleShape rect;
        sf::SoundBuffer beepBuffer;
        sf::Sound beep;

    //METHODS
        //Helper method used in handleInput
        void handleKeyEvent(sf::Event e);

        //Overridden I/O methods
        void handleInput() override;
        void playSound() override;
        void draw(const std::array<bool, DISPLAY_WIDTH*DISPLAY_HEIGHT>& screen) override; 
};