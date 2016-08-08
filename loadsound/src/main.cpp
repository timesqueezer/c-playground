#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "CoordinateSystem.hpp"
#include "WavView.hpp"
#include "FFTView.hpp"
#include "Controls.hpp"
#include "ControlElement.hpp"

sf::Uint16 RES_X = 2050;
sf::Uint16 RES_Y = 1080;

//sf::Uint16 RES_X = 2560;
//sf::Uint16 RES_Y = 1440;

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("Argument pointing to audio file required.\n");
        return -1;
    }

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(argv[1])) {
        printf("Error loading file.\n");
    }

    //char* fontPath = "/usr/share/fonts/TTF/Roboto-Regular.ttf";
    std::string fontPath = "/usr/share/fonts/TTF/InputSansCondensed-Regular.ttf";

    int fft_size = argc >= 3 ? atoi(argv[2]) : 128;
    //GRAPHMODE graph_mode = GRAPHMODE_BARS;
    GRAPHMODE graph_mode = GRAPHMODE_INTENSITY;
    //FFTMODE fft_mode = FFTMODE_DFT;
    FFTMODE fft_mode = FFTMODE_FASTFFT;

    FFTView fft(fft_size, &buffer, RES_X - 50, RES_Y - 200, fft_mode, graph_mode);
    //WAVView wav(buffer, RES_X - 50, RES_Y - 50);
    CoordinateSystem cSystem(buffer.getSampleRate() / 2, 1.0, RES_X, RES_Y - 150, fontPath);
    Controls controls(RES_X, 150, &fft);
    sf::String s("FFT Size");
    ControlElement fftSizeSwitchButton(200, 50, 50, RES_Y - 116, TYPE_BUTTON, s);
    controls.addElement(&fftSizeSwitchButton);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;

    sf::RenderWindow window(sf::VideoMode(RES_X, RES_Y), "HI", sf::Style::Default, settings);
    sf::RenderTexture fftTexture;
    if (!fftTexture.create(RES_X - 50, RES_Y - 150)) {
        printf("Failed to create fft-texture.\n");
        return -1;
    }

    sf::RenderTexture controlsTexture;
    if (!controlsTexture.create(RES_X, 150)) {
        printf("Failed to create controls-texture.\n");
        return -1;
    }

    sf::Font font;
    if (!font.loadFromFile(fontPath)) {
        printf("Please install font.\n");
        return -1;
    }


    // FPS STRING
    sf::Text fps_string;
    fps_string.setFont(font);
    fps_string.setCharacterSize(22);
    fps_string.setColor(sf::Color::White);
    fps_string.setPosition(250, RES_Y - 116);

    // FFT-SIZE STRING
    sf::Text fft_string;
    fps_string.setFont(font);
    fps_string.setCharacterSize(22);
    fps_string.setColor(sf::Color::White);
    fps_string.setPosition(250, RES_Y - 66);
    fps_string.setString(std::to_string(fft.getFFTSize()));

    fft.calc();
    fft.render();
    fftTexture.clear();

    fftTexture.draw(fft);
    //fftTexture.draw(wav);
    fftTexture.display();


    cSystem.render();

    sf::Clock clock;
    clock.restart();

    bool paused = false;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                    break;

                case sf::Event::Resized:
                    printf("new dimensions: %ix%i\n", event.size.width, event.size.height);
                    cSystem.setDimensions(event.size.width, event.size.height);
                    fft.setDimensions(event.size.width-50, event.size.height-50);

                case sf::Event::LostFocus:
                    paused = true;

                case sf::Event::GainedFocus:
                    paused = false;

                default:
                    break;
            }

            controls.handleEvent(event);
        }

        if (paused) {
            sf::sleep(sf::seconds(0.1f));
            continue;
        }

        sf::Time elapsed = clock.restart();
        //sf::String mseconds(elapsed.asMilliseconds());
        if (elapsed.asMilliseconds() > 0) {
            fps_string.setString(std::to_string(1000/elapsed.asMilliseconds()));
        } else {
            fps_string.setString("0");
        }

        window.clear();

        sf::Sprite inner(fftTexture.getTexture());
        inner.setPosition(sf::Vector2f(50, 0));
        window.draw(inner);

        window.draw(cSystem);
        window.draw(controls);
        window.draw(fps_string);
        window.draw(fft_string);
        window.display();
    }


    return 0;
}
