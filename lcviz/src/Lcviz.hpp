#ifndef LCVIZ_HPP
#define LCVIZ_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>

//#include "CoordinateSystem.hpp"

class Lcviz {
public:
    Lcviz(int, char**);
    void loop();

private:
    sf::Font mFont;

    sf::Text mFpsString;
    //CoordinateSystem mCSystem;

    sf::RenderWindow mWindow;
    sf::RenderTexture mFFTTexture;

    sf::Clock mClock;
    bool mPaused;

};

#endif
