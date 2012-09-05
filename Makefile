CXX         =   clang++
CXXFLAGS    =   -O0 -g -ggdb3\
                -W -Wall -Wshadow -Wformat\
                -Wsequence-point -Wunused\
		        -Wuninitialized -Wfloat-equal -ansi\
		        -pedantic -Weffc++
LDFLAGS     =   -lm

.PHONY: clean doxy all

all: t u doxy
	./tests/tester.sh ./t

t: t.cpp

u: u.cpp set.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(<:%.cpp=%.cpp) -o $@

clean:
	rm -f t u tags
	rm -rf doc

doxy: t.cpp u.cpp set.h
	doxygen
