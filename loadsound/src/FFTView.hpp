#ifndef _FFTVIEW_HPP
#define _FFTVIEW_HPP

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "../fastfft/FastFFT.hpp"

#define PI 3.14159265358979323846264338327950288419716939937510

enum FFTMODE { FFTMODE_DFT, FFTMODE_RFFT, FFTMODE_FFT, FFTMODE_FASTFFT };
enum GRAPHMODE { GRAPHMODE_INTENSITY, GRAPHMODE_BARS };

class FFTView : public sf::Drawable {
private:
    int mFFTSize, mWidth, mHeight;
    sf::SoundBuffer* mBuffer;
    std::vector<sf::RectangleShape> mBars;
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
    FFTMODE mMode;
    GRAPHMODE mGraphMode;
    double* mMag;
    double mMax;
    int mMaxFreq;

    Loki::Factory<AbstractFFT<double>, unsigned int> mGfftFactory;
    AbstractFFT<double>* mGfft;

public:
    FFTView();
    FFTView(int, sf::SoundBuffer*, int, int, FFTMODE, GRAPHMODE);
    void create(int, sf::SoundBuffer*, int, int, FFTMODE, GRAPHMODE);
    void setFFTSize(int);
    void setDimensions(int, int);
    void render();
    void calc();
    int getFFTSize() const;
};

#endif
