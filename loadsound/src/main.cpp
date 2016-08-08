#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "CoordinateSystem.hpp"
#include "WavView.hpp"
#include "FFTView.hpp"

sf::Uint16 RES_X = 1280;
sf::Uint16 RES_Y = 720;

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
    GRAPHMODE graph_mode = GRAPHMODE_BARS;
    //GRAPHMODE graph_mode = GRAPHMODE_INTENSITY;
    FFTMODE fft_mode = FFTMODE_DFT;

    FFTView fft(fft_size, &buffer, RES_X - 50, RES_Y - 50, fft_mode, graph_mode);
    //WAVView wav(buffer, RES_X - 50, RES_Y - 50);
    CoordinateSystem cSystem(buffer.getSampleRate() / 2, 1.0, RES_X, RES_Y, fontPath);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;

    sf::RenderWindow window(sf::VideoMode(RES_X, RES_Y), "HI", sf::Style::Default, settings);
    sf::RenderTexture texture;
    if (!texture.create(RES_X - 50, RES_Y - 50)) {
        return -1;
    }

    sf::Font font;
    if (!font.loadFromFile(fontPath)) {
        printf("Please install Roboto-Regular.\n");
        return -1;
    }

    sf::Text fps_string;
    fps_string.setFont(font);
    fps_string.setCharacterSize(24);
    fps_string.setColor(sf::Color::Blue);

    fft.calc();
    fft.render();

    cSystem.render();

    sf::Clock clock;
    clock.restart();

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
                    fft.setDimensions(event.size.width, event.size.height);

                default:
                    break;
            }
        }

        sf::Time elapsed = clock.restart();
        //sf::String mseconds(elapsed.asMilliseconds());
        if (elapsed.asMilliseconds() > 0) {
            fps_string.setString(std::to_string(1000/elapsed.asMilliseconds()));
        } else {
            fps_string.setString("0");
        }

        window.clear();
        texture.clear();

        texture.draw(fft);
        //texture.draw(wav);
        texture.display();

        sf::Sprite inner(texture.getTexture());
        inner.setPosition(sf::Vector2f(50, 0));
        window.draw(inner);

        window.draw(cSystem);
        window.draw(fps_string);
        window.display();
    }


    return 0;
}
