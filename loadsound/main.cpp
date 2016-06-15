#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <string>

#define PI           3.14159265358979323846  /* pi */

sf::Uint16 RES_X = 2050;
sf::Uint16 RES_Y = 1080;

class CoordinateSystem : public sf::Drawable {
private:
    double mXScale, mYScale;
    int mWidth, mHeight;
    sf::RectangleShape mXAxis, mYAxis;
    std::vector<sf::RectangleShape> mStepSegments;
    std::vector<sf::Text> mLabels;
    sf::Font mFont;

    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
public:
    CoordinateSystem(double, double, int, int);
};

CoordinateSystem::CoordinateSystem(double x_scale, double y_scale, int width, int height) {
    mXScale = x_scale;
    mYScale = y_scale;
    mWidth = width;
    mHeight = height;

    int border_width = 50;
    int line_width = 1;
    sf::Color color(255, 255, 255, 172);
    double step_x_size = 500;
    double step_y_size = y_scale / 10;
    int font_size = 16;

    mFont.loadFromFile("roboto.ttf");

    // Set Axis up
    mXAxis = sf::RectangleShape(sf::Vector2f(width - border_width, line_width));
    mXAxis.setPosition(sf::Vector2f(border_width, height - border_width - line_width));
    mXAxis.setFillColor(color);

    int num_x_steps = x_scale / step_x_size;
    int step_x_spacing = (width - 50) / num_x_steps;
    for (int i = 0; i < num_x_steps; ++i) {
        sf::RectangleShape segment(sf::Vector2f(line_width, 5));
        segment.setPosition(sf::Vector2f(border_width + (i * step_x_spacing), height - border_width));
        segment.setFillColor(color);
        mStepSegments.push_back(segment);

        sf::Text label = sf::Text();
        label.setFont(mFont);
        label.setCharacterSize(font_size);
        label.setColor(color);
        label.setPosition(sf::Vector2f(border_width + (i * step_x_spacing) + (font_size / 2), height - border_width + 10));
        label.setRotation(80);
        int value = (int)(i * step_x_size);
        if (value >= 10000) {
            label.setString(std::to_string(value/1000) + 'k');
        } else {
            label.setString(std::to_string(value));
        }
        mLabels.push_back(label);
    }

    mYAxis = sf::RectangleShape(sf::Vector2f(line_width, height - border_width));
    mYAxis.setPosition(sf::Vector2f(border_width, 0));
    mYAxis.setFillColor(color);

    int num_y_steps = y_scale / step_y_size;
    int step_y_spacing = (height - 50) / num_y_steps;
    for (int i = 0; i < num_y_steps; ++i) {
        sf::RectangleShape segment(sf::Vector2f(5, line_width));
        segment.setPosition(sf::Vector2f(border_width - 5, height - border_width - (i * step_y_spacing)));
        segment.setFillColor(color);
        mStepSegments.push_back(segment);

        sf::Text label = sf::Text();
        label.setFont(mFont);
        label.setCharacterSize(font_size);
        label.setColor(color);
        label.setPosition(sf::Vector2f(5, height - border_width - (i * step_y_spacing) - (font_size / 2)));

        char str[4];
        sprintf(str, "%.2f", i * step_y_size);
        label.setString(str);
        mLabels.push_back(label);
    }

}

void CoordinateSystem::draw(sf::RenderTarget& window, sf::RenderStates states) const {
    // Draw Axis
    window.draw(mXAxis);
    window.draw(mYAxis);

    int num_segments = mStepSegments.size();
    for (int i = 0; i < num_segments; ++i) {
        window.draw(mStepSegments[i]);
        window.draw(mLabels[i]);
    }

}

class WAVView : public sf::Drawable {
private:
    std::vector<sf::Vertex> mVertices;
    int mWidth, mHeight;
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
public:
    WAVView(sf::SoundBuffer&, int, int);
};

WAVView::WAVView(sf::SoundBuffer& buffer, int width, int height) {
    mWidth = width;
    mHeight = height;

    const sf::Int16* samples = buffer.getSamples();
    int N = buffer.getSampleCount();

    double double_width = (double)(width * 2);
    printf("Double width: %f\n", double_width);
    double lol = N / double_width;
    printf("Samples per pixel: %f\n", lol);

    int window_size = floor(lol);
    printf("Window Size: %i\n", window_size);
    printf("You lost %i samples\n", N % window_size);

    int half_16 = (1 << 16) / 2;
    int full_16 = 1 << 16;

    for (int i = 0; i < width; ++i) {
        // Average all samples for this pixel
        int start = i*window_size;
        int end = start+window_size;
        for (int j = start; j < end; ++j) {
            int x = (int)round((double)(samples[j*2] + half_16) / full_16 * height);
            mVertices.push_back(sf::Vertex(sf::Vector2f(i, x), sf::Color::White));
        }
    }
}

void WAVView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(&mVertices[0], mVertices.size(), sf::LinesStrip);
}


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
    printf("Length is %f\n", length);
}

void FFTView::calc() {
    const sf::Int16* samples = mBuffer->getSamples();
    int half_16 = (1 << 16) / 2;
    //int full_16 = 1 << 16;

    int N = mBuffer->getSampleCount();
    int max_freq = 20000;//mBuffer->getSampleRate() / 2;
    double mag[max_freq];
    //double phase[max_freq];

    sf::Clock clock;
    clock.restart();

    for (int k = 0; k < max_freq; ++k) {
        double real = 0;
        double imag = 0;
        for (int n = 0; n < mFFTSize; ++n) {
            real += (samples[n*2] * cos(2*PI*k*n/N));
            imag += (samples[n*2] * sin(2*PI*k*n/N));
        }
        mag[k] = 2 * sqrt((real*real)+(imag*imag)) / (half_16*mFFTSize);
        //phase[k] =
    }

    printf("FFT done after: %ims\n", clock.restart().asMilliseconds());

    // scale logarithmically
    int base = 2;
    for (int i=0;i<max_freq;++i) {
        //mag[i] = exp(mag[i])-1;
        //mag[i] = pow(30.0, mag[i])-1;
        //mag[i] = (pow(base,mag[i])-1)/(base-1);
        mag[i] = log10( (9 * mag[i]) + 1);
    }

    /*
    double max = 0;
    for (int i=0;i<max_freq;++i) {
        if (mag[i] > max) {
            max = mag[i];
        }
    }
    printf("MAX MAG: %f\n", max);*/

    // Generating bar graph
    int num_bars = 1000;
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

        int bar_height = (int)round(average * mHeight);
        sf::RectangleShape bar(sf::Vector2f(bar_width, bar_height));
        bar.setPosition(sf::Vector2f(i*(bar_width + padding), mHeight - bar_height));
        bar.setFillColor(sf::Color(200, 220, 255, 255));
        //bar.setFillColor(sf::Color(200, 220*(1/average), 255*(1/average), 255));
        if ((i * k_width) % 1000 == 0) {
            bar.setFillColor(sf::Color(255, 0, 255, 255));
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
