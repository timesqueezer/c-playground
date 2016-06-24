#ifndef _WAVVIEW_HPP
#define _WAVVIEW_HPP

#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class WAVView : public sf::Drawable {
private:
    std::vector<sf::Vertex> mVertices;
    int mWidth, mHeight;
    virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
public:
    WAVView(sf::SoundBuffer&, int, int);
};

#endif
