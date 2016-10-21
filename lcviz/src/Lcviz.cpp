#include "Lcviz.hpp"

sf::Uint16 RES_X = 1920;
sf::Uint16 RES_Y = 1080;

//sf::Uint16 RES_X = 2560;
//sf::Uint16 RES_Y = 1440;

Lcviz::Lcviz(int argc, char** argv) {
    // FONT
    //char* fontPath = "/usr/share/fonts/TTF/Roboto-Regular.ttf";
    std::string fontPath = "/usr/share/fonts/TTF/InputSansCondensed-Regular.ttf";
    if (!mFont.loadFromFile(fontPath)) {
        printf("Please install font.\n");
    }

    // FPS STRING
    sf::Text mFpsString;
    mFpsString.setFont(mFont);
    mFpsString.setCharacterSize(22);
    mFpsString.setFillColor(sf::Color::White);
    //mFpsString.setPosition(250, RES_Y - 116);
    mFpsString.setPosition(10, 10);

    // RENDERWINDOW + GUI
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;

    mWindow.create(sf::VideoMode(RES_X, RES_Y), "Logic Circuit visualizer", sf::Style::Default, settings);
    //mWindow.resetGLStates();

}
/*
void Lcviz::update() {

}
*/
void Lcviz::loop() {
    mClock.restart();

    printf("Current Window size is %i x %i\n", mWindow.getSize().x, mWindow.getSize().y);

    sf::RectangleShape testShape(sf::Vector2f(RES_X, 2));
    testShape.setPosition(sf::Vector2f((RES_X/2) - 5, (RES_Y/2) - 5 ));

    while (mWindow.isOpen()) {
        sf::Event event;
        while (mWindow.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    mWindow.close();
                    break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) {
                        mWindow.close();
                    }
                    break;

                case sf::Event::Resized:
                    printf("new dimensions: %ix%i\n", event.size.width, event.size.height);
                    /*mCSystem.setDimensions(event.size.width, event.size.height - 150);
                    mFFTView.setDimensions(event.size.width - 50, event.size.height - 200);
                    mFFTTexture.create(event.size.width - 50, event.size.height - 150);
                    mFFTTexture.draw(mFFTView);

                    testShape.setPosition(sf::Vector2f((event.size.width/2) - 5, (event.size.height/2) - 5 ));*/

                case sf::Event::LostFocus:
                printf("Paused game\n");
                    mPaused = true;
                    break;

                case sf::Event::GainedFocus:
                    printf("Unpaused game\n");
                    mPaused = false;
                    break;

                default:
                    break;
            }
        }

        if (mPaused) {
            sf::sleep(sf::seconds(0.1f));
            continue;
        }

        sf::Time elapsed = mClock.restart();
        //sf::String mseconds(elapsed.asMilliseconds());
        if (elapsed.asMilliseconds() > 0) {
            mFpsString.setString(std::to_string(1000/elapsed.asMilliseconds()));
        } else {
            mFpsString.setString("0");
        }

        mWindow.clear();

        //mWindow.draw(mCSystem);
        mWindow.draw(mFpsString);
        mWindow.draw(testShape);
        mWindow.display();

    }
}
