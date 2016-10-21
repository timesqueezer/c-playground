.PHONY: build clean

PRJDIRS := src include

SRCFILES := $(shell find $(PRJDIRS) -type f -name "*.cpp")
HDRFILES := $(shell find $(PRJDIRS) -type f -name "*.hpp")

LIBS = sfml-system sfml-window sfml-graphics sfml-audio
LINKFLAGS = $(addprefix -l, $(LIBS))

WARNINGS := -Wall -Wextra
CPPFLAGS := -g -std=c++11 $(WARNINGS)

default: build

build:
	@$(CXX) $(CPPFLAGS) -o build/main $(SRCFILES) $(LINKFLAGS)

run: build
	./build/main

clean:
	-@$(RM) -r build/*
