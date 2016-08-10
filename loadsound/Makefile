.PHONY: build clean

PRJDIRS := src include fastfft

SRCFILES := $(shell find $(PRJDIRS) -type f -name "*.cpp")
HDRFILES := $(shell find $(PRJDIRS) -type f -name "*.hpp")

LIBS = sfml-system sfml-window sfml-graphics sfml-audio sfgui
LINKFLAGS = $(addprefix -l, $(LIBS))

WARNINGS := -Wall -Wextra
CPPFLAGS := -g -std=c++11 $(WARNINGS)

default: build

build:
	@$(CXX) $(CPPFLAGS) -o build/main $(SRCFILES) $(LINKFLAGS)

run: build
	./build/main samples/1s1k5k.wav 4096

clean:
	-@$(RM) -r build/*
