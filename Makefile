CXX         =   clang++
CXXFLAGS    =   -O0 -g -ggdb3\
                -W -Wall -Wshadow -Wformat\
                -Wsequence-point -Wunused\
		        -Wuninitialized -Wfloat-equal -ansi\
		        -pedantic -Weffc++
LDFLAGS     =   -lm

.PHONY: clean doxy all

all: t doxy
#	./tests/tester.sh ./t

t: t.cpp Unionfind-cpp/set.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(<:%.cpp=%.cpp) -o $@

clean:
	rm -f t tags
	rm -rf doc

doxy: t.cpp Unionfind-cpp/set.h
	doxygen
