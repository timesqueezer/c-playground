#ifndef _CONTROLS_HPP
#define _CONTROLS_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "FFTView.hpp"

class ControlElement;
class Controls : public sf::Drawable {
private:
    std::vector<ControlElement*> mControlElements;
    int mWidth, mHeight;
    FFTView* mFFTView;
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

public:
    Controls(int, int, FFTView*);
    void handleEvent(const sf::Event&);
    void addElement(ControlElement*);
    void cycleFFTSize();
};

#endif
