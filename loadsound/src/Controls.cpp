#include "Controls.hpp"

Controls::Controls(int width, int height, FFTView* fft_view) {
    mWidth = width;
    mHeight = height;
    mFFTView = fft_view;
}

void Controls::addElement(ControlElement* element) {
    element->setParentControl(this);
    mControlElements.push_back(element);
}

void Controls::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    int len = mControlElements.size();
    for (int i=0; i < len; ++i) {
        target.draw(*mControlElements[i], states);
    }
}

void Controls::handleEvent(const sf::Event& event) {
    int len = mControlElements.size();
    for (int i=0; i < len; ++i) {
        mControlElements[i]->handleEvent(event);
    }
}

void Controls::cycleFFTSize() {
    int fft_size = mFFTView->getFFTSize();
    if (fft_size >= 32768) {
        fft_size = 512;
    } else {
        fft_size <<= 1;
    }
    mFFTView->setFFTSize(fft_size);
}
