#include "Chip8_SFML.hpp"

Chip8_SFML::Chip8_SFML(std::string romFilename)
: Chip8{romFilename}
{
    sf::VideoMode desktop{sf::VideoMode::getDesktopMode()};
    sf::Vector2i center{static_cast<int>(desktop.width) / 2 - 640 / 2, static_cast<int>(desktop.height) / 2 - 320 / 2};

    //1 bit per pixel. Chip8 is monochrome.
    //(hopefully this works)
    window.create(sf::VideoMode{640, 320, 1}, "Chip8");

    //Center the window
    window.setPosition(center);

    //Initialize rectangle
    rect.setFillColor(sf::Color{sf::Color::White});
    rect.setSize(sf::Vector2f{10, 10});

    //Load sounds
    beepBuffer.loadFromFile("../assets/beep.ogg");
    beep.setBuffer(beepBuffer);
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

void Chip8_SFML::playSound(){
    beep.play();
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
                if(screen[(y * 64) + x]){
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
        case sf::Event::KeyPressed:
            switch(e.key.code){
                //ESC
                case sf::Keyboard::Escape:
                    running = false;
                break;

                case sf::Keyboard::Num1:
                    pressKey(1);
                break;

                case sf::Keyboard::Num2:
                    pressKey(2);
                break;

                case sf::Keyboard::Num3:
                    pressKey(3);
                break;

                case sf::Keyboard::Num4:
                    pressKey(0xC);
                break;

                case sf::Keyboard::Q:
                    pressKey(4);
                break;

                case sf::Keyboard::W:
                    pressKey(5);
                break;

                case sf::Keyboard::E:
                    pressKey(6);
                break;

                case sf::Keyboard::R:
                    pressKey(0xD);
                break;

                case sf::Keyboard::A:
                    pressKey(7);
                break;

                case sf::Keyboard::S:
                    pressKey(8);
                break;

                case sf::Keyboard::D:
                    pressKey(9);
                break;

                case sf::Keyboard::F:
                    pressKey(0xE);
                break;

                case sf::Keyboard::Z:
                    pressKey(0xA);
                break;

                case sf::Keyboard::X:
                    pressKey(0);
                break;

                case sf::Keyboard::C:
                    pressKey(0xB);
                break;

                case sf::Keyboard::V:
                    pressKey(0xF);
                break;
            }
        break;

        case sf::Event::KeyReleased:
            switch(e.key.code){
                //ESC
                case sf::Keyboard::Escape:
                    running = false;
                break;

                case sf::Keyboard::Num1:
                    releaseKey(1);
                break;

                case sf::Keyboard::Num2:
                    releaseKey(2);
                break;

                case sf::Keyboard::Num3:
                    releaseKey(3);
                break;

                case sf::Keyboard::Num4:
                    releaseKey(0xC);
                break;

                case sf::Keyboard::Q:
                    releaseKey(4);
                break;

                case sf::Keyboard::W:
                    releaseKey(5);
                break;

                case sf::Keyboard::E:
                    releaseKey(6);
                break;

                case sf::Keyboard::R:
                    releaseKey(0xD);
                break;

                case sf::Keyboard::A:
                    releaseKey(7);
                break;

                case sf::Keyboard::S:
                    releaseKey(8);
                break;

                case sf::Keyboard::D:
                    releaseKey(9);
                break;

                case sf::Keyboard::F:
                    releaseKey(0xE);
                break;

                case sf::Keyboard::Z:
                    releaseKey(0xA);
                break;

                case sf::Keyboard::X:
                    releaseKey(0);
                break;

                case sf::Keyboard::C:
                    releaseKey(0xB);
                break;

                case sf::Keyboard::V:
                    releaseKey(0xF);
                break;
            }
        break; 
    }
}