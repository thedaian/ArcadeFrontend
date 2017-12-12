#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#ifndef CORE_HPP
#define CORE_HPP

/**
    This is the base class for handling the main game loop.
    It holds the window and that's mostly about it.
    In this case it's not that useful, however it comes from a program that had multiple possible functions in one,
    and I initially thought the arcade software would run on a similar system (with a separate class for the game list and game info screens)
*/

class core {
private:
    sf::Clock sleepClock;
protected:
    sf::RenderWindow app;
    sf::Color cc;
public:
    core();
    ~core();

    void init();

    sf::RenderWindow * getApp();
    virtual void handleEvent(sf::Event);
    virtual void update();
    virtual void draw();

    bool isOpen();
    void clearScreen();
    void display();
};

#endif // CORE_HPP
