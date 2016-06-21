.PHONY: build clean

PRJDIRS := src include fft

SRCFILES := $(shell find $(PRJDIRS) -type f -name "*.cpp")
HDRFILES := $(shell find $(PRJDIRS) -type f -name "*.hpp")

LIBS = sfml-system sfml-window sfml-graphics sfml-audio
LINKFLAGS = $(addprefix -l, $(LIBS))

WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wuninitialized -Wconversion -Wstrict-prototypes
CPPFLAGS := -g -std=c++11 # $(WARNINGS)

default: build

build:
	@$(CXX) $(CPPFLAGS) -o build/main $(SRCFILES) $(LINKFLAGS)

clean:
	-@$(RM) -r build/*
