#include "Analyzer.hpp"

sf::Uint16 RES_X = 2050;
sf::Uint16 RES_Y = 1080;

//sf::Uint16 RES_X = 2560;
//sf::Uint16 RES_Y = 1440;

Analyzer::Analyzer(int argc, char** argv) {
    if (argc <= 1) {
        printf("Argument pointing to audio file required.\n");
    }

    if (!mBuffer.loadFromFile(argv[1])) {
        printf("Error loading file.\n");
    }

    // FONT
    //char* fontPath = "/usr/share/fonts/TTF/Roboto-Regular.ttf";
    std::string fontPath = "/usr/share/fonts/TTF/InputSansCondensed-Regular.ttf";
    if (!mFont.loadFromFile(fontPath)) {
        printf("Please install font.\n");
    }

    // STRINGS

    // FPS STRING
    sf::Text mFpsString;
    mFpsString.setFont(mFont);
    mFpsString.setCharacterSize(22);
    mFpsString.setFillColor(sf::Color::White);
    mFpsString.setPosition(250, RES_Y - 116);

    // FFT-SIZE STRING
    sf::Text mFFTString;
    mFFTString.setFont(mFont);
    mFFTString.setCharacterSize(22);
    mFFTString.setFillColor(sf::Color::White);
    mFFTString.setPosition(250, RES_Y - 66);


    // FFT
    int fft_size = argc >= 3 ? atoi(argv[2]) : 128;
    //GRAPHMODE graph_mode = GRAPHMODE_BARS;
    GRAPHMODE graph_mode = GRAPHMODE_INTENSITY;
    //FFTMODE fft_mode = FFTMODE_DFT;
    FFTMODE fft_mode = FFTMODE_FASTFFT;

    mFFTView.create(fft_size, &mBuffer, RES_X - 50, RES_Y - 200, fft_mode, graph_mode);

    // COORDINATESYSTEM
    mCSystem.create(mBuffer.getSampleRate() / 2, 1.0, RES_X, RES_Y - 150, fontPath);

    // RENDERWINDOW + GUI
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;

    mWindow.create(sf::VideoMode(RES_X, RES_Y), "Audio Analyzer", sf::Style::Default, settings);
    mWindow.resetGLStates();

    //mGuiWindow = sfg::Window::Create();
    //auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.f );
    //mFFTButton = sfg::Button::Create( "Click me" );
    //box->Pack( mFFTButton );
    //mFFTButton->GetSignal( sfg::Widget::OnLeftClick ).Connect( std::bind( &Analyzer::FFTSizeToggleCallback, this ) );

    //mGuiWindow->Add( box );

    // RENDERTEXTURES
    if (!mFFTTexture.create(RES_X - 50, RES_Y - 200)) {
        printf("Failed to create fft-texture.\n");
    }
}

void Analyzer::update() {
    mFFTView.calc();
    mFFTView.render();
    mFFTString.setString(std::to_string(mFFTView.getFFTSize()));
    mFFTTexture.clear();

    mFFTTexture.draw(mFFTView);
    //mFFTTexture.draw(wav);
    mFFTTexture.display();

    mCSystem.render();
}

void Analyzer::loop() {
    mClock.restart();

    printf("Current Window size is %i x %i\n", mWindow.getSize().x, mWindow.getSize().y);

    /*sf::RectangleShape testShape(sf::Vector2f(RES_X, 2));
    testShape.setPosition(sf::Vector2f((RES_X/2) - 5, (RES_Y/2) - 5 ));*/

    while (mWindow.isOpen())
    {
        sf::Event event;
        while (mWindow.pollEvent(event))
        {
            mGuiWindow->HandleEvent( event );
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
                    mPaused = true;

                case sf::Event::GainedFocus:
                    mPaused = false;

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

        mGuiWindow->Update( elapsed.asSeconds() );

        mWindow.clear();

        sf::Sprite inner(mFFTTexture.getTexture());
        inner.setPosition(sf::Vector2f(50, 0));
        mWindow.draw(inner);

        mWindow.draw(mCSystem);
        mSfgui.Display( mWindow );
        mWindow.draw(mFpsString);
        mWindow.draw(mFFTString);
        //mWindow.draw(testShape);
        mWindow.display();

    }
}

void Analyzer::FFTSizeToggleCallback() {
    printf("Callback :D\n");
    //mFFTView
}
