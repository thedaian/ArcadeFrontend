#include "core.hpp"

core::core()
{
    this->cc = sf::Color(0, 0, 0, 0);
}

core::~core()
{
}

void core::init()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    this->app.create(sf::VideoMode(desktop.width, desktop.height), "Arcade Cabinet", sf::Style::Fullscreen);
}

sf::RenderWindow * core::getApp()
{
    return &this->app;
}

void core::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::Closed)
    {
        this->app.close();
    }

    if(event.type == sf::Event::KeyReleased)
    {
        switch(event.key.code)
        {
        case sf::Keyboard::F4:
            if( (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) )
            {
                this->app.close();
            }
            break;
        }
    }
}

void core::update()
{
}

void core::draw()
{
}

bool core::isOpen()
{
    return this->app.isOpen();
}

void core::clearScreen()
{
    this->app.clear(this->cc);
}

void core::display()
{
    this->app.display();
    if(this->sleepClock.getElapsedTime().asMilliseconds() < 15)
    {
        sf::sleep(sf::milliseconds(15 - this->sleepClock.getElapsedTime().asMilliseconds()));
    }
    this->sleepClock.restart();
}
