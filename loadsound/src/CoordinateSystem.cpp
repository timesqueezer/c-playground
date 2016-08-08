#include <SFML/Graphics.hpp>
#include <cmath>

#include "CoordinateSystem.hpp"

CoordinateSystem::CoordinateSystem(double x_scale, double y_scale, int width, int height, std::string font_path) {
    mXScale = x_scale;
    mYScale = y_scale;
    mWidth = width;
    mHeight = height;
    if (!mFont.loadFromFile(font_path)) {
        printf("Please install font.\n");
    }
}


void CoordinateSystem::setDimensions(int width, int height) {
    mWidth = width;
    mHeight = height;
    this->render();
}

void CoordinateSystem::render() {

    int border_width = 50;
    int line_width = 1;
    sf::Color color(255, 255, 255, 172);
    double step_y_size = mYScale / 10;
    int font_size = 16;

    // Set Axis up
    mXAxis = sf::RectangleShape(sf::Vector2f(mWidth - border_width, line_width));
    mXAxis.setPosition(sf::Vector2f(border_width, mHeight - border_width - line_width));
    mXAxis.setFillColor(color);

    // x-axis segments

    // Log scaling f(x, width) = 20 * 10^3(x / width)
    // Reverse:    x(f, width) = (max / 3)*math.log10(f / 20)
    int frequency_positions[10] = {20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    int segment_positions[10];

    for (int i = 0; i < 10; ++i) {
        // x(frequency_positions[i], mWidth)
        segment_positions[i] = (int) ((double)(mWidth - border_width) / 3) * log10((double)frequency_positions[i] / 20);
        segment_positions[i] += border_width;
    }

    // Linear scaling
    /*for (int i = 0; i < 10; ++i) {
        int segment_positions[i] = border_width + (i * mWidth / 10)
    }*/

    // Set-up Shapes etc.
    for (int i = 0; i < 10; ++i) {
        sf::RectangleShape segment(sf::Vector2f(line_width, 5));
        segment.setPosition(sf::Vector2f(segment_positions[i], mHeight - border_width));
        segment.setFillColor(color);
        mStepSegments.push_back(segment);

        sf::Text label = sf::Text();
        label.setFont(mFont);
        label.setCharacterSize(font_size);
        label.setColor(color);
        label.setPosition(sf::Vector2f(segment_positions[i] + (font_size / 2), mHeight - border_width + 10));
        label.setRotation(60);
        //int value = (int)(i * 1000);
        int value = frequency_positions[i];
        if (value >= 10000) {
            label.setString(std::to_string(value/1000) + 'k');
        } else {
            label.setString(std::to_string(value));
        }
        mLabels.push_back(label);
    }

    mYAxis = sf::RectangleShape(sf::Vector2f(line_width, mHeight - border_width));
    mYAxis.setPosition(sf::Vector2f(border_width, 0));
    mYAxis.setFillColor(color);

    int num_y_steps = mYScale / step_y_size;
    int step_y_spacing = (mHeight - 50) / num_y_steps;
    for (int i = 0; i < num_y_steps; ++i) {
        sf::RectangleShape segment(sf::Vector2f(5, line_width));
        segment.setPosition(sf::Vector2f(border_width - 5, mHeight - border_width - (i * step_y_spacing)));
        segment.setFillColor(color);
        mStepSegments.push_back(segment);

        sf::Text label = sf::Text();
        label.setFont(mFont);
        label.setCharacterSize(font_size);
        label.setColor(color);
        label.setPosition(sf::Vector2f(5, mHeight - border_width - (i * step_y_spacing) - (font_size / 2)));

        char str[4];
        sprintf(str, "%.2f", i * step_y_size);
        label.setString(str);
        mLabels.push_back(label);
    }
}


void CoordinateSystem::draw(sf::RenderTarget& window, sf::RenderStates states) const {
    // Draw Axis
    window.draw(mXAxis);
    window.draw(mYAxis);

    int num_segments = mStepSegments.size();
    for (int i = 0; i < num_segments; ++i) {
        window.draw(mStepSegments[i]);
        window.draw(mLabels[i]);
    }

}
