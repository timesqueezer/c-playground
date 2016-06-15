#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <string>

#define PI           3.14159265358979323846  /* pi */

sf::Uint16 RES_X = 1920;
sf::Uint16 RES_Y = 1080;

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("Argument pointing to audio file required.\n");
        return -1;
    }

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(argv[1])) {
        printf("Error loading file.\n"  );
        return -1;
    }

    int N = buffer.getSampleCount();
    int sample_rate = buffer.getSampleRate();
    int channel_count = buffer.getChannelCount();
    float length = (float)N/(float)sample_rate;

    printf("Got %i samples @%i / %i\n", N, sample_rate, channel_count);
    printf("Obviously the length is %f/%i = %f\n", length, channel_count, length/channel_count);

    const short int* samples = buffer.getSamples();
    /*for (int i = 0; i < N; ++i) {
        printf("Sample#%i: %i\n", i, samples[i]);
    }*/

    sf::RenderWindow window(sf::VideoMode(RES_X, RES_Y), "HI");
    //sf::VertexArray rofl(sf::LinesStrip, N);
    std::vector<sf::Vertex> rofl;
    /*sf::Texture texture;
    if (!texture.create(RES_X, RES_Y)) {
        std::cout << "Error\n";
    }
    sf::Uint8 targetColors[RES_X * RES_Y * 4];*/

    /*double double_width = (double)(RES_X * 2);
    printf("Double width: %f\n", double_width);
    double lol = N / double_width;
    printf("Samples per pixel: %f\n", lol);

    long window_size = (long)floor(lol);
    printf("Window Size: %i\n", window_size);
    printf("You lost %i samples\n", N % window_size);

    int half_16 = (1 << 16) / 2;
    int full_16 = 1 << 16;
    printf("2^16 / 2 = %i\n", half_16);

    for (int i = 0; i < RES_X; ++i) {
        // Average all samples for this pixel
        long start = i*window_size;
        long end = start+window_size;
        //printf("Window from %i to %i", start, end);
        //bool column[RES_Y] = {false};
        for (int j = start; j < end; ++j) {
            int x = (int)round((double)(samples[j*2] + half_16) / full_16 * RES_Y);
            rofl.push_back(sf::Vertex(sf::Vector2f(i, x), sf::Color::White));
        }
    }*/

        /*average = (int)round((double)average / window_size);
        if (i == 50) printf("Average : %i\n", average);
        average += half_16;
        if (i == 50) printf("uAverage: %i\n", average);
        int average_in_px = (int)round((double)average / full_16 * RES_Y);
        if (i == 50) printf("average in px: %i\n", average_in_px);*/
        /*

        //int average_in_px = 250;

        // Fill one line
        for (int y = 0; y < RES_Y; ++y) {
            if (column[y]) {
                targetColors[(i * 4) + (RES_X * 4 * y)] = 200;
                targetColors[(i * 4) + 1 + (RES_X * 4 * y)] = 225;
                targetColors[(i * 4) + 2 + (RES_X * 4 * y)] = 255;
                targetColors[(i * 4) + 3 + (RES_X * 4 * y)] = 255;
            } else {
                targetColors[(i * 4) + (RES_X * 4 * y)] = 20;
                targetColors[(i * 4) + 1 + (RES_X * 4 * y)] = 20;
                targetColors[(i * 4) + 2 + (RES_X * 4 * y)] = 20;
                targetColors[(i * 4) + 3 + (RES_X * 4 * y)] = 255;
            }
        }
    }

    texture.update(targetColors);

    sf::Sprite sprite;
    sprite.setTexture(texture);*/

    int max_freq = sample_rate / 2;
    double mag[max_freq];
    //double phase[max_freq];

    sf::Clock clock;
    clock.restart();

    int fft_size = 1024;
    for (int k = 0; k < max_freq; ++k) {
        double real = 0;
        double imag = 0;
        for (int n = 0; n < fft_size; ++n) {
            real += (samples[n*2] * cos(2*PI*k*n/N));
            imag += (samples[n*2] * sin(2*PI*k*n/N));
        }
        mag[k] = sqrt((real*real)+(imag*imag)) / N;
        //phase[k] =
    }

    printf("FFT done after: %ims\n", clock.restart().asMilliseconds());
    //for (int i=0;i<150;++i) { printf("%f\n", mag[i]); }

    int half_16 = (1 << 16) / 2;
    int full_16 = 1 << 16;

    // Generating bar graph
    int num_bars = 100;
    int padding = 5;
    int bar_width = (int)floor((double)(RES_X - (padding*num_bars) ) / num_bars);
    sf::RectangleShape bars[num_bars];
    int k_width = (int)floor((double)max_freq / num_bars);
    for (int i = 0; i < num_bars; ++i) {
        double average = 0;
        for (int k = 0; k < k_width; ++k) {
            average += mag[k+(i*k_width)];
        }
        average = average / k_width;
        printf("Average%i: %f\n", i, average);
        //int height = (int)round((double)(average + half_16) / full_16 * RES_Y);
        int height = (int)round(average*RES_X/4);
        bars[i] = sf::RectangleShape(sf::Vector2f(bar_width, height));
        bars[i].setPosition(sf::Vector2f(i*(bar_width+padding), 0));
        bars[i].setFillColor(sf::Color(200, 220, i*255/num_bars, 255));
    }

    sf::Font font;
    if (!font.loadFromFile("roboto.ttf")) {
        return -1;
    }

    sf::Text fps_string;
    fps_string.setFont(font);
    fps_string.setCharacterSize(32);
    fps_string.setColor(sf::Color::Blue);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Time elapsed = clock.restart();
        //sf::String mseconds(elapsed.asMilliseconds());
        if (elapsed.asMilliseconds() > 0) {
            fps_string.setString(std::to_string(1000/elapsed.asMilliseconds()));
        } else {
            fps_string.setString("0");
        }

        window.clear();
        for (int i = 0; i < num_bars; ++i) {
            window.draw(bars[i]);
        }
        //window.draw(sprite);
        //window.draw(&rofl[0], rofl.size(), sf::LinesStrip);
        window.draw(fps_string);
        window.display();
    }


    return 0;
}
