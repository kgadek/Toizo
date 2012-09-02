CXX         =   clang++
CXXFLAGS    =   -O0 -g -ggdb3\
                -W -Wall -Wshadow -Wformat\
                -Wsequence-point -Wunused\
		        -Wuninitialized -Wfloat-equal -ansi\
		        -pedantic -Weffc++
LDFLAGS     =   -lm

.PHONY: clean doxy all

all: t doxy
	./tests/tester.sh ./t

t: t.cpp

clean:
	rm -f t
	rm -rf doc

doxy: t.cpp
	doxygen
