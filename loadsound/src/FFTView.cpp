#include <complex>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>

#include "FFTView.hpp"
#include "../include/Utils.hpp"

FFTView::FFTView() {}

FFTView::FFTView(int fft_size, sf::SoundBuffer* buffer, int width, int height, FFTMODE mode, GRAPHMODE graph_mode) {
    this->create(fft_size, buffer, width, height, mode, graph_mode);
}

void FFTView::create(int fft_size, sf::SoundBuffer* buffer, int width, int height, FFTMODE mode, GRAPHMODE graph_mode) {
    mFFTSize = fft_size;
    mBuffer = buffer;
    mWidth = width;
    mHeight = height;
    mMode = mode;
    mGraphMode = graph_mode;

    if ((mFFTSize & (mFFTSize - 1)) != 0 ) {
        printf("fft size has to be a power of 2\n");
    }

    const unsigned Min = 1;
    const unsigned Max = 20;

    FactoryInit<GFFTList<GFFT,Min,Max>::Result>::apply(mGfftFactory);
    mMag = new double[mFFTSize];

    int p = log2(mFFTSize);
    printf("p = %i, fftsize = %i\n", p, mFFTSize);

    mGfft = mGfftFactory.CreateObject(p);

    int N = buffer->getSampleCount();
    int sample_rate = buffer->getSampleRate();
    int channel_count = buffer->getChannelCount();
    float length = (float)N / (float)sample_rate / channel_count;

    printf("Got %i samples @%i / %i\n", N, sample_rate, channel_count);
    printf("Length is %f\n\n", length);
}

void FFTView::setFFTSize(int fftsize) {
    int p = log2(fftsize);
    printf("p = %i, fftsize = %i\n", p, fftsize);
    mGfft = mGfftFactory.CreateObject(p);
    mFFTSize = fftsize;
    calc();
    render();
}

void FFTView::setBuffer(sf::SoundBuffer* buffer) {
    mBuffer = buffer;
}

void FFTView::setDimensions(int width, int height) {
    mWidth = width;
    mHeight = height;
    this->render();
}

void FFTView::calc() {
    const sf::Int16* samples = mBuffer->getSamples();
    int N = mBuffer->getSampleCount();

    int half_16 = (1 << 16) / 2;

    mMaxFreq = 20000;

    sf::Clock clock;
    clock.restart();

    if (mMode == FFTMODE_DFT) {
        mMag = new double[mMaxFreq];
        int N = mBuffer->getSampleCount() / 2;

        // DFT
        for (int k = 0; k < mMaxFreq; ++k) {
            double real = 0;
            double imag = 0;
            for (int n = 0; n < mFFTSize; ++n) {
                real += (samples[n*2] * cos(2*PI*k*n/N));
                imag += (samples[n*2] * sin(2*PI*k*n/N));
            }
            mMag[k] = sqrt((real*real)+(imag*imag)) / (half_16*mFFTSize);
            //phase[k] =
        }

    } else if (mMode == FFTMODE_FASTFFT) {

        double* data = new double[2 * mFFTSize];

        for (int i=0; i < mFFTSize; ++i) {
            data[2*i] = samples[2*i];
            data[2*i+1] = 0;
        }

        mGfft->fft(data);

        for (int i=0; i < mFFTSize; ++i) {
            mMag[i] = sqrt((data[2*i] * data[2*i]) + (data[2*i+1] * data[2*i+1])) / (mFFTSize * half_16);
        }

    } else {
        printf("Invalid Mode\n");
    }

    printf("FFT done after: %ius\n", (int)clock.restart().asMicroseconds());

    mMax = 1;
    /*for (int i=0;i<mFFTSize;++i) {
        if (mMag[i] > mMax) {
            mMax = mMag[i];
        }
    }*/

    printf("Max value = %f.2\n", mMax);

}

void FFTView::render() {

    mBars.clear();

    double ratio = (double)mBuffer->getSampleRate() / (double)mFFTSize;

    for (int i=0; i < mFFTSize; ++i) {
        double freq = i * ratio;
        int bar_position = (int)(( (double)mWidth / 3 ) * log10(freq / 20));
        double normalized_value = mMag[i] / mMax;

        sf::RectangleShape bar;
        bar = sf::RectangleShape(sf::Vector2f(1, mHeight));
        bar.setPosition(sf::Vector2f(bar_position, 0));
        bar.setFillColor(sf::Color(120, 180, 255, normalized_value*255));

        mBars.push_back(bar);
    }

}

void FFTView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.clear(sf::Color(20, 20, 20, 255));
    int num_bars = mBars.size();
    for (int i = 0; i < num_bars; ++i) {
        target.draw(mBars[i], states);
    }
}

int FFTView::getFFTSize() const {
    return mFFTSize;
}
