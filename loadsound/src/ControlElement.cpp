#include "ControlElement.hpp"
#include <iostream>


ControlElement::ControlElement(int width, int height, int pos_x, int pos_y, CONTROL_ELEMENT_TYPE type, sf::String& text) {
    mWidth = width;
    mHeight = height;
    mPosX = pos_x;
    mPosY = pos_y;
    mElementType = type;
    if (mElementType == TYPE_BUTTON) {
        mShape = sf::RectangleShape(sf::Vector2f(mWidth, mHeight));
        mShape.setPosition(sf::Vector2f(mPosX, mPosY));
        mShape.setFillColor(sf::Color(240, 240, 240));
        mShape.setOutlineThickness(1);
        mShape.setOutlineColor(sf::Color(220, 220, 220));
    }
}

void ControlElement::setParentControl(Controls* control) {
    mParentControl = control;
}

void ControlElement::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mShape);
}

void ControlElement::handleEvent(const sf::Event& event) {
    sf::FloatRect rect = mShape.getGlobalBounds();
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            std::cout << "the left button was pressed" << std::endl;
            std::cout << "mouse x: " << event.mouseButton.x << std::endl;
            std::cout << "mouse y: " << event.mouseButton.y << std::endl;
            if (rect.contains(event.mouseButton.x, event.mouseButton.y)) {
                printf("YES IM IN\n");
                mParentControl->cycleFFTSize();
            } else {
                printf(":(\n");
            }
        }
    }
}