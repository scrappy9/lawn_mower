//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>


#include <iostream>
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>

#include "Lawn.h"
#include "strategies.h"



int main() {

#ifdef _DEBUG
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF;  // check heap alloc and dump mem leaks at exit
    _CrtSetDbgFlag(tmpFlag);
    _crtBreakAlloc = 101;
#endif


    {
        sf::RenderWindow window(sf::VideoMode(512, 512), "LawnMower!");

        Lawn lawn;
        //Mower mower{std::make_unique<RandomWalkStrategy>()};
        Mower mower{std::make_unique<DepthFirstSearchStrategy>()};
        lawn.disableTilesRandomly(50);
        lawn.place(mower);

        while (window.isOpen()) {

            sf::Event event;
            sf::Vector2i mouse_down, mouse_up;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::KeyPressed
                    && event.key.code == sf::Keyboard::Space) {
                    mower.step();
                }
            }

            window.clear();
            window.draw(lawn);
            window.draw(mower);
            window.display();
        }

    }

    return 0;
}