#ifndef _COORDINATE_SYSTEM_HPP
#define _COORDINATE_SYSTEM_HPP

#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <string>

class CoordinateSystem : public sf::Drawable {
private:
    double mXScale, mYScale;
    int mWidth, mHeight;
    sf::RectangleShape mXAxis, mYAxis;
    std::vector<sf::RectangleShape> mStepSegments;
    std::vector<sf::Text> mLabels;
    sf::Font mFont;

    void draw(sf::RenderTarget&, sf::RenderStates) const;
public:
    CoordinateSystem();
    CoordinateSystem(double, double, int, int, std::string);
    void create(double, double, int, int, std::string);
    void setDimensions(int, int);
    void render();
};

#endif
