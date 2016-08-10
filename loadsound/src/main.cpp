#include "Analyzer.hpp"

int main(int argc, char** argv) {
    Analyzer analyzer(argc, argv);
    analyzer.update();
    analyzer.loop();
}