#ifndef _COORDINATE_SYSTEM_HPP
#define _COORDINATE_SYSTEM_HPP

#include <SFML/Graphics.hpp>
#include <cstdlib>

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

#endif
