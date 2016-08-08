#ifndef _CONTROL_ELEMENT_HPP
#define _CONTROL_ELEMENT_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Controls.hpp"

enum CONTROL_ELEMENT_TYPE { TYPE_BUTTON };

class ControlElement : public sf::Drawable {
private:
    CONTROL_ELEMENT_TYPE mElementType;
    int mWidth, mHeight, mPosX, mPosY;
    sf::RectangleShape mShape;
    Controls* mParentControl;
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;

public:
    ControlElement(int, int, int, int, CONTROL_ELEMENT_TYPE, sf::String&);
    void handleEvent(const sf::Event&);
    void setParentControl(Controls*);
};

#endif
