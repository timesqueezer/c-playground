#ifndef _FFTVIEW_HPP
#define _FFTVIEW_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#define PI 3.14159265358979323846264338327950288419716939937510

enum FFTMODE { FFTMODE_DFT, FFTMODE_RFFT, FFTMODE_FFT };
enum GRAPHMODE { GRAPHMODE_INTENSITY, GRAPHMODE_BARS };

class FFTView : public sf::Drawable {
private:
    int mFFTSize, mWidth, mHeight;
    sf::SoundBuffer* mBuffer;
    std::vector<sf::RectangleShape> mBars;
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
    FFTMODE mMode;
    GRAPHMODE mGraphMode;

public:
    FFTView(int, sf::SoundBuffer*, int, int, FFTMODE, GRAPHMODE);
    void calc();
};

#endif
