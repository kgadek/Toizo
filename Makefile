CXX         =   g++
CXXFLAGS    =   -O0 -g -ggdb3 -pg\
                -W -Wall -Wshadow -Wformat\
                -Wsequence-point -Wunused -Wchar-subscripts\
		        -Wuninitialized -Wfloat-equal -ansi\
		        -pedantic -Weffc++
LDFLAGS     =   -lm

.PHONY: clean doxy all tagfile

all: t doxy tagfile
#	./tests/tester.sh ./t

t: t.cpp Unionfind-cpp/set.h n.h tagfile
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(<:%.cpp=%.cpp) -o $@

tagfile:
	ctags -R

clean:
	rm -f t tags
	rm -rf doc

doxy: t.cpp Unionfind-cpp/set.h
	doxygen
