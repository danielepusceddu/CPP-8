#include "Chip8_SFML.hpp"

Chip8_SFML::Chip8_SFML(std::string romFilename)
: Chip8{romFilename}
{
    //1 bit per pixel. Chip8 is monochrome.
    //(hopefully this works)
    window.create(sf::VideoMode{640, 320, 1}, "Chip8");
}


void Chip8_SFML::handleInput(){
    sf::Event buf;

    while(window.pollEvent(buf)){
        if(buf.type == sf::Event::KeyPressed || buf.type == sf::Event::KeyReleased){
            handleKeyEvent(buf);
        }
        else if(buf.type == sf::Event::Closed){
            running = false;
        }
    }
}


void Chip8_SFML::display(){

    if(screenUpdated){
        //draw on our sfml window...
        screenUpdated = false;
    }
}