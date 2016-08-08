.PHONY: build clean

PRJDIRS := src

SRCFILES := $(shell find $(PRJDIRS) -type f -name "*.cpp")
HDRFILES := $(shell find $(PRJDIRS) -type f -name "*.hpp")

WARNINGS := -Wall -Wextra
CPPFLAGS := -g -std=c++11 $(WARNINGS)

default: build

build:
	@$(CXX) $(CPPFLAGS) -o build/main $(SRCFILES) $(LINKFLAGS)

run: build
	./build/main

clean:
	-@$(RM) -r build/*
