#include "Chip8_SFML.hpp"

Chip8_SFML::Chip8_SFML(std::string romFilename)
: Chip8{romFilename}
{
    //1 bit per pixel. Chip8 is monochrome.
    //(hopefully this works)
    window.create(sf::VideoMode{640, 320, 1}, "Chip8");

    //Initialize rectangle
    rect.setFillColor(sf::Color{sf::Color::White});
    rect.setSize(sf::Vector2f{10, 10});
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

    //If the screen has been updated since last draw
    if(screenUpdated){
        window.clear();
        auto screen = get_screen();

        //draw on our sfml window...
        //For each coordinate
        for(int y = 0; y < 32; y++){
            for(int x = 0; x < 64; x++){
                //If pixel is turned on
                if(screen[(y * 32) + x]){
                    //Draw it
                    rect.setPosition(x*10, y*10);
                    window.draw(rect);
                }
            }
        }

        window.display();
        screenUpdated = false;
    }
}


void Chip8_SFML::handleKeyEvent(sf::Event e){
}