#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <string>
#include <complex>

#include "lol/fft.hpp"
#include "include/CoordinateSystem.hpp"
#include "include/WavView.hpp"

#define PI           3.14159265358979323846  /* pi */

sf::Uint16 RES_X = 2050;
sf::Uint16 RES_Y = 1080;

class FFTView : public sf::Drawable {
private:
    int mFFTSize, mWidth, mHeight;
    sf::SoundBuffer* mBuffer;
    std::vector<sf::RectangleShape> mBars;
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

public:
    FFTView(int, sf::SoundBuffer*, int, int);
    void calc();
};

FFTView::FFTView(int fft_size, sf::SoundBuffer* buffer, int width, int height) {
    mFFTSize = fft_size;
    mBuffer = buffer;
    mWidth = width;
    mHeight = height;

    int N = buffer->getSampleCount();
    int sample_rate = buffer->getSampleRate();
    int channel_count = buffer->getChannelCount();
    float length = (float)N / (float)sample_rate / channel_count;

    printf("Got %i samples @%i / %i\n", N, sample_rate, channel_count);
    printf("Length is %f\n\n", length);
}

std::vector<std::complex<double>> FFT(std::vector<std::complex<double>> &samples) {
    int max_freq = 20000;
    int N =samples.size();
    if (N == 1) { return samples; }

    int M = N / 2;

    std::vector<std::complex<double>> Xeven(M, 0);
    std::vector<std::complex<double>> Xodd(M, 0);

    for (int i = 0; i < M; ++i) {
        Xeven[i] = samples[2 * i];
        Xodd[i] = samples[2 * i + 1];
    }

    if (M > max_freq) { M = max_freq; }

    std::vector<std::complex<double>> Feven(M, 0);
    Feven = FFT(Xeven);
    std::vector<std::complex<double>> Fodd(M, 0);
    Fodd = FFT(Xodd);

    printf("NUM SAMPLES: %i\n", N);

    printf("Feven: ");
    for (int i = 0; i < M; ++i) {
        printf("%.2f+i%.2f, ", Feven[i].real(), Feven[i].imag());
    }
    printf("\n");

    printf("Fodd. ");
    for (int i = 0; i < M; ++i) {
        printf("%.2f+i%.2f, ", Fodd[i].real(), Fodd[i].imag());
    }
    printf("\n");

    int F;
    if (N > max_freq) { F = max_freq; }
    else { F = N; }

    std::vector<std::complex<double>> freqbins(F, 0);

    for (int k = 0; k < M; ++k) {
        std::complex<double> cpexp = std::polar(1.0, -2*PI*k/N) * Fodd[k];
        freqbins[k] = Feven[k] + cpexp;

        //freqbins[k+N/2] = Feven[k] - cpexp;
    }
    printf("freqbins: ");
    for (int i = 0; i < F; ++i) {
        printf("%.2f+i%.2f, ", freqbins[i].real(), freqbins[i].imag());
    }
    printf("\n\n");

    return freqbins;

}

void FFTView::calc() {
    const sf::Int16* smpls = mBuffer->getSamples();
    //double samples[] = {0, 0.77, 1, 0.77, 0, -0.77, -1, 0.77};

    std::vector<sf::Int16> samples(&smpls[0], &smpls[79999]);

    //int N = mBuffer->getSampleCount();
    int N = 80000;
    int max_freq = 20000;//mBuffer->getSampleRate() / 2;
    double mag[max_freq];


    /*std::vector<std::complex<double>> csamples(N, 0);
    for (int i = 0; i < N; ++i) {
        csamples[i].real( (double)samples[i*2]);
    }*/

    sf::Clock clock;
    clock.restart();

    std::vector<double> inputreal(N/2, 0);
    for (int i = 0; i < N/2; ++i) {
        inputreal[i] = (double)samples[i*2];
    }

    std::vector<double> freqbins(inputreal);
    std::vector<double> actualoutimag(N/2, 0);
    Fft::transform(freqbins, actualoutimag);

    /******** DFT
    for (int k = 0; k < max_freq; ++k) {
        double real = 0;
        double imag = 0;
        for (int n = 0; n < mFFTSize; ++n) {
            real += (samples[n*2] * cos(2*PI*k*n/N));
            imag += (samples[n*2] * sin(2*PI*k*n/N));
        }
        mag[k] = 2 * sqrt((real*real)+(imag*imag)) / (half_16*mFFTSize);
        //phase[k] =
    }*/

    //std::vector<std::complex<double>> freqbins = FFT(csamples);

    printf("FFT done after: %ims\n", clock.restart().asMilliseconds());
    printf("N/2 == %i\n", N/2);

    // scale logarithmically
    //int base = 2;
    for (int i=0;i<max_freq;++i) {
        mag[i] = sqrt( pow(freqbins[i] / N, 2) + pow(actualoutimag[i] / N, 2) );
        //mag[i] = exp(mag[i])-1;
        //mag[i] = pow(30.0, mag[i])-1;
        //mag[i] = (pow(base,mag[i])-1)/(base-1);
        mag[i] = log10( (9 * mag[i]) + 1);
    }

    double max = 0;
    for (int i=0;i<max_freq;++i) {
        if (mag[i] > max) {
            max = mag[i];
        }
    }
    printf("MAX MAG: %f\n", max);

    // Generating bar graph
    int num_bars = mWidth;
    int padding = 0;
    int bar_width = (double)(mWidth / (num_bars + (padding * num_bars) ) );
    int k_width = (int)round((double)max_freq / num_bars);
    printf("k_width: %i, you lost %i\n", k_width, max_freq%num_bars);

    for (int i = 0; i < num_bars; ++i) {
        double average = 0;
        for (int k = 0; k < k_width; ++k) {
            average += mag[k + (i * k_width)];
        }
        average = average / k_width;

        int bar_height = (int)round(average * mHeight / max);
        int new_width = (int)round( mWidth*3/ (2 * pow(i, 3/2)) );
        //int new_x = (int)round( new_width * sqrt(i) );
        int new_x = (2*new_width/3) * pow(i, 3/2);
        sf::RectangleShape bar(sf::Vector2f(new_width, bar_height));
        bar.setPosition(sf::Vector2f(new_x, mHeight - bar_height));
        bar.setFillColor(sf::Color(200, 220, 255, 255));
        //bar.setFillColor(sf::Color(200, 220*(1/average), 255*(1/average), 255));
        if ((i * k_width) % 1000 == 0) {
            bar.setFillColor(sf::Color(255, 0, 128, 255));
        }
        if (new_width > 3) {
            bar.setOutlineThickness(-1);
            bar.setOutlineColor(sf::Color::Black);
        }
        mBars.push_back(bar);
    }
}

void FFTView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    int num_bars = mBars.size();
    for (int i = 0; i < num_bars; ++i) {
        target.draw(mBars[i], states);
    }
};

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("Argument pointing to audio file required.\n");
        return -1;
    }

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(argv[1])) {
        printf("Error loading file.\n"  );
    }

    FFTView fft(4096, &buffer, RES_X - 50, RES_Y - 50);
    fft.calc();
    WAVView wav(buffer, RES_X - 50, RES_Y - 50);
    CoordinateSystem cSystem(buffer.getSampleRate() / 2, 1.0, RES_X, RES_Y);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;

    sf::RenderWindow window(sf::VideoMode(RES_X, RES_Y), "HI", sf::Style::Default, settings);
    sf::RenderTexture texture;
    if (!texture.create(RES_X - 50, RES_Y - 50)) {
        return -1;
    }

    sf::Font font;
    if (!font.loadFromFile("roboto.ttf")) {
        return -1;
    }

    sf::Text fps_string;
    fps_string.setFont(font);
    fps_string.setCharacterSize(24);
    fps_string.setColor(sf::Color::Blue);

    sf::Clock clock;
    clock.restart();

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
