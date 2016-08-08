#include <complex>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <stdio.h>

#include "FFTView.hpp"
#include "../include/Utils.hpp"
#include "../fft/fft.hpp"

FFTView::FFTView(int fft_size, sf::SoundBuffer* buffer, int width, int height, FFTMODE mode, GRAPHMODE graph_mode) {
    mFFTSize = fft_size;
    mBuffer = buffer;
    mWidth = width;
    mHeight = height;
    mMode = mode;
    mGraphMode = graph_mode;

    int N = buffer->getSampleCount();
    int sample_rate = buffer->getSampleRate();
    int channel_count = buffer->getChannelCount();
    float length = (float)N / (float)sample_rate / channel_count;

    printf("Got %i samples @%i / %i\n", N, sample_rate, channel_count);
    printf("Length is %f\n\n", length);
}

void FFTView::setDimensions(int width, int height) {
    mWidth = width;
    mHeight = height;
    this->render();
}

void FFTView::calc() {
    const sf::Int16* samples = mBuffer->getSamples();
    int N = mBuffer->getSampleCount() / 2;
    mMaxFreq = 20000;

    sf::Clock clock;
    clock.restart();

    if (mMode == FFTMODE_DFT) {
        mMag = new double[mMaxFreq];

        int half_16 = (1 << 8) / 2;

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
        mMag = new double[mFFTSize];

        if ((mFFTSize & (mFFTSize - 1)) != 0 ) {
            printf("fft size has to be a power of 2\n");
        }

        const unsigned Min = 1;
        const unsigned Max = 20;

        Loki::Factory<AbstractFFT<double>,unsigned int> gfft_factory;
        FactoryInit<GFFTList<GFFT,Min,Max>::Result>::apply(gfft_factory);

        int p = log2(mFFTSize);
        printf("p = %i\n", p);

        AbstractFFT<double>* gfft = gfft_factory.CreateObject(p);
        double* data = new double[2 * mFFTSize];

        for (int i=0; i < mFFTSize; ++i) {
            data[2*i] = samples[2*i];
            data[2*i+1] = 0;
        }

        gfft->fft(data);

        for (int i=0; i < mFFTSize; ++i) {
            mMag[i] = sqrt((data[2*i] * data[2*i]) + (data[2*i+1] * data[2*i+1]));
        }

        /*for (int i=0; i<max_freq; ++i) {
            mag[i] = i <= mFFTSize ? sqrt((data[2*i] * data[2*i]) + (data[2*i+1] * data[2*i+1])) / mFFTSize : 0;
        }*/

    } else {
        printf("Invalid Mode\n");
    }

    printf("FFT done after: %ius\n", clock.restart().asMicroseconds());

    mMax = 0;
    for (int i=0;i<mFFTSize;++i) {
        if (mMag[i] > mMax) {
            mMax = mMag[i];
        }
    }

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

    /*// Generating bar graph
    int num_bars = mWidth;
    //int padding = 0;

    int mem_k_start = 0;

    for (int i = 0; i < num_bars; ++i) {
        // Frequency-width
        //double k_start = mem_k_start != 0 ? mem_k_start : log10scale_reverse<double>((double)(i+1), (double)1, (double)num_bars) * mMaxFreq;
        //double k_end = log10scale_reverse<double>((double)(i+2), (double)1, (double)num_bars) * mMaxFreq;
        double k_start = mem_k_start != 0 ? mem_k_start : x_to_freq((double)i/(double)num_bars, (double)mMaxFreq);
        double k_end = x_to_freq((double)(i+1)/num_bars, (double)mMaxFreq);
        int k_width = (int)(floor(k_end) - floor(k_start));

        int bar_position;
        int bar_width;

        if (mGraphMode == GRAPHMODE_BARS) {*/
            // Bar width in px
            //int actual_bar_start = log10scale_reverse<double>((double)(i+1), (double)1, (double)mWidth) * mWidth;
            //int actual_bar_end = log10scale_reverse<double>((double)(i+2), (double)1, (double)mWidth) * mWidth;

            /*
            double actual_bar_start = f_to_x(k_start, (double)mWidth);
            double actual_bar_end = f_to_x(k_end, (double)mWidth);
            int actual_bar_width = (int)round(actual_bar_end - actual_bar_start);
            */
/*
            bar_position = (int)round(f_to_x(k_start, (double)mWidth));
            int bar_end = (int)round(f_to_x(k_end, (double)mWidth));
            bar_width = bar_end - bar_position;
        /*
        } else if (mGraphMode == GRAPHMODE_INTENSITY) {
            // Frequency to x-coordinate
            //bar_position = (int)(( (double)mWidth / 3 ) * log10(k_start / 20));
            bar_position = (int)round(f_to_x(k_start, (double)mWidth));
        }
        //printf("i: %i, k_width: %f - %f = %i, bar(end - start) = width: %f - %f = %i\n", i, k_start, k_end, k_width, actual_bar_end, actual_bar_start, actual_bar_width);

        if (k_width < 1 && mem_k_start == 0) {
            mem_k_start = k_start;
            continue;
        }
        if (k_width >= 1 && mem_k_start != 0) {
            bar_position = (int)((double)(mWidth) / 3) * log10((double)(mem_k_start + ((double)(k_end - mem_k_start) / 2)) / 20);
            mem_k_start = 0;
        }

        if (mem_k_start != 0) {
            continue;
        }

        double average = 0;
        for (int k = floor(k_start); k < floor(k_end); ++k) {
            average += mMag[k];
        }
        average = average / k_width;

        //int bar_height = (int)round(average * mHeight / max);
        double normalized_value = average / mMax;

        //printf("i: %i, k_width: %f - %f = %i, bar position: %i, avg: %f\n", i, k_start, k_end, k_width, bar_position, normalized_value);

        sf::RectangleShape bar;

        if (mGraphMode == GRAPHMODE_BARS) {
            int bar_height = normalized_value * mHeight;
            bar = sf::RectangleShape(sf::Vector2f((int)bar_width, bar_height));
            bar.setPosition(sf::Vector2f(bar_position, mHeight - bar_height));
            bar.setFillColor(sf::Color(120, 180, 255, 255));
            //bar.setFillColor(sf::Color(200, 220*(1/average), 255*(1/average), 255));
            /*if (1000 - (k_start / (k_start / 1000)) < (k_width/2) ) {
                bar.setFillColor(sf::Color(255, 0, 128, 255));
            }*/
            /*if (actual_bar_width > 10) {
                bar.setOutlineThickness(-1);
                bar.setOutlineColor(sf::Color::Black);
            }*/
/*
        } else if (mGraphMode == GRAPHMODE_INTENSITY) {
            bar = sf::RectangleShape(sf::Vector2f(1, mHeight));
            bar.setPosition(sf::Vector2f(bar_position, 0));
            bar.setFillColor(sf::Color(120, 180, 255, normalized_value*255));
        }

        mBars.push_back(bar);
    }*/

 }

void FFTView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.clear(sf::Color(20, 20, 20, 255));
    int num_bars = mBars.size();
    for (int i = 0; i < num_bars; ++i) {
        target.draw(mBars[i], states);
    }
};
