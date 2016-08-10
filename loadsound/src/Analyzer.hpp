#ifndef ANALYZER_HPP
#define ANALYZER_HPP

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "CoordinateSystem.hpp"
#include "WavView.hpp"
#include "FFTView.hpp"

class Analyzer {
public:
    Analyzer(int, char**);
    void FFTSizeToggleCallback();
    void Run();
    void update();
    void loop();

private:
    sf::SoundBuffer mBuffer;
    sf::Font mFont;

    sf::Text mFpsString;
    sf::Text mFFTString;

    FFTView mFFTView;
    CoordinateSystem mCSystem;

    sf::RenderWindow mWindow;
    sf::RenderTexture mFFTTexture;

    sfg::SFGUI mSfgui;
    sfg::Window::Ptr mGuiWindow;
    sfg::Button::Ptr mFFTButton;

    sf::Clock mClock;
    bool mPaused;

};

#endif
