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

std::vector<std::complex<double>> FFT(std::vector<std::complex<double>> &samples) {
    int mMaxFreq = 20000;
    int N =samples.size();
    if (N == 1) { return samples; }

    int M = N / 2;

    std::vector<std::complex<double>> Xeven(M, 0);
    std::vector<std::complex<double>> Xodd(M, 0);

    for (int i = 0; i < M; ++i) {
        Xeven[i] = samples[2 * i];
        Xodd[i] = samples[2 * i + 1];
    }

    if (M > mMaxFreq) { M = mMaxFreq; }

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
    if (N > mMaxFreq) { F = mMaxFreq; }
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
    /*
    samples[] = {0, 0.77, 1, 0.77, 0, -0.77, -1, 0.77};
    int N = 8;
    int mMaxFreq = N;
    double mag[mMaxFreq];
    */

    const sf::Int16* samples = mBuffer->getSamples();
    int N = mBuffer->getSampleCount() / 2;
    mMaxFreq = 20000;
    mMag = new double[mMaxFreq];

    sf::Clock clock;
    clock.restart();

    if (mMode == FFTMODE_FFT) {
        std::vector<double> inputreal(N, 0);
        for (int i = 0; i < N; ++i) {
            inputreal[i] = (double)samples[i*2];
        }

        std::vector<double> freqbins(inputreal);
        std::vector<double> actualoutimag(N, 0);
        Fft::transform(freqbins, actualoutimag);

    } else if (mMode == FFTMODE_RFFT) {
        std::vector<std::complex<double>> csamples(N, 0);
        for (int i = 0; i < N; ++i) {
            csamples[i].real( (double)samples[i*2] );
        }

        std::vector<std::complex<double>> freqbins = FFT(csamples);

    } else if (mMode == FFTMODE_DFT) {
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

    } else {
        printf("Invalid Mode\n");
    }

    printf("FFT done after: %ims\n", clock.restart().asMilliseconds());
    printf("N/2 == %i\n", N/2);

    // scale logarithmically
    //int base = 2;
    for (int i=0;i<mMaxFreq;++i) {
        // abs(x)
        //mag[i] = sqrt( pow(freqbins[i] / N, 2) + pow(actualoutimag[i] / N, 2) );

        mMag[i] = log10( (9 * mMag[i]) + 1);
    }

    // Get maximum to normalize
    double max = 0;
    for (int i=0;i<mMaxFreq;++i) {
        if (mMag[i] > max) {
            max = mMag[i];
        }
    }
    printf("MAX MAG: %f\n", max);

}

 void FFTView::render() {

    // Generating bar graph
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

        if (mGraphMode == GRAPHMODE_BARS) {
            // Bar width in px
            //int actual_bar_start = log10scale_reverse<double>((double)(i+1), (double)1, (double)mWidth) * mWidth;
            //int actual_bar_end = log10scale_reverse<double>((double)(i+2), (double)1, (double)mWidth) * mWidth;

            /*
            double actual_bar_start = f_to_x(k_start, (double)mWidth);
            double actual_bar_end = f_to_x(k_end, (double)mWidth);
            int actual_bar_width = (int)round(actual_bar_end - actual_bar_start);
            */
            bar_position = (int)round(f_to_x(k_start, (double)mWidth));
            int bar_end = (int)round(f_to_x(k_end, (double)mWidth));
            bar_width = bar_end - bar_position;
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

        } else if (mGraphMode == GRAPHMODE_INTENSITY) {
            bar = sf::RectangleShape(sf::Vector2f(1, mHeight));
            bar.setPosition(sf::Vector2f(bar_position, 0));
            bar.setFillColor(sf::Color(120, 180, 255, normalized_value*255));
        }

        mBars.push_back(bar);
    }

 }

void FFTView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.clear(sf::Color(30, 30, 30, 255));
    int num_bars = mBars.size();
    for (int i = 0; i < num_bars; ++i) {
        target.draw(mBars[i], states);
    }
};
