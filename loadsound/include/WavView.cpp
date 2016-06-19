#include "WavView.hpp"


WAVView::WAVView(sf::SoundBuffer& buffer, int width, int height) {
    mWidth = width;
    mHeight = height;

    const sf::Int16* samples = buffer.getSamples();
    int N = buffer.getSampleCount();

    double double_width = (double)(width * 2);
    printf("Double width: %f\n", double_width);
    double lol = N / double_width;
    printf("Samples per pixel: %f\n", lol);

    int window_size = floor(lol);
    printf("Window Size: %i\n", window_size);
    printf("You lost %i samples\n", N % window_size);

    int half_16 = (1 << 16) / 2;
    int full_16 = 1 << 16;

    for (int i = 0; i < width; ++i) {
        // Average all samples for this pixel
        int start = i*window_size;
        int end = start+window_size;
        for (int j = start; j < end; ++j) {
            int x = (int)round((double)(samples[j*2] + half_16) / full_16 * height);
            mVertices.push_back(sf::Vertex(sf::Vector2f(i, x), sf::Color::White));
        }
    }
}

void WAVView::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(&mVertices[0], mVertices.size(), sf::LinesStrip);
}

